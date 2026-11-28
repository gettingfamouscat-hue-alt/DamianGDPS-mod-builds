#include "../src/InstallTransaction.hpp"
#include <chrono>
#include <iostream>
using namespace updater::install;
void require(bool ok,char const* reason) { if (!ok) throw std::runtime_error(reason); }
void write(fs::path const& p,std::string const& data) { fs::create_directories(p.parent_path()); std::ofstream f(p,std::ios::binary);f<<data;f.close();require(bool(f),"Fixture write failed"); }
std::string read(fs::path const& p) { std::ifstream f(p,std::ios::binary); return {std::istreambuf_iterator<char>(f),{}}; }
int main() {
    auto temp=fs::canonical(fs::temp_directory_path());
    auto root=temp/("damian-updater-test-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    fs::create_directory(root);
    try {
        auto mods=root/"Mods",stage=root/"Stage",saves=root/"Saves";
        fs::create_directories(mods);fs::create_directory(stage);
        write(mods/"owner.first.geode","old first");write(mods/"first-renamed.geode","duplicate old first");
        write(mods/"unrelated.other.geode","leave this alone");
        write(saves/"CCLocalLevels.dat","unsaved-looking editor progress");write(saves/"settings.json","keep my settings");
        write(stage/"first.geode","new first");write(stage/"required.geode","new dependency");
        std::vector<Package> plan{{"owner.first",stage/"first.geode",{mods/"owner.first.geode",mods/"first-renamed.geode"}},
            {"owner.required",stage/"required.geode",{}}};
        // Failure after the second write must restore overwritten and newly created paths.
        bool failed=false;
        try { commit(mods,root/"rollback",plan,[](size_t n) {if(n==2)throw std::runtime_error("Simulated disk failure");}); }
        catch(std::exception const&) {failed=true;}
        require(failed,"Fault injection did not run");
        require(read(mods/"owner.first.geode")=="old first","Overwritten package was not restored");
        require(read(mods/"first-renamed.geode")=="duplicate old first","Alias was lost during rollback");
        require(!fs::exists(mods/"owner.required.geode"),"New package was not rolled back");
        write(stage/"first.geode","new first");write(stage/"required.geode","new dependency");
        commit(mods,root/"backup",plan);
        require(read(mods/"owner.first.geode")=="new first","Update failed");
        require(read(mods/"owner.required.geode")=="new dependency","Required package missing");
        require(!fs::exists(mods/"first-renamed.geode"),"Duplicate alias was not removed");
        require(read(root/"backup"/"0.geode")=="old first","Backup missing");
        require(read(root/"backup"/"1.geode")=="duplicate old first","Alias backup missing");
        require(fs::exists(root/"backup"/"complete.txt"),"Successful update missing record");
        require(read(mods/"unrelated.other.geode")=="leave this alone","Unrelated mod changed");
        require(read(saves/"CCLocalLevels.dat")=="unsaved-looking editor progress" && read(saves/"settings.json")=="keep my settings","Save data changed");
        write(stage/"unsafe.geode","bad");write(root/"outside.geode","outside");
        failed=false;
        try {commit(mods,root/"unsafe-backup",{{"owner.bad",stage/"unsafe.geode",{root/"outside.geode"}}});}catch(std::exception const&){failed=true;}
        require(failed && read(root/"outside.geode")=="outside" && !fs::exists(mods/"owner.bad.geode"),"Path escape was not rejected");
        for (auto const& id:{"../bad","owner/escape","owner:escape",".bad","bad.","owner.two.dots",""}) require(!safeID(id),"Unsafe mod ID accepted");
        require(safeID("owner.valid-mod_2"),"Valid ID rejected");
        // Before recursive cleanup, prove this is our fresh child of the system temp folder.
        require(fs::canonical(root.parent_path())==temp && root.filename().string().starts_with("damian-updater-test-"),"Cleanup path escaped fixture");
        fs::remove_all(root);
        std::cout<<"PASS: reinstall, dependency install, duplicate cleanup, complete backups, injected mid-install rollback, path safety, unrelated mods and save/settings preservation.\n";
    } catch(std::exception const& e) {std::cerr<<e.what()<<"; fixtures retained at "<<root<<'\n';return 1;}
}
