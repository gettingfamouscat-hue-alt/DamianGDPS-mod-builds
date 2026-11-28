#include "hooks/DynamicObjectAction.hpp"
#include "Geode/binding/GameObject.hpp"
#include "hooks/PlayLayer.hpp"
#include "util/debug.hpp"

using namespace geode::prelude;
using namespace persistenceAPI;

void PADynamicObjectAction::load(Stream& i_stream) {
    i_stream >> *this;
}

void PADynamicObjectAction::save(Stream& o_stream) {
    o_stream << *this;
}

inline void persistenceAPI::operator>>(Stream& i_stream, PADynamicObjectAction& o_value) {
    i_stream >> o_value.m_trigger;
    SEPARATOR_I
    i_stream >> o_value.m_centerObject;
    SEPARATOR_I
    i_stream >> o_value.m_aimTargetObject;
    SEPARATOR_I
    i_stream >> o_value.m_targetObject;
    SEPARATOR_I
    i_stream >> o_value.m_minXObject;
    SEPARATOR_I
    i_stream >> o_value.m_minYObject;
    SEPARATOR_I
    i_stream >> o_value.m_maxXObject;
    SEPARATOR_I
    i_stream >> o_value.m_maxYObject;
    SEPARATOR_I
    if (i_stream.getPAVersion() > 1) {
        i_stream >> o_value.m_elapsed;
        SEPARATOR_I
        i_stream >> o_value.m_easedElapsed;
        SEPARATOR_I
        i_stream >> o_value.m_easedProgress;
        SEPARATOR_I
        i_stream >> o_value.m_justStarted;
        SEPARATOR_I
        i_stream >> o_value.m_stopped;
        SEPARATOR_I
        i_stream >> o_value.m_paused;
        SEPARATOR_I
        i_stream >> o_value.m_pausePending;
        SEPARATOR_I
        i_stream >> o_value.m_controlID;
        SEPARATOR_I
        i_stream >> o_value.m_targetGroupID;
        SEPARATOR_I
        i_stream >> o_value.m_centerGroupID;
    } else {
        i_stream.read(reinterpret_cast<char*>(&o_value) + offsetof(PADynamicObjectAction,m_maxYObject) + sizeof(GameObject*), 32);
    }
    SEPARATOR_I
}

inline void persistenceAPI::operator<<(Stream& o_stream, PADynamicObjectAction& i_value) {
    o_stream << i_value.m_trigger;
    SEPARATOR_O
    o_stream << i_value.m_centerObject;
    SEPARATOR_O
    o_stream << i_value.m_aimTargetObject;
    SEPARATOR_O
    o_stream << i_value.m_targetObject;
    SEPARATOR_O
    o_stream << i_value.m_minXObject;
    SEPARATOR_O
    o_stream << i_value.m_minYObject;
    SEPARATOR_O
    o_stream << i_value.m_maxXObject;
    SEPARATOR_O
    o_stream << i_value.m_maxYObject;
    SEPARATOR_O
    o_stream << i_value.m_elapsed;
    SEPARATOR_O
    o_stream << i_value.m_easedElapsed;
    SEPARATOR_O
    o_stream << i_value.m_easedProgress;
    SEPARATOR_O
    o_stream << i_value.m_justStarted;
    SEPARATOR_O
    o_stream << i_value.m_stopped;
    SEPARATOR_O
    o_stream << i_value.m_paused;
    SEPARATOR_O
    o_stream << i_value.m_pausePending;
    SEPARATOR_O
    o_stream << i_value.m_controlID;
    SEPARATOR_O
    o_stream << i_value.m_targetGroupID;
    SEPARATOR_O
    o_stream << i_value.m_centerGroupID;
    SEPARATOR_O
}

#if defined(PA_DEBUG) && defined(PA_DESCRIBE)
void PADynamicObjectAction::describe() {
    int l_object1Index = -1;
    int l_object2Index = -1;
    int l_object3Index = -1;
    int l_object4Index = -1;
    int l_object5Index = -1;
    int l_object6Index = -1;
    int l_object7Index = -1;
    int l_object8Index = -1;
    if (!m_trigger) {
        log::info("[PADynamicObjectAction - describe] no game object?? @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
    } else {
        PAPlayLayer* l_playLayer = static_cast<PAPlayLayer*>(PlayLayer::get());
        if (l_playLayer) l_object1Index = l_playLayer->getGameObjectIndex(m_trigger);
    }
    if (!m_centerObject) {
        log::info("[PADynamicObjectAction - describe] no game object?? @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
    } else {
        PAPlayLayer* l_playLayer = static_cast<PAPlayLayer*>(PlayLayer::get());
        if (l_playLayer) l_object2Index = l_playLayer->getGameObjectIndex(m_centerObject);
    }
    if (!m_aimTargetObject) {
        log::info("[PADynamicObjectAction - describe] no game object?? @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
    } else {
        PAPlayLayer* l_playLayer = static_cast<PAPlayLayer*>(PlayLayer::get());
        if (l_playLayer) l_object3Index = l_playLayer->getGameObjectIndex(m_aimTargetObject);
    }
    if (!m_targetObject) {
        log::info("[PADynamicObjectAction - describe] no game object?? @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
    } else {
        PAPlayLayer* l_playLayer = static_cast<PAPlayLayer*>(PlayLayer::get());
        if (l_playLayer) l_object4Index = l_playLayer->getGameObjectIndex(m_targetObject);
    }
    if (!m_minXObject) {
        log::info("[PADynamicObjectAction - describe] no game object?? @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
    } else {
        PAPlayLayer* l_playLayer = static_cast<PAPlayLayer*>(PlayLayer::get());
        if (l_playLayer) l_object5Index = l_playLayer->getGameObjectIndex(m_minXObject);
    }
    if (!m_minYObject) {
        log::info("[PADynamicObjectAction - describe] no game object?? @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
    } else {
        PAPlayLayer* l_playLayer = static_cast<PAPlayLayer*>(PlayLayer::get());
        if (l_playLayer) l_object6Index = l_playLayer->getGameObjectIndex(m_minYObject);
    }
    if (!m_maxXObject) {
        log::info("[PADynamicObjectAction - describe] no game object?? @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
    } else {
        PAPlayLayer* l_playLayer = static_cast<PAPlayLayer*>(PlayLayer::get());
        if (l_playLayer) l_object7Index = l_playLayer->getGameObjectIndex(m_maxXObject);
    }
    if (!m_maxYObject) {
        log::info("[PADynamicObjectAction - describe] no game object?? @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
    } else {
        PAPlayLayer* l_playLayer = static_cast<PAPlayLayer*>(PlayLayer::get());
        if (l_playLayer) l_object8Index = l_playLayer->getGameObjectIndex(m_maxYObject);
    }

    log::info("[PADynamicObjectAction - describe] l_object1Index: {}", l_object1Index);
    log::info("[PADynamicObjectAction - describe] l_object2Index: {}", l_object2Index);
    log::info("[PADynamicObjectAction - describe] l_object3Index: {}", l_object3Index);
    log::info("[PADynamicObjectAction - describe] l_object4Index: {}", l_object4Index);
    log::info("[PADynamicObjectAction - describe] l_object5Index: {}", l_object5Index);
    log::info("[PADynamicObjectAction - describe] l_object6Index: {}", l_object6Index);
    log::info("[PADynamicObjectAction - describe] l_object7Index: {}", l_object7Index);
    log::info("[PADynamicObjectAction - describe] l_object8Index: {}", l_object8Index);
    log::info("[PADynamicObjectAction - describe] m_elapsed: {}", m_elapsed);
    log::info("[PADynamicObjectAction - describe] m_easedElapsed: {}", m_easedElapsed);
    log::info("[PADynamicObjectAction - describe] m_easedProgress: {}", m_easedProgress);
    log::info("[PADynamicObjectAction - describe] m_justStarted: {}", m_justStarted);
    log::info("[PADynamicObjectAction - describe] m_stopped: {}", m_stopped);
    log::info("[PADynamicObjectAction - describe] m_paused: {}", m_paused);
    log::info("[PADynamicObjectAction - describe] m_pausePending: {}", m_pausePending);
    log::info("[PADynamicObjectAction - describe] m_controlID: {}", m_controlID);
    log::info("[PADynamicObjectAction - describe] m_targetGroupID: {}", m_targetGroupID);
    log::info("[PADynamicObjectAction - describe] m_centerGroupID: {}", m_centerGroupID);
}
#endif