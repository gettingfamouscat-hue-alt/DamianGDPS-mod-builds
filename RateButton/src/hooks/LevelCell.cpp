#include <Geode/Geode.hpp>
#include <Geode/modify/LevelCell.hpp>
#include "../utils/CustomFace.hpp"
#include "../utils/Utils.hpp"

using namespace geode::prelude;

namespace {
    // Custom-face list cells (Impossible / Very Easy): bring VIEW left and
    // difficulty face right so the title isn't pinched between them.
    constexpr float kCustomViewShiftX = -22.f;
    constexpr float kCustomFaceShiftX = 6.f;
    constexpr float kImpossibleTextShiftX = 14.f;
    constexpr float kCustomNameShiftX = 4.f;
    constexpr float kCustomPreviewScale = 0.85f;
    // Stock name width is tight against the right-side VIEW/thumbnail.
    constexpr float kCustomNamePad = 72.f;
    constexpr char const *kLayoutNudgedFlag = "rb-custom-layout-nudged";
    constexpr char const *kPreviewNudgedFlag = "rb-custom-preview-nudged";

    void shiftNodeX(CCNode *node, float dx) {
        if (!node || dx == 0.f)
            return;
        node->setPositionX(node->getPositionX() + dx);
    }

    CCNode *findPreviewNode(CCNode *root, char const *sprId, char const *bareId) {
        if (auto *n = root->getChildByIDRecursive(sprId))
            return n;
        return root->getChildByIDRecursive(bareId);
    }
}

class $modify(MyLevelCell, LevelCell) {
    void applyCustomDifficultyFace() {
        if (!m_level)
            return;

        CCNode *host = nullptr;
        if (auto *main = m_mainLayer) {
            if (auto *container = main->getChildByID("difficulty-container"))
                host = container->getChildByID("difficulty-sprite");
        }
        if (!host)
            host = this->getChildByIDRecursive("difficulty-sprite");
        if (!host)
            return;

        CustomFace::applyForLevel(host, m_level);
    }

    void fitImpossibleColumn() {
        if (!Utils::isImpossibleDemonLevel(m_level)) return;
        auto* column = this->getChildByIDRecursive("difficulty-container");
        if (!column || !column->getParent()) return;
        auto* face = column->getChildByID(CustomFace::ID_IMPOSSIBLE);
        if (!face) return;
        float const left = 10.f;
        float right = m_width - 10.f;
        for (auto id : {"song-name", "length-icon", "length-label", "downloads-icon",
                "downloads-label", "likes-icon", "likes-label", "orbs-icon", "orbs-label",
                "level-name", "creator-name"}) {
            auto* node = this->getChildByIDRecursive(id);
            if (!node || !node->isVisible()) continue;
            auto const cellPoint = this->convertToNodeSpace(node->convertToWorldSpace({0.f, 0.f}));
            if (cellPoint.x > left) right = std::min(right, cellPoint.x - 7.f);
        }
        if (right <= left + 12.f) return;
        float const allowedWidth = right - left;
        auto* caption = column->getChildByID(CustomFace::ID_IMPOSSIBLE_LABEL);
        if (auto* label = typeinfo_cast<CCLabelBMFont*>(caption))
            label->limitLabelWidth(std::min(60.f, allowedWidth - 6.f), .3f, .08f);
        for (auto id : {CustomFace::ID_IMPOSSIBLE, CustomFace::ID_FEATURE_FIRE}) {
            auto* art = column->getChildByID(id);
            if (!art) continue;
            auto const p0 = this->convertToNodeSpace(art->convertToWorldSpace({0.f, 0.f}));
            auto const p1 = this->convertToNodeSpace(art->convertToWorldSpace({art->getContentSize().width, 0.f}));
            float const width = std::abs(p1.x - p0.x);
            if (width > allowedWidth - 4.f)
                art->setScale(art->getScale() * (allowedWidth - 4.f) / width);
        }
        auto const faceWorld = face->convertToWorldSpace({face->getContentSize().width * .5f,
            face->getContentSize().height * .5f});
        auto target = this->convertToNodeSpace(faceWorld);
        target.x = (left + right) * .5f;
        auto* parent = column->getParent();
        float const dx = parent->convertToNodeSpace(this->convertToWorldSpace(target)).x
            - parent->convertToNodeSpace(faceWorld).x;
        column->setPositionX(column->getPositionX() + dx);
    }

    void nudgeCustomPreview() {
        if (this->getChildByID(kPreviewNudgedFlag))
            return;

        auto nudge = [](CCNode *node) {
            if (!node)
                return;
            // Keep thumbnail with VIEW as it moves left.
            node->setScale(node->getScale() * kCustomPreviewScale);
            shiftNodeX(node, kCustomViewShiftX * 0.5f);
        };

        auto *clip = findPreviewNode(
            this, "cdc.level_thumbnails/clipping-node", "clipping-node"
        );
        auto *sep = findPreviewNode(
            this, "cdc.level_thumbnails/separator", "separator"
        );
        if (!clip && !sep)
            return;

        nudge(clip);
        nudge(sep);

        // One-shot marker so a delayed pass doesn't double-scale.
        auto *flag = CCNode::create();
        flag->setID(kPreviewNudgedFlag);
        this->addChild(flag);
    }

    // Impossible / Very Easy: face right, VIEW left â€” free title room without
    // widening the whole online list. Other difficulties stay stock.
    void applyCustomFaceLayout() {
        if (!m_level)
            return;
        if (!Utils::isImpossibleDemonLevel(m_level) && !Utils::isVeryEasyLevel(m_level))
            return;

        // Preserve the native song and stat rows for custom difficulties.
        for (auto id : {"song-name", "length-icon", "length-label", "downloads-icon",
                "downloads-label", "likes-icon", "likes-label", "orbs-icon", "orbs-label"}) {
            if (auto* node = this->getChildByIDRecursive(id)) node->setVisible(true);
        }

        if (!this->getChildByID(kLayoutNudgedFlag)) {
            if (auto *view = this->getChildByIDRecursive("view-button"))
                shiftNodeX(view, kCustomViewShiftX);

            // Shift face column right (stock sprite + custom overlay).
            CCNode *faceRoot = nullptr;
            if (auto *main = m_mainLayer)
                faceRoot = main->getChildByID("difficulty-container");
            if (!faceRoot)
                faceRoot = this->getChildByIDRecursive("difficulty-container");
            if (!faceRoot)
                faceRoot = this->getChildByIDRecursive("difficulty-sprite");
            if (!Utils::isImpossibleDemonLevel(m_level)) shiftNodeX(faceRoot, kCustomFaceShiftX);

            if (auto *label = typeinfo_cast<CCLabelBMFont *>(
                    this->getChildByIDRecursive("level-name")
                )) {
                shiftNodeX(label, Utils::isImpossibleDemonLevel(m_level)
                    ? kCustomNameShiftX + kImpossibleTextShiftX : kCustomNameShiftX);
            }
            if (Utils::isImpossibleDemonLevel(m_level)) {
                shiftNodeX(this->getChildByIDRecursive("creator-name"), kImpossibleTextShiftX);
                shiftNodeX(this->getChildByIDRecursive("completed-icon"), kImpossibleTextShiftX);
                shiftNodeX(this->getChildByIDRecursive("percentage-label"), kImpossibleTextShiftX);
                for (auto id : {"song-name", "length-icon", "length-label", "downloads-icon",
                        "downloads-label", "likes-icon", "likes-label", "orbs-icon", "orbs-label"})
                    shiftNodeX(this->getChildByIDRecursive(id), kImpossibleTextShiftX);
            }

            auto *flag = CCNode::create();
            flag->setID(kLayoutNudgedFlag);
            this->addChild(flag);
        }

        this->nudgeCustomPreview();
        // Level Thumbnails often attaches after loadCustomLevelCell returns.
        this->retain();
        Loader::get()->queueInMainThread([self = this]() {
            if (self) {
                self->nudgeCustomPreview();
                self->release();
            }
        });

        auto *label = typeinfo_cast<CCLabelBMFont *>(
            this->getChildByIDRecursive("level-name")
        );
        if (!label)
            return;

        float maxWidth = m_width - kCustomNamePad;
        if (maxWidth < 160.f)
            maxWidth = 160.f;
        label->limitLabelWidth(maxWidth, label->getScale(), 0.f);
        this->fitImpossibleColumn();
    }

    void loadCustomLevelCell() {
        LevelCell::loadCustomLevelCell();
        this->applyCustomDifficultyFace();
        this->applyCustomFaceLayout();
    }

    void loadFromLevel(GJGameLevel *level) {
        LevelCell::loadFromLevel(level);
        this->applyCustomDifficultyFace();
        this->applyCustomFaceLayout();
    }
};
