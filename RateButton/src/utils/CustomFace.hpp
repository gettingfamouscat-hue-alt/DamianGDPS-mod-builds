#pragma once

#include "Utils.hpp"

#include <algorithm>
#include <string>

using namespace geode::prelude;

namespace CustomFace {
    inline constexpr float CUSTOM_FACE_SCALE = 1.25f;
    inline constexpr float STOCK_FACE_DISPLAY = 62.f * 1.25f; // DevRateStarsPopup stock face
    // Extra stacked VERY/EASY line covers stock star row — push stars down.
    inline constexpr float VERY_EASY_STARS_NUDGE_Y = -19.f;

    inline constexpr char const *ID_VERY_EASY = "rb-very-easy-face";
    inline constexpr char const *ID_IMPOSSIBLE = "rb-impossible-face";
    inline constexpr char const *ID_VERY_EASY_LABEL = "rb-very-easy-face-label";
    inline constexpr char const *ID_VERY_EASY_LABEL_BOT = "rb-very-easy-face-label-bot";
    inline constexpr char const *ID_IMPOSSIBLE_LABEL = "rb-impossible-face-label";
    // Recreated featured/epic/legendary/mythic coin — stock fire lives on the
    // host GJDifficultySprite we hide, so we draw it ourselves behind the face.
    inline constexpr char const *ID_FEATURE_FIRE = "rb-feature-fire";
    inline constexpr char const *ID_STAR_NUDGE = "rb-ve-star-nudge";

    // Scale `face` so its longest side matches the host's on-screen box.
    // Tall "Long" difficulty sprites (face + baked label): match WIDTH so the
    // face circle stays normal size instead of shrinking to fit the full column.
    inline void scaleToHost(CCSprite *face, CCNode *host) {
        if (!face || !host)
            return;

        auto const hostSize = host->getContentSize();
        auto const faceSize = face->getContentSize();
        if (faceSize.width <= 0.f || faceSize.height <= 0.f)
            return;

        float const targetW = hostSize.width * host->getScaleX();
        float const targetH = hostSize.height * host->getScaleY();
        if (targetW <= 0.f || targetH <= 0.f)
            return;

        float scale;
        if (targetH > targetW * 1.15f)
            scale = targetW / faceSize.width;
        else
            scale = std::min(targetW / faceSize.width, targetH / faceSize.height);

        face->setScale(scale);
    }

    // Texture-swap path (rate popup / share face sprite). Keeps on-screen size
    // equal to the sprite's current displayed size before the swap.
    inline bool applyTextureKeepingSize(CCSprite *face, char const *sprName) {
        auto *custom = CCSprite::create(sprName);
        if (!face || !custom || !custom->getTexture())
            return false;

        float target = std::max(
            face->getContentSize().width * face->getScaleX(),
            face->getContentSize().height * face->getScaleY()
        );
        if (target < 8.f)
            target = STOCK_FACE_DISPLAY;

        auto *tex = custom->getTexture();
        auto const size = tex->getContentSize();
        if (size.width <= 0.f || size.height <= 0.f)
            return false;

        face->setTexture(tex);
        face->setTextureRect(CCRect{ 0.f, 0.f, size.width, size.height });
        auto const longest = std::max(size.width, size.height);
        face->setScale(target / longest);
        face->setColor({ 255, 255, 255 });
        face->setOpacity(255);
        return true;
    }

    inline void clearId(CCNode *parent, char const *nodeId) {
        if (!parent || !nodeId)
            return;
        if (auto *old = parent->getChildByID(nodeId))
            old->removeFromParent();
    }

    inline void clearOverlays(CCNode *parent, CCNode *hostToRestore) {
        if (!parent)
            return;
        clearId(parent, ID_VERY_EASY);
        clearId(parent, ID_IMPOSSIBLE);
        clearId(parent, ID_VERY_EASY_LABEL);
        clearId(parent, ID_VERY_EASY_LABEL_BOT);
        clearId(parent, ID_IMPOSSIBLE_LABEL);
        clearId(parent, ID_FEATURE_FIRE);
        if (hostToRestore)
            hostToRestore->setVisible(true);
    }

    inline float faceBottomY(CCSprite *face) {
        return face->getPositionY()
            - face->getContentSize().height * face->getScaleY()
                * face->getAnchorPoint().y;
    }

    // Featured yellow / epic blue / legendary yellow / mythic pink-red coins.
    inline char const *featureCoinFrame(GJGameLevel *level) {
        if (!level)
            return nullptr;
        int const epic = static_cast<int>(level->m_isEpic);
        if (epic >= 3)
            return "GJ_epicCoin3_001.png";
        if (epic == 2)
            return "GJ_epicCoin2_001.png";
        if (epic == 1)
            return "GJ_epicCoin_001.png";
        if (static_cast<int>(level->m_featured) >= 1)
            return "GJ_featuredCoin_001.png";
        return nullptr;
    }

    // Stock fire is a child of GJDifficultySprite; hiding the host kills it.
    // Redraw the coin behind our custom face so tiers stay colored.
    inline void addFeatureFire(CCNode *parent, CCNode *host, CCSprite *face, GJGameLevel *level) {
        if (!parent || !host || !face)
            return;
        clearId(parent, ID_FEATURE_FIRE);

        char const *frame = featureCoinFrame(level);
        if (!frame)
            return;

        auto *fire = CCSprite::createWithSpriteFrameName(frame);
        if (!fire)
            return;

        scaleToHost(fire, host);
        fire->setScale(fire->getScale() * CUSTOM_FACE_SCALE);
        fire->setPosition(face->getPosition());
        if (Utils::isVeryEasyLevel(level)) {
            // The upper part of the PNG is a halo, not the round face.
            // Let the visible fire ring sit just outside the entire Very Easy face.
            float desiredWidth = face->getScaledContentSize().width * 1.08f;
            if (fire->getContentSize().width > 0.f)
                fire->setScale(desiredWidth / fire->getContentSize().width);
            fire->setPositionY(face->getPositionY()
                - face->getScaledContentSize().height * 0.16f);
        }
        fire->setAnchorPoint(host->getAnchorPoint());
        fire->setZOrder(host->getZOrder()); // face is +1 → fire sits behind
        fire->setColor({ 255, 255, 255 });
        fire->setOpacity(255);
        fire->setID(ID_FEATURE_FIRE);
        parent->addChild(fire);
    }

    // Idempotent nudge of stars (and diamonds) under the difficulty column.
    // Marker lives on searchRoot — never addChild onto stars-label/icon (those
    // can be batch-node sprites; CCSpriteBatchNode::addChild crashes on a
    // plain CCNode — see crash at CustomFace.hpp setStarRowNudge).
    inline void setStarRowNudge(CCNode *searchRoot, bool nudgeDown) {
        if (!searchRoot)
            return;

        auto apply = [&](char const *nodeId) {
            auto *node = searchRoot->getChildByIDRecursive(nodeId);
            if (!node)
                return;

            std::string const markerId = std::string(ID_STAR_NUDGE) + "-" + nodeId;
            auto *marker = searchRoot->getChildByID(markerId);
            if (nudgeDown) {
                if (marker)
                    return;
                node->setPositionY(node->getPositionY() + VERY_EASY_STARS_NUDGE_Y);
                auto *flag = CCNode::create();
                if (!flag)
                    return;
                flag->setID(markerId);
                searchRoot->addChild(flag);
            } else if (marker) {
                node->setPositionY(node->getPositionY() - VERY_EASY_STARS_NUDGE_Y);
                marker->removeFromParent();
            }
        };

        apply("stars-label");
        apply("stars-icon");
        apply("diamond-label");
        apply("diamond-icon");
    }

    // Single-line caption under a custom face (e.g. IMPOSSIBLE).
    inline CCLabelBMFont *addCaption(
        CCNode *parent,
        CCSprite *face,
        char const *text,
        char const *labelId,
        float yBelowFace = 6.f
    ) {
        if (!parent || !face || !text || !*text)
            return nullptr;

        auto *label = CCLabelBMFont::create(text, "bigFont.fnt");
        label->setScale(0.3f);
        label->limitLabelWidth(72.f, 0.3f, 0.18f);
        label->setID(labelId);
        label->setZOrder(face->getZOrder());
        label->setColor({ 255, 255, 255 });
        label->setOpacity(255);
        label->setPosition({ face->getPositionX(), faceBottomY(face) - yBelowFace });
        parent->addChild(label);
        return label;
    }

    // Very Easy: VERY on top, EASY underneath (not a smashed "VERY EASY").
    inline void addVeryEasyStackedCaption(CCNode *parent, CCSprite *face) {
        if (!parent || !face)
            return;

        auto *very = CCLabelBMFont::create("VERY", "bigFont.fnt");
        very->setScale(0.28f);
        very->limitLabelWidth(72.f, 0.28f, 0.16f);
        very->setID(ID_VERY_EASY_LABEL);
        very->setZOrder(face->getZOrder());
        very->setColor({ 255, 255, 255 });
        very->setOpacity(255);

        auto *easy = CCLabelBMFont::create("EASY", "bigFont.fnt");
        easy->setScale(0.3f);
        easy->limitLabelWidth(72.f, 0.3f, 0.18f);
        easy->setID(ID_VERY_EASY_LABEL_BOT);
        easy->setZOrder(face->getZOrder());
        easy->setColor({ 255, 255, 255 });
        easy->setOpacity(255);

        float const baseY = faceBottomY(face);
        // VERY sits where stock Easy caption would; EASY tucked under it.
        very->setPosition({ face->getPositionX(), baseY - 5.f });
        easy->setPosition({
            face->getPositionX(),
            very->getPositionY()
                - very->getContentSize().height * very->getScaleY() * 0.55f
                - 2.f
        });

        parent->addChild(very);
        parent->addChild(easy);
    }

    // Overlay custom PNG on top of a stock difficulty sprite; hide host.
    // Optional caption replaces baked "EASY"/"DEMON" text on Long cell sprites.
    // Pass level so featured/epic fire can be redrawn behind the custom face.
    inline CCSprite *applyOverlay(
        CCNode *host,
        char const *sprName,
        char const *nodeId,
        char const *labelId,
        char const *caption = nullptr,
        GJGameLevel *level = nullptr
    ) {
        if (!host || !host->getParent())
            return nullptr;

        auto *parent = host->getParent();
        clearOverlays(parent, host);

        auto *face = CCSprite::create(sprName);
        if (!face || !face->getTexture()) {
            log::warn("Rate Button: failed to load custom face {}", sprName);
            return nullptr;
        }

        scaleToHost(face, host);
        float const originalHeight = face->getScaledContentSize().height;
        face->setScale(face->getScale() * CUSTOM_FACE_SCALE);
        // Grow upward from the old bottom edge, preserving caption/star spacing.
        face->setPosition({host->getPositionX(), host->getPositionY()
            + (face->getScaledContentSize().height - originalHeight) * host->getAnchorPoint().y});
        face->setAnchorPoint(host->getAnchorPoint());
        face->setZOrder(host->getZOrder() + 1);
        face->setColor({ 255, 255, 255 });
        face->setOpacity(255);
        face->setID(nodeId);
        parent->addChild(face);

        // Fire before hiding host so we can copy layout from the live sprite.
        addFeatureFire(parent, host, face, level);
        host->setVisible(false);

        if (caption && *caption)
            addCaption(parent, face, caption, labelId);

        return face;
    }

    inline bool applyForLevel(CCNode *host, GJGameLevel *level) {
        if (!host || !level)
            return false;
        // Do NOT gate on isOnGdps here. Very Easy / Impossible are encoded in
        // demonDiff flags that never appear on official servers. Mac URL-offset
        // detection is flaky — faces must still show when the binary loads.

        auto *parent = host->getParent();

        if (Utils::isImpossibleDemonLevel(level)) {
            applyOverlay(
                host,
                "impossible_demon.png"_spr,
                ID_IMPOSSIBLE,
                ID_IMPOSSIBLE_LABEL,
                "IMPOSSIBLE",
                level
            );
            setStarRowNudge(parent, false);
            return true;
        }

        if (Utils::isVeryEasyLevel(level)) {
            auto *face = applyOverlay(
                host,
                "very_easy.png"_spr,
                ID_VERY_EASY,
                ID_VERY_EASY_LABEL,
                nullptr,
                level
            );
            if (face && parent)
                addVeryEasyStackedCaption(parent, face);
            setStarRowNudge(parent, face != nullptr);
            return face != nullptr;
        }

        if (parent)
            clearOverlays(parent, host);
        setStarRowNudge(parent, false);
        return false;
    }
}
