#pragma once
#include "Geode/Geode.hpp"
#include "Geode/modify/PlayLayer.hpp"
#include "../import_export.hpp"

namespace persistenceAPI {
    class $modify(PAPlayLayer, PlayLayer) {
    public:
        struct Fields {
            int m_uniqueIDBase = 12;
        };

        // overrides

        $override
        SABE_PA_DLL bool init(GJGameLevel* i_level, bool i_dontCreateObjects);

        // custom methods

        SABE_PA_DLL inline int getGameObjectIndex(GameObject* i_object) {
            if (i_object == nullptr) return -1;

            if (i_object == m_player1) return -2;
            if (i_object == m_player2) return -3;
            if (m_objects) {
                int l_index = i_object->m_uniqueID - m_fields->m_uniqueIDBase;
                // A game update or another mod can change the ID sequence.
                // Only use the shortcut when it points to this exact object.
                if (l_index >= 0 && static_cast<unsigned int>(l_index) < m_objects->count()
                    && m_objects->objectAtIndex(l_index) == i_object) {
                    return l_index;
                }
                l_index = m_objects->indexOfObject(i_object);
                if (l_index >= 0) return l_index;
            }
            geode::log::info("@@@@@@@@@@@@@@ UNKNOWN GAMEOBJECT BEING SAVED @@@@@@@@@@@@@@@");
            return -404;
        }

        SABE_PA_DLL GameObject* getGameObject(int i_index) {
            if (i_index > -1) {
                if (!m_objects || static_cast<unsigned int>(i_index) >= m_objects->count()) {
                    geode::log::warn("Saved object index {} is outside the current level", i_index);
                    return nullptr;
                }
                return static_cast<GameObject*>(m_objects->objectAtIndex(i_index));
            }
            if (i_index == -2) {
                return m_player1;
            }
            if (i_index == -3) {
                return m_player2;
            }
            if (i_index == -4) {
                geode::log::info("!!!!!!!!!!!!!! GAMEOBJECTPHYSICS BACKWARD COMPATIBILITY GAMEOBJECT !!!!!!!!!!!!!!");
            }
            if (i_index == -404) {
                geode::log::info("@@@@@@@@@@@@@@ UNKNOWN GAMEOBJECT BEING LOADED @@@@@@@@@@@@@@@");
            }
            return nullptr;
        }
    };
}