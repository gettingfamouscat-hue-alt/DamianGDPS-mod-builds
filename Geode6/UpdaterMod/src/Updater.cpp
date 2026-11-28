#include "Updater.hpp"
#include "InstallTransaction.hpp"
#include <Geode/utils/crypto.hpp>
#include <Geode/utils/file.hpp>
#include <Geode/utils/web.hpp>
#include <asp/fs/std.hpp>
#include <chrono>
#include <set>
#include <thread>

using namespace geode::prelude;
namespace updater {
namespace {
namespace fs = std::filesystem;
using JSON = matjson::Value;
constexpr auto repository = "https://api.github.com/repos/gettingfamouscat-hue-alt/DamianGDPS-mod-builds/releases/latest";
constexpr auto releases = "https://github.com/gettingfamouscat-hue-alt/DamianGDPS-mod-builds/releases/download/";
std::atomic<bool> active{false};
std::string platform() {
#if defined(GEODE_IS_WINDOWS)
    return "Windows";
#elif defined(GEODE_IS_MACOS)
    return "Mac";
#elif defined(GEODE_IS_ANDROID64)
    return "android64";
#elif defined(GEODE_IS_ANDROID32)
    return "android32";
#else
    throw std::runtime_error("This device is not supported by the GDPS pack.");
#endif
}
asp::fs::Path path(fs::path const& p) { return asp::fs::Path::fromStdPath(p); }
bool hashOK(std::string const& hash) {
    return hash.size()==64 && std::all_of(hash.begin(),hash.end(),[](char c) { return (c>='0'&&c<='9') || (c>='a'&&c<='f'); });
}
std::string string(JSON const& value, char const* key) {
    auto result=value[key].asString();
    if (!result) throw std::runtime_error(std::string("The update has invalid ")+key+" information.");
    return result.unwrap();
}
void cancelled(std::shared_ptr<State> const& s) {
    if (s->phase==1) throw std::runtime_error("Update cancelled. Installed mods were not changed.");
}
ByteVector download(std::string const& url, size_t maxSize, std::shared_ptr<State> const& s, bool showProgress=false) {
    cancelled(s);
    if (!url.starts_with("https://")) throw std::runtime_error("The download link is not secure.");
    auto request=web::WebRequest(); request.userAgent("Damian-GDPS-Updater/1.0");
    request.header("Cache-Control","no-cache"); request.timeout(asp::time::Duration::fromSecs(120));
    if (showProgress) request.onProgress([s](web::WebProgress const& p) {
        if (auto percent=p.downloadProgress()) s->percentage=static_cast<int>(*percent);
    });
    auto response=request.getSync(url);
    cancelled(s);
    if (!response.ok()) throw std::runtime_error("Download failed (HTTP "+std::to_string(response.code())+"). Please try again.");
    if (response.data().empty() || response.data().size()>maxSize) throw std::runtime_error("The download has an unexpected size.");
    return std::move(response).data();
}
JSON json(ByteVector const& data) {
    auto value=matjson::parse(std::string(data.begin(),data.end()));
    if (!value || !value.unwrap().isObject()) throw std::runtime_error("The update server returned invalid information.");
    return value.unwrap();
}
void verify(ByteVector const& data, std::string const& expected) {
    if (!hashOK(expected) || geode::sha256(data).toString()!=expected)
        throw std::runtime_error("A download failed its integrity check. Installed mods were not changed.");
}
struct Candidate { fs::path staged; ModMetadata metadata; };
Candidate inspect(fs::path const& source, std::string const& id, std::string const& expectedVersion={}) {
    auto metadata=ModMetadata::createFromGeodeFile(path(source));
    if (metadata.hasErrors() || metadata.getID()!=id || (!expectedVersion.empty() && metadata.getVersion().toVString()!=expectedVersion))
        throw std::runtime_error("Invalid mod package: "+id+".");
    if (!metadata.checkPlatformSupported() || !metadata.checkTargetVersions())
        throw std::runtime_error(id+" does not support this Geometry Dash / Geode version.");
    auto zip=file::Unzip::create(path(source));
    if (!zip || !zip.unwrap().hasEntry(asp::fs::Path(std::string(metadata.getBinaryName())))) throw std::runtime_error(id+" is missing its native module.");
    return {source,std::move(metadata)};
}
struct Asset { std::string url, hash; };
Asset asset(JSON const& release, std::string const& name) {
    if (!release["assets"].isArray()) throw std::runtime_error("The release has no downloads.");
    for (auto const& item:release["assets"].asArray().unwrap()) {
        if (item["name"].asString().unwrapOr("")!=name) continue;
        auto url=string(item,"browser_download_url");
        if (!url.starts_with(releases)) throw std::runtime_error("The release download is from an unexpected source.");
        auto digest=string(item,"digest");
        if (!digest.starts_with("sha256:")) throw std::runtime_error("The release is missing its integrity information.");
        return {url,digest.substr(7)};
    }
    throw std::runtime_error("The latest release does not contain "+name+".");
}
struct Context {
    fs::path mods, temp, backups;
    std::string gd;
    VersionInfo loader;
    std::map<std::string,ModMetadata> installed;
    std::map<std::string,fs::path> locations;
    std::set<std::string> enabled;
};
void run(Context context, std::shared_ptr<State> s) {
    fs::path stage;
    try {
        auto release=json(download(repository,2*1024*1024,s));
        auto manifestAsset=asset(release,"release-manifest.json");
        auto manifestBytes=download(manifestAsset.url,1024*1024,s); verify(manifestBytes,manifestAsset.hash);
        auto manifest=json(manifestBytes);
        auto target=platform();
        if (manifest["gd"].asString().unwrapOr("")!=context.gd || manifest["geodeMajor"].asInt().unwrapOr(0)!=static_cast<int64_t>(context.loader.getMajor()))
            throw std::runtime_error("The latest GDPS pack targets a different game or Geode version. Update Geometry Dash and Geode first.");
        auto info=manifest["platforms"][target];
        if (!info["mods"].isArray() || info["mods"].size()<2 || info["mods"].size()>64) throw std::runtime_error("This device has no valid mod pack.");
        auto archive=asset(release,"DamianGDPS-mods-"+target+"-GD"+context.gd+".zip");
        if (archive.hash!=string(info,"archiveSHA256")) throw std::runtime_error("The pack manifest does not match its release.");
        s->status("Downloading the latest GDPS mod pack...");
        auto bytes=download(archive.url,192*1024*1024,s,true); verify(bytes,archive.hash);
        s->status("Checking the downloaded mods...");
        fs::create_directories(context.temp);
        auto transaction=std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        stage=context.temp/transaction; fs::create_directory(stage);
        auto zipResult=file::Unzip::create(bytes);
        if (!zipResult) throw std::runtime_error("Could not open the mod pack.");
        auto zip=std::move(zipResult).unwrap();
        std::map<std::string,std::pair<std::string,std::string>> expected;
        for (auto const& item:info["mods"].asArray().unwrap()) {
            auto id=string(item,"id"), hash=string(item,"sha256"), version=string(item,"version");
            if (!install::safeID(id) || id=="geode.loader" || !hashOK(hash) || !expected.emplace(id,std::make_pair(hash,version)).second)
                throw std::runtime_error("The mod pack contains invalid or repeated IDs.");
        }
        if (!expected.contains("damian.mod-updater")) throw std::runtime_error("This pack does not include the updater. Please install a newer complete pack.");
        std::map<std::string,Candidate> plan;
        size_t totalSize=0;
        for (auto const& entry:zip.getEntries()) {
            std::string name=entry.string();
            if (!name.ends_with(".geode")) continue;
            if (!install::safeName(name)) throw std::runtime_error("The mod pack contains an invalid file path.");
            cancelled(s);
            auto extracted=zip.extract(entry);
            if (!extracted) throw std::runtime_error("A mod package could not be extracted.");
            auto data=std::move(extracted).unwrap(); totalSize+=data.size();
            if (totalSize>256*1024*1024 || data.size()>64*1024*1024) throw std::runtime_error("The extracted mod pack is too large.");
            auto temporary=stage/name;
            if (!file::writeBinary(path(temporary),data)) throw std::runtime_error("Could not stage a mod package. Check free space.");
            auto metadata=ModMetadata::createFromGeodeFile(path(temporary));
            std::string id=metadata.getID();
            if (!expected.contains(id) || plan.contains(id)) throw std::runtime_error("The mod pack contains an unexpected or repeated package.");
            verify(data,expected.at(id).first);
            plan.emplace(id,inspect(temporary,id,expected.at(id).second));
        }
        if (plan.size()!=expected.size()) throw std::runtime_error("The downloaded pack is missing mods.");
        // Resolve required dependencies before touching any installed files.
        std::set<std::string> visited;
        while (visited.size()<plan.size()) {
            if (plan.size()>128) throw std::runtime_error("The dependency list is too large.");
            auto next=std::find_if(plan.begin(),plan.end(),[&](auto const& p) { return !visited.contains(p.first); });
            if (next==plan.end()) break;
            auto id=next->first; visited.insert(id);
            auto dependencies=next->second.metadata.getDependencies();
            for (auto const& dependency:dependencies) {
                if (!dependency.isRequired()) continue;
                auto dep=dependency.getID();
                if (dep=="geode.loader") {
                    if (!dependency.getVersion().compare(context.loader)) throw std::runtime_error("A mod requires a different Geode version.");
                    continue;
                }
                if (!install::safeID(dep)) throw std::runtime_error("Invalid required dependency.");
                if (!plan.contains(dep)) {
                    s->status("Downloading required mod: "+dep);
                    auto url="https://api.geode-sdk.org/v1/mods/"+dep+"/versions/latest?gd="+context.gd+"&platforms="+GEODE_PLATFORM_SHORT_IDENTIFIER+
                        (dependency.getVersion().getComparison()==VersionCompare::Any ? "" : "&major="+std::to_string(dependency.getVersion().getUnderlyingVersion().getMajor()));
                    auto response=json(download(url,1024*1024,s)); auto payload=response["payload"];
                    if (payload["mod_id"].asString().unwrapOr("")!=dep) throw std::runtime_error("Could not find a compatible dependency: "+dep);
                    auto link=string(payload,"download_link");
                    if (!link.starts_with("https://api.geode-sdk.org/v1/mods/"+dep+"/versions/")) throw std::runtime_error("Unexpected dependency download source.");
                    auto data=download(link,64*1024*1024,s,true); verify(data,string(payload,"hash"));
                    auto destination=stage/(dep+".geode");
                    if (!file::writeBinary(path(destination),data)) throw std::runtime_error("Could not stage "+dep+".");
                    plan.emplace(dep,inspect(destination,dep));
                }
                if (!dependency.getVersion().compare(plan.at(dep).metadata.getVersion())) throw std::runtime_error("The mod pack has conflicting requirements for "+dep+".");
            }
        }
        // Check active mods too: a shared dependency update must not break another mod.
        auto effective=context.installed;
        std::set<std::string> willEnable=context.enabled;
        for (auto const& [id,candidate]:plan) {
            effective.insert_or_assign(id,candidate.metadata);
            if (!context.installed.contains(id)) willEnable.insert(id);
        }
        bool added=true;
        while (added) {
            added=false; auto roots=willEnable;
            for (auto const& id:roots) if (effective.contains(id)) for (auto const& dep:effective.at(id).getDependencies())
                if (dep.isRequired() && dep.getID()!="geode.loader" && willEnable.insert(dep.getID()).second) added=true;
        }
        for (auto const& id:willEnable) {
            if (!effective.contains(id)) throw std::runtime_error("A required installed mod is missing: "+id+".");
            auto const& metadata=effective.at(id);
            for (auto const& dep:metadata.getDependencies()) if (dep.isRequired()) {
                auto version=dep.getID()=="geode.loader" ? context.loader : effective.contains(dep.getID()) ? effective.at(dep.getID()).getVersion() : VersionInfo();
                if (!dep.getVersion().compare(version)) throw std::runtime_error("The update would break a dependency of "+id+". Installed mods were not changed.");
            }
            for (auto const& bad:metadata.getIncompatibilities())
                if (bad.isBreaking() && willEnable.contains(bad.getID()) && effective.contains(bad.getID()) && bad.getVersion().compare(effective.at(bad.getID()).getVersion()))
                    throw std::runtime_error(id+" is incompatible with "+bad.getID()+". Disable the conflicting mod before updating.");
        }
        for (auto const& [id,candidate]:plan) if (context.locations.contains(id))
            if (fs::is_symlink(context.locations.at(id)) || fs::canonical(context.locations.at(id).parent_path())!=fs::canonical(context.mods))
                throw std::runtime_error(id+" is installed outside the default Mods folder. Move its package there before updating.");
        int expectedPhase=0;
        if (!s->phase.compare_exchange_strong(expectedPhase,2)) { cancelled(s); throw std::runtime_error("Could not start installation."); }
        s->status("Installing mods and keeping a backup...");
        std::map<std::string,std::vector<fs::path>> old;
        for (auto const& entry:fs::directory_iterator(context.mods)) {
            if (entry.path().extension()!=".geode") continue;
            if (fs::is_symlink(entry.path())) {
                if (plan.contains(entry.path().stem().string())) throw std::runtime_error("An updated mod is a linked package. Move it into Mods before updating.");
                continue;
            }
            auto metadata=ModMetadata::createFromGeodeFile(path(entry.path()));
            if (!metadata.wasCompletelyUnparseable()) old[std::string(metadata.getID())].push_back(entry.path());
            if (plan.contains(entry.path().stem().string()) && !metadata.wasCompletelyUnparseable() && std::string(metadata.getID())!=entry.path().stem().string())
                throw std::runtime_error("A mod filename belongs to a different mod. Rename it before updating.");
        }
        std::vector<install::Package> packages;
        for (auto const& [id,candidate]:plan) packages.push_back({id,candidate.staged,old[id]});
        install::commit(context.mods,context.backups/transaction,packages);
        for (auto const& id:willEnable) if (!context.enabled.contains(id) && context.installed.contains(id)) s->enable.push_back(id);
        s->status("Mods updated. Restarting Geometry Dash...");
    } catch (std::exception const& error) {
        log::error("GDPS mod update stopped: {}",error.what());
        std::lock_guard lock(s->mutex); s->error=error.what();
    } catch (...) {
        std::lock_guard lock(s->mutex); s->error="The update could not finish. Please try again.";
    }
    // Only our fresh, checked staging directory is removed; backups and saves stay.
    if (!stage.empty() && stage.parent_path()==context.temp) { std::error_code ignored; fs::remove_all(stage,ignored); }
    s->done=true; active=false;
}
}
bool busy() { return active; }
std::shared_ptr<State> start() {
    if (active.exchange(true)) return nullptr;
    auto s=std::make_shared<State>();
    try {
        Context context;
        context.mods=dirs::getModsDir().toStdPath().unwrap();
        context.temp=(dirs::getTempDir()/"damian-mod-updater").toStdPath().unwrap();
        context.backups=(dirs::getGeodeSaveDir()/"mod-update-backups").toStdPath().unwrap();
        context.gd=Loader::get()->getGameVersion(); context.loader=Loader::get()->getVersion();
        for (auto* mod:Loader::get()->getAllMods()) {
            if (mod->getID()=="geode.loader") continue;
            context.installed.emplace(mod->getID(),mod->getMetadata());
            context.locations.emplace(mod->getID(),mod->getPackagePath().toStdPath().unwrap());
            if (mod->isOrWillBeEnabled()) context.enabled.insert(mod->getID());
        }
        std::thread([context=std::move(context),s]() mutable { run(std::move(context),s); }).detach();
    } catch (...) { active=false; s->error="Could not start the update."; s->done=true; }
    return s;
}
}
