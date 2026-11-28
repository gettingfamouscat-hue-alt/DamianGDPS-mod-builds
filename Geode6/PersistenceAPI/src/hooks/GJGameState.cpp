#include "hooks/GJGameState.hpp"
#include "Geode/binding/PlayLayer.hpp"
#include "hooks/cocos2d/CCNode.hpp"
#include "hooks/EventTriggerInstance.hpp"
#include "hooks/PlayLayer.hpp"
#include "hooks/EnterEffectInstance.hpp"
#include "hooks/SFXTriggerState.hpp"
#include "hooks/SFXTriggerInstance.hpp"
#include "hooks/SongTriggerState.hpp"
#include "hooks/SongChannelState.hpp"
#include "hooks/DynamicObjectAction.hpp"
#include "hooks/AdvancedFollowInstance.hpp"
#include "hooks/GameObjectPhysics.hpp"
#include "hooks/GJValueTween.hpp"
#include "util/debug.hpp"

using namespace geode::prelude;
using namespace persistenceAPI;

void PAGJGameState::load(Stream& i_stream) {
    i_stream >> *this;
}

void PAGJGameState::save(Stream& o_stream) {
    o_stream << *this;
}

inline void persistenceAPI::operator>>(Stream& i_stream, PAGJGameState& o_value) {
    i_stream >> o_value.m_cameraZoom;
    SEPARATOR_I
    i_stream >> o_value.m_targetCameraZoom;
    SEPARATOR_I
    i_stream >> o_value.m_cameraOffset;
    SEPARATOR_I
    i_stream >> o_value.m_cameraOffsetTarget;
    SEPARATOR_I
    i_stream >> o_value.m_lastCameraTarget;
    SEPARATOR_I
    i_stream >> o_value.m_lastCameraOffset;
    SEPARATOR_I
    i_stream >> o_value.m_cameraDeltaPosition;
    SEPARATOR_I
    i_stream >> o_value.m_staticCameraStart;
    SEPARATOR_I
    i_stream >> o_value.m_staticCameraPosition;
    SEPARATOR_I
    i_stream >> o_value.m_staticCameraTarget;
    SEPARATOR_I
    i_stream >> o_value.m_staticExitOffset;
    SEPARATOR_I
    i_stream >> o_value.m_appliedStaticExitOffset;
    SEPARATOR_I
    i_stream >> o_value.m_staticStartVelocity;
    SEPARATOR_I
    i_stream >> o_value.m_staticCameraDuration;
    SEPARATOR_I
    i_stream >> o_value.m_staticTransitionProgress;
    SEPARATOR_I
    i_stream >> o_value.m_lastStaticTransitionProgress;
    SEPARATOR_I
    i_stream >> o_value.m_staticFollowEasing;
    SEPARATOR_I
    i_stream >> o_value.m_staticFollow;
    SEPARATOR_I
    i_stream >> o_value.m_staticFollowGroup;
    SEPARATOR_I
    i_stream >> o_value.m_staticCameraActive;
    SEPARATOR_I
    i_stream >> o_value.m_staticExitPending;
    SEPARATOR_I
    i_stream >> o_value.m_staticExitDuration;
    SEPARATOR_I
    i_stream >> o_value.m_staticExitEasingRate;
    SEPARATOR_I
    i_stream >> o_value.m_staticExitEasing;
    SEPARATOR_I
    i_stream >> o_value.m_staticSmoothVelocity;
    SEPARATOR_I
    i_stream >> o_value.m_staticSmoothVelocityMod;
    SEPARATOR_I
    i_stream >> o_value.m_staticExitStartOffset;
    SEPARATOR_I
    i_stream >> o_value.m_unkPoint25;
    SEPARATOR_I
    i_stream >> o_value.m_internalCameraOffset;
    SEPARATOR_I
    i_stream >> o_value.m_internalCameraOffsetTarget;
    SEPARATOR_I
    i_stream >> o_value.m_lastInternalCameraOffsetTarget;
    SEPARATOR_I
    i_stream >> o_value.m_unkPoint29;
    SEPARATOR_I
    i_stream >> o_value.m_dualStaticCamera;
    SEPARATOR_I
    i_stream >> o_value.m_gameplayOffsetX;
    SEPARATOR_I
    i_stream >> o_value.m_gameplayOffsetXDontZoom;
    SEPARATOR_I
    i_stream >> o_value.m_gameplayOffsetY;
    SEPARATOR_I
    i_stream >> o_value.m_gameplayOffsetYDontZoom;
    SEPARATOR_I
    i_stream >> o_value.m_snapInternalCameraOffset;
    SEPARATOR_I
    i_stream >> o_value.m_middleGroundOffsetY;
    SEPARATOR_I
    i_stream >> o_value.m_middleGroundOffsetYTarget;
    SEPARATOR_I
    i_stream >> o_value.m_middleGroundOffsetYDuration;
    SEPARATOR_I
    i_stream >> o_value.m_cameraMoveXActive;
    SEPARATOR_I
    i_stream >> o_value.m_cameraMoveYActive;
    SEPARATOR_I
    i_stream >> o_value.m_cameraPadding;
    SEPARATOR_I
    i_stream >> o_value.m_cameraEasing;
    SEPARATOR_I
    i_stream >> o_value.m_unkInt5;
    SEPARATOR_I
    i_stream >> o_value.m_unkInt6;
    SEPARATOR_I
    i_stream >> o_value.m_unkInt7;
    SEPARATOR_I
    i_stream >> o_value.m_unkInt8;
    SEPARATOR_I
    i_stream >> o_value.m_unkInt9;
    SEPARATOR_I
    i_stream >> o_value.m_flyCameraY;
    SEPARATOR_I
    i_stream >> o_value.m_lastModeHeight;
    SEPARATOR_I
    i_stream >> o_value.m_lastCameraZoom;
    SEPARATOR_I
    i_stream >> o_value.m_zoomDuration;
    SEPARATOR_I
    i_stream >> o_value.m_portalY;
    SEPARATOR_I
    i_stream >> o_value.m_portalYDuringZoom;
    SEPARATOR_I
    i_stream >> o_value.m_gravityRelated;
    SEPARATOR_I
    i_stream >> o_value.m_cameraCenterY;
    SEPARATOR_I
    i_stream >> o_value.m_cameraMoveYTarget;
    SEPARATOR_I
    i_stream >> o_value.m_flyCameraTargetY;
    SEPARATOR_I
    i_stream >> o_value.m_unkInt15;
    SEPARATOR_I
    i_stream >> o_value.m_flyGroundActive;
    SEPARATOR_I
    i_stream >> o_value.m_isFreeMode;
    SEPARATOR_I
    i_stream >> o_value.m_disableCameraGridSnap;
    SEPARATOR_I
    i_stream >> o_value.m_backgroundSpeedX;
    SEPARATOR_I
    i_stream >> o_value.m_backgroundSpeedY;
    SEPARATOR_I
    i_stream >> o_value.m_middlegroundSpeedX;
    SEPARATOR_I
    i_stream >> o_value.m_middlegroundSpeedY;
    SEPARATOR_I
    i_stream >> o_value.m_cameraAngle;
    SEPARATOR_I
    i_stream >> o_value.m_targetCameraAngle;
    SEPARATOR_I
    i_stream >> o_value.m_playerStreakBlend;
    SEPARATOR_I
    i_stream >> o_value.m_timeWarp;
    SEPARATOR_I
    i_stream >> o_value.m_timeWarpRelated;
    SEPARATOR_I
    i_stream >> o_value.m_currentChannel;
    SEPARATOR_I
    i_stream >> o_value.m_rotateChannel;
    SEPARATOR_I
    UMAP_SEPARATOR_I
    i_stream >> o_value.m_spawnChannelRelated0;
    UMAP_SEPARATOR_I
    i_stream >> o_value.m_spawnChannelRelated1;
    UMAP_SEPARATOR_I
    SEPARATOR_I
    i_stream >> o_value.m_totalTime;
    SEPARATOR_I
    i_stream >> o_value.m_levelTime;
    SEPARATOR_I
    i_stream >> o_value.m_bestTime;
    SEPARATOR_I
    i_stream >> o_value.m_commandIndex;
    SEPARATOR_I
    i_stream >> o_value.m_unkUint3;
    SEPARATOR_I
    i_stream >> o_value.m_currentProgress;
    SEPARATOR_I
    i_stream >> o_value.m_stepCommandIndex;
    SEPARATOR_I
    i_stream >> o_value.m_positionHistoryTime;
    SEPARATOR_I
    i_stream >> o_value.m_positionHistoryClock;
    SEPARATOR_I
    i_stream >> o_value.m_areaEffectCounter;
    SEPARATOR_I
    i_stream >> o_value.m_lastActivatedPortal1;
    SEPARATOR_I
    i_stream >> o_value.m_lastActivatedPortal2;
    SEPARATOR_I
    i_stream >> o_value.m_cameraPosition;
    SEPARATOR_I
    i_stream >> o_value.m_isFlipped;
    SEPARATOR_I
    i_stream >> o_value.m_levelFlipProgress;
    SEPARATOR_I
    i_stream >> o_value.m_inSecondHalfOfFlipAnimation;
    SEPARATOR_I
    i_stream >> o_value.m_isFlipping;
    SEPARATOR_I
    i_stream >> o_value.m_isDualMode;
    SEPARATOR_I
    i_stream >> o_value.m_dualAnimationProgress;
    SEPARATOR_I
    UMAP_SEPARATOR_I
    i_stream >> o_value.m_tweenActions;
    UMAP_SEPARATOR_I
    SEPARATOR_I
    i_stream >> o_value.m_cameraEdgeValue0;
    SEPARATOR_I
    i_stream >> o_value.m_cameraEdgeValue1;
    SEPARATOR_I
    i_stream >> o_value.m_cameraEdgeValue2;
    SEPARATOR_I
    i_stream >> o_value.m_cameraEdgeValue3;
    SEPARATOR_I
    UMAP_SEPARATOR_I
    i_stream >> o_value.m_gameObjectPhysics;
    UMAP_SEPARATOR_I
    VEC_SEPARATOR_I
    i_stream >> o_value.m_positionHistory;
    VEC_SEPARATOR_I
    SEPARATOR_I
    i_stream >> o_value.m_unkUint10;
    SEPARATOR_I
    i_stream >> o_value.m_positionHistoryIndex;
    SEPARATOR_I
    i_stream >> o_value.m_cameraHistorySlot;
    SEPARATOR_I
    i_stream >> o_value.m_cameraStepDiff;
    SEPARATOR_I
    i_stream >> o_value.m_backgroundWidth;
    SEPARATOR_I
    i_stream >> o_value.m_timeModRelated;
    SEPARATOR_I
    i_stream >> o_value.m_timeModRelated2;
    SEPARATOR_I
    UMAP_SEPARATOR_I
    i_stream >> o_value.m_activatedObjectIDs;
    UMAP_SEPARATOR_I
    SEPARATOR_I
    i_stream >> o_value.m_teleportCameraEaseTimer;
    SEPARATOR_I
    i_stream >> o_value.m_lastCameraEase;
    SEPARATOR_I
    i_stream >> o_value.m_cameraLastPosition;
    SEPARATOR_I
    i_stream >> o_value.m_player1GoingSideways;
    SEPARATOR_I
    i_stream >> o_value.m_player1GoingLeft;
    SEPARATOR_I
    i_stream >> o_value.m_audioOnDeath;
    SEPARATOR_I
    i_stream >> o_value.m_respawnTime;
    SEPARATOR_I
    i_stream >> o_value.m_noDeathSFX;
    SEPARATOR_I
    i_stream >> o_value.m_cameraShakeEnabled;
    SEPARATOR_I
    i_stream >> o_value.m_cameraShakeDuration;
    SEPARATOR_I
    i_stream >> o_value.m_cameraShakeStrength;
    SEPARATOR_I
    i_stream >> o_value.m_cameraShakeInterval;
    SEPARATOR_I
    i_stream >> o_value.m_lastShakeTime;
    SEPARATOR_I
    i_stream >> o_value.m_cameraShakeOffset;
    SEPARATOR_I
    i_stream >> o_value.m_dualRelated;
    SEPARATOR_I
    UMAP_SEPARATOR_I
    i_stream >> o_value.m_stateObjects;
    UMAP_SEPARATOR_I
    MAP_SEPARATOR_I
    i_stream >> o_value.m_gameEventTriggers;
    MAP_SEPARATOR_I
    i_stream >> o_value.m_gameEventCounts;
    MAP_SEPARATOR_I
    UMAP_SEPARATOR_I
    i_stream >> o_value.m_enterEffectInstanceVectors;
    UMAP_SEPARATOR_I
    i_stream >> o_value.m_exitEffectInstanceVectors;
    UMAP_SEPARATOR_I
    VEC_SEPARATOR_I
    i_stream >> o_value.m_enterChannelMap;
    VEC_SEPARATOR_I
    i_stream >> o_value.m_exitChannelMap;
    VEC_SEPARATOR_I
    i_stream >> o_value.m_moveEffectInstances;
    VEC_SEPARATOR_I
    i_stream >> o_value.m_rotateEffectInstances;
    VEC_SEPARATOR_I
    i_stream >> o_value.m_scaleEffectInstances;
    VEC_SEPARATOR_I
    i_stream >> o_value.m_fadeEffectInstances;
    VEC_SEPARATOR_I
    i_stream >> o_value.m_tintEffectInstances;
    VEC_SEPARATOR_I
    USET_SEPARATOR_I
    i_stream >> o_value.m_unsortedAreaEffects;
    USET_SEPARATOR_I
    SEPARATOR_I
    i_stream >> o_value.m_advancedFollowNeedsSort;
    SEPARATOR_I
    VEC_SEPARATOR_I
    i_stream >> o_value.m_advanceFollowInstances;
    VEC_SEPARATOR_I
    i_stream >> o_value.m_dynamicMoveActions;
    VEC_SEPARATOR_I
    i_stream >> o_value.m_dynamicRotateActions;
    VEC_SEPARATOR_I
    SEPARATOR_I
    i_stream >> o_value.m_instantExitStaticCameraX;
    SEPARATOR_I
    i_stream >> o_value.m_instantExitStaticCameraY;
    SEPARATOR_I
    i_stream >> o_value.m_maxPortalY;
    SEPARATOR_I
    UMAP_SEPARATOR_I
    i_stream >> o_value.m_sfxTriggerChannels;
    UMAP_SEPARATOR_I
    MAP_SEPARATOR_I
    i_stream >> o_value.m_proximityVolumeRelated;
    MAP_SEPARATOR_I
    UMAP_SEPARATOR_I
    i_stream >> o_value.m_songChannelStates;
    UMAP_SEPARATOR_I
    i_stream >> o_value.m_songTriggerStateVectors;
    UMAP_SEPARATOR_I
    VEC_SEPARATOR_I
    i_stream >> o_value.m_sfxTriggerStates;
    VEC_SEPARATOR_I
    SEPARATOR_I
    i_stream >> o_value.m_mainSongStopped;
    SEPARATOR_I
    i_stream >> o_value.m_background;
    SEPARATOR_I
    i_stream >> o_value.m_ground;
    SEPARATOR_I
    i_stream >> o_value.m_middleground;
    SEPARATOR_I
    i_stream >> o_value.m_unlinkDualGravity;
    SEPARATOR_I
    i_stream >> o_value.m_points;
    SEPARATOR_I
    i_stream >> o_value.m_hasObtainedPoints;
    SEPARATOR_I
    i_stream >> o_value.m_pauseCounter;
    SEPARATOR_I
    i_stream >> o_value.m_pauseBufferTimer;
    SEPARATOR_I
}

inline void persistenceAPI::operator<<(Stream& o_stream, PAGJGameState& i_value) {
    o_stream << i_value.m_cameraZoom;
    SEPARATOR_O
    o_stream << i_value.m_targetCameraZoom;
    SEPARATOR_O
    o_stream << i_value.m_cameraOffset;
    SEPARATOR_O
    o_stream << i_value.m_cameraOffsetTarget;
    SEPARATOR_O
    o_stream << i_value.m_lastCameraTarget;
    SEPARATOR_O
    o_stream << i_value.m_lastCameraOffset;
    SEPARATOR_O
    o_stream << i_value.m_cameraDeltaPosition;
    SEPARATOR_O
    o_stream << i_value.m_staticCameraStart;
    SEPARATOR_O
    o_stream << i_value.m_staticCameraPosition;
    SEPARATOR_O
    o_stream << i_value.m_staticCameraTarget;
    SEPARATOR_O
    o_stream << i_value.m_staticExitOffset;
    SEPARATOR_O
    o_stream << i_value.m_appliedStaticExitOffset;
    SEPARATOR_O
    o_stream << i_value.m_staticStartVelocity;
    SEPARATOR_O
    o_stream << i_value.m_staticCameraDuration;
    SEPARATOR_O
    o_stream << i_value.m_staticTransitionProgress;
    SEPARATOR_O
    o_stream << i_value.m_lastStaticTransitionProgress;
    SEPARATOR_O
    o_stream << i_value.m_staticFollowEasing;
    SEPARATOR_O
    o_stream << i_value.m_staticFollow;
    SEPARATOR_O
    o_stream << i_value.m_staticFollowGroup;
    SEPARATOR_O
    o_stream << i_value.m_staticCameraActive;
    SEPARATOR_O
    o_stream << i_value.m_staticExitPending;
    SEPARATOR_O
    o_stream << i_value.m_staticExitDuration;
    SEPARATOR_O
    o_stream << i_value.m_staticExitEasingRate;
    SEPARATOR_O
    o_stream << i_value.m_staticExitEasing;
    SEPARATOR_O
    o_stream << i_value.m_staticSmoothVelocity;
    SEPARATOR_O
    o_stream << i_value.m_staticSmoothVelocityMod;
    SEPARATOR_O
    o_stream << i_value.m_staticExitStartOffset;
    SEPARATOR_O
    o_stream << i_value.m_unkPoint25;
    SEPARATOR_O
    o_stream << i_value.m_internalCameraOffset;
    SEPARATOR_O
    o_stream << i_value.m_internalCameraOffsetTarget;
    SEPARATOR_O
    o_stream << i_value.m_lastInternalCameraOffsetTarget;
    SEPARATOR_O
    o_stream << i_value.m_unkPoint29;
    SEPARATOR_O
    o_stream << i_value.m_dualStaticCamera;
    SEPARATOR_O
    o_stream << i_value.m_gameplayOffsetX;
    SEPARATOR_O
    o_stream << i_value.m_gameplayOffsetXDontZoom;
    SEPARATOR_O
    o_stream << i_value.m_gameplayOffsetY;
    SEPARATOR_O
    o_stream << i_value.m_gameplayOffsetYDontZoom;
    SEPARATOR_O
    o_stream << i_value.m_snapInternalCameraOffset;
    SEPARATOR_O
    o_stream << i_value.m_middleGroundOffsetY;
    SEPARATOR_O
    o_stream << i_value.m_middleGroundOffsetYTarget;
    SEPARATOR_O
    o_stream << i_value.m_middleGroundOffsetYDuration;
    SEPARATOR_O
    o_stream << i_value.m_cameraMoveXActive;
    SEPARATOR_O
    o_stream << i_value.m_cameraMoveYActive;
    SEPARATOR_O
    o_stream << i_value.m_cameraPadding;
    SEPARATOR_O
    o_stream << i_value.m_cameraEasing;
    SEPARATOR_O
    o_stream << i_value.m_unkInt5;
    SEPARATOR_O
    o_stream << i_value.m_unkInt6;
    SEPARATOR_O
    o_stream << i_value.m_unkInt7;
    SEPARATOR_O
    o_stream << i_value.m_unkInt8;
    SEPARATOR_O
    o_stream << i_value.m_unkInt9;
    SEPARATOR_O
    o_stream << i_value.m_flyCameraY;
    SEPARATOR_O
    o_stream << i_value.m_lastModeHeight;
    SEPARATOR_O
    o_stream << i_value.m_lastCameraZoom;
    SEPARATOR_O
    o_stream << i_value.m_zoomDuration;
    SEPARATOR_O
    o_stream << i_value.m_portalY;
    SEPARATOR_O
    o_stream << i_value.m_portalYDuringZoom;
    SEPARATOR_O
    o_stream << i_value.m_gravityRelated;
    SEPARATOR_O
    o_stream << i_value.m_cameraCenterY;
    SEPARATOR_O
    o_stream << i_value.m_cameraMoveYTarget;
    SEPARATOR_O
    o_stream << i_value.m_flyCameraTargetY;
    SEPARATOR_O
    o_stream << i_value.m_unkInt15;
    SEPARATOR_O
    o_stream << i_value.m_flyGroundActive;
    SEPARATOR_O
    o_stream << i_value.m_isFreeMode;
    SEPARATOR_O
    o_stream << i_value.m_disableCameraGridSnap;
    SEPARATOR_O
    o_stream << i_value.m_backgroundSpeedX;
    SEPARATOR_O
    o_stream << i_value.m_backgroundSpeedY;
    SEPARATOR_O
    o_stream << i_value.m_middlegroundSpeedX;
    SEPARATOR_O
    o_stream << i_value.m_middlegroundSpeedY;
    SEPARATOR_O
    o_stream << i_value.m_cameraAngle;
    SEPARATOR_O
    o_stream << i_value.m_targetCameraAngle;
    SEPARATOR_O
    o_stream << i_value.m_playerStreakBlend;
    SEPARATOR_O
    o_stream << i_value.m_timeWarp;
    SEPARATOR_O
    o_stream << i_value.m_timeWarpRelated;
    SEPARATOR_O
    o_stream << i_value.m_currentChannel;
    SEPARATOR_O
    o_stream << i_value.m_rotateChannel;
    SEPARATOR_O
    UMAP_SEPARATOR_O
    o_stream << i_value.m_spawnChannelRelated0;
    UMAP_SEPARATOR_O
    o_stream << i_value.m_spawnChannelRelated1;
    UMAP_SEPARATOR_O
    SEPARATOR_O
    o_stream << i_value.m_totalTime;
    SEPARATOR_O
    o_stream << i_value.m_levelTime;
    SEPARATOR_O
    o_stream << i_value.m_bestTime;
    SEPARATOR_O
    o_stream << i_value.m_commandIndex;
    SEPARATOR_O
    o_stream << i_value.m_unkUint3;
    SEPARATOR_O
    o_stream << i_value.m_currentProgress;
    SEPARATOR_O
    o_stream << i_value.m_stepCommandIndex;
    SEPARATOR_O
    o_stream << i_value.m_positionHistoryTime;
    SEPARATOR_O
    o_stream << i_value.m_positionHistoryClock;
    SEPARATOR_O
    o_stream << i_value.m_areaEffectCounter;
    SEPARATOR_O
    o_stream << i_value.m_lastActivatedPortal1;
    SEPARATOR_O
    o_stream << i_value.m_lastActivatedPortal2;
    SEPARATOR_O
    o_stream << i_value.m_cameraPosition;
    SEPARATOR_O
    o_stream << i_value.m_isFlipped;
    SEPARATOR_O
    o_stream << i_value.m_levelFlipProgress;
    SEPARATOR_O
    o_stream << i_value.m_inSecondHalfOfFlipAnimation;
    SEPARATOR_O
    o_stream << i_value.m_isFlipping;
    SEPARATOR_O
    o_stream << i_value.m_isDualMode;
    SEPARATOR_O
    o_stream << i_value.m_dualAnimationProgress;
    SEPARATOR_O
    UMAP_SEPARATOR_O
    o_stream << i_value.m_tweenActions;
    UMAP_SEPARATOR_O
    SEPARATOR_O
    o_stream << i_value.m_cameraEdgeValue0;
    SEPARATOR_O
    o_stream << i_value.m_cameraEdgeValue1;
    SEPARATOR_O
    o_stream << i_value.m_cameraEdgeValue2;
    SEPARATOR_O
    o_stream << i_value.m_cameraEdgeValue3;
    SEPARATOR_O
    UMAP_SEPARATOR_O
    o_stream << i_value.m_gameObjectPhysics;
    UMAP_SEPARATOR_O
    VEC_SEPARATOR_O
    o_stream << i_value.m_positionHistory;
    VEC_SEPARATOR_O
    SEPARATOR_O
    o_stream << i_value.m_unkUint10;
    SEPARATOR_O
    o_stream << i_value.m_positionHistoryIndex;
    SEPARATOR_O
    o_stream << i_value.m_cameraHistorySlot;
    SEPARATOR_O
    o_stream << i_value.m_cameraStepDiff;
    SEPARATOR_O
    o_stream << i_value.m_backgroundWidth;
    SEPARATOR_O
    o_stream << i_value.m_timeModRelated;
    SEPARATOR_O
    o_stream << i_value.m_timeModRelated2;
    SEPARATOR_O
    UMAP_SEPARATOR_O
    o_stream << i_value.m_activatedObjectIDs;
    UMAP_SEPARATOR_O
    SEPARATOR_O
    o_stream << i_value.m_teleportCameraEaseTimer;
    SEPARATOR_O
    o_stream << i_value.m_lastCameraEase;
    SEPARATOR_O
    o_stream << i_value.m_cameraLastPosition;
    SEPARATOR_O
    o_stream << i_value.m_player1GoingSideways;
    SEPARATOR_O
    o_stream << i_value.m_player1GoingLeft;
    SEPARATOR_O
    o_stream << i_value.m_audioOnDeath;
    SEPARATOR_O
    o_stream << i_value.m_respawnTime;
    SEPARATOR_O
    o_stream << i_value.m_noDeathSFX;
    SEPARATOR_O
    o_stream << i_value.m_cameraShakeEnabled;
    SEPARATOR_O
    o_stream << i_value.m_cameraShakeDuration;
    SEPARATOR_O
    o_stream << i_value.m_cameraShakeStrength;
    SEPARATOR_O
    o_stream << i_value.m_cameraShakeInterval;
    SEPARATOR_O
    o_stream << i_value.m_lastShakeTime;
    SEPARATOR_O
    o_stream << i_value.m_cameraShakeOffset;
    SEPARATOR_O
    o_stream << i_value.m_dualRelated;
    SEPARATOR_O
    UMAP_SEPARATOR_O
    o_stream << i_value.m_stateObjects;
    UMAP_SEPARATOR_O
    MAP_SEPARATOR_O
    o_stream << i_value.m_gameEventTriggers;
    MAP_SEPARATOR_O
    o_stream << i_value.m_gameEventCounts;
    MAP_SEPARATOR_O
    UMAP_SEPARATOR_O
    o_stream << i_value.m_enterEffectInstanceVectors;
    UMAP_SEPARATOR_O
    o_stream << i_value.m_exitEffectInstanceVectors;
    UMAP_SEPARATOR_O
    VEC_SEPARATOR_O
    o_stream << i_value.m_enterChannelMap;
    VEC_SEPARATOR_O
    o_stream << i_value.m_exitChannelMap;
    VEC_SEPARATOR_O
    o_stream << i_value.m_moveEffectInstances;
    VEC_SEPARATOR_O
    o_stream << i_value.m_rotateEffectInstances;
    VEC_SEPARATOR_O
    o_stream << i_value.m_scaleEffectInstances;
    VEC_SEPARATOR_O
    o_stream << i_value.m_fadeEffectInstances;
    VEC_SEPARATOR_O
    o_stream << i_value.m_tintEffectInstances;
    VEC_SEPARATOR_O
    USET_SEPARATOR_O
    o_stream << i_value.m_unsortedAreaEffects;
    USET_SEPARATOR_O
    SEPARATOR_O
    o_stream << i_value.m_advancedFollowNeedsSort;
    SEPARATOR_O
    VEC_SEPARATOR_O
    o_stream << i_value.m_advanceFollowInstances;
    VEC_SEPARATOR_O
    o_stream << i_value.m_dynamicMoveActions;
    VEC_SEPARATOR_O
    o_stream << i_value.m_dynamicRotateActions;
    VEC_SEPARATOR_O
    SEPARATOR_O
    o_stream << i_value.m_instantExitStaticCameraX;
    SEPARATOR_O
    o_stream << i_value.m_instantExitStaticCameraY;
    SEPARATOR_O
    o_stream << i_value.m_maxPortalY;
    SEPARATOR_O
    UMAP_SEPARATOR_O
    o_stream << i_value.m_sfxTriggerChannels;
    UMAP_SEPARATOR_O
    MAP_SEPARATOR_O
    o_stream << i_value.m_proximityVolumeRelated;
    MAP_SEPARATOR_O
    UMAP_SEPARATOR_O
    o_stream << i_value.m_songChannelStates;
    UMAP_SEPARATOR_O
    o_stream << i_value.m_songTriggerStateVectors;
    UMAP_SEPARATOR_O
    VEC_SEPARATOR_O
    o_stream << i_value.m_sfxTriggerStates;
    VEC_SEPARATOR_O
    SEPARATOR_O
    o_stream << i_value.m_mainSongStopped;
    SEPARATOR_O
    o_stream << i_value.m_background;
    SEPARATOR_O
    o_stream << i_value.m_ground;
    SEPARATOR_O
    o_stream << i_value.m_middleground;
    SEPARATOR_O
    o_stream << i_value.m_unlinkDualGravity;
    SEPARATOR_O
    o_stream << i_value.m_points;
    SEPARATOR_O
    o_stream << i_value.m_hasObtainedPoints;
    SEPARATOR_O
    o_stream << i_value.m_pauseCounter;
    SEPARATOR_O
    o_stream << i_value.m_pauseBufferTimer;
    SEPARATOR_O
}

void PAGJGameState::clean() {
    for (std::pair<int, std::vector<EnterEffectInstance>> l_pair : m_enterEffectInstanceVectors) {
        l_pair.second.clear();
        std::vector<EnterEffectInstance>().swap(l_pair.second);
    }

    for (std::pair<int, std::vector<EnterEffectInstance>> l_pair : m_exitEffectInstanceVectors) {
        l_pair.second.clear();
        std::vector<EnterEffectInstance>().swap(l_pair.second);
    }

    m_moveEffectInstances.clear();
    std::vector<EnterEffectInstance>().swap(m_moveEffectInstances);

    m_rotateEffectInstances.clear();
    std::vector<EnterEffectInstance>().swap(m_rotateEffectInstances);

    m_scaleEffectInstances.clear();
    std::vector<EnterEffectInstance>().swap(m_scaleEffectInstances);

    m_fadeEffectInstances.clear();
    std::vector<EnterEffectInstance>().swap(m_fadeEffectInstances);

    m_tintEffectInstances.clear();
    std::vector<EnterEffectInstance>().swap(m_tintEffectInstances);

    m_advanceFollowInstances.clear();
    std::vector<AdvancedFollowInstance>().swap(m_advanceFollowInstances);
}

#if defined(PA_DEBUG) && defined(PA_DESCRIBE)
void PAGJGameState::describe() {
    log::info("[PAGJGameState - describe] m_cameraZoom: {}", m_cameraZoom);
    log::info("[PAGJGameState - describe] m_targetCameraZoom: {}", m_targetCameraZoom);
    log::info("[PAGJGameState - describe] m_cameraOffset: {}", m_cameraOffset);
    log::info("[PAGJGameState - describe] m_cameraOffsetTarget: {}", m_cameraOffsetTarget);
    log::info("[PAGJGameState - describe] m_lastCameraTarget: {}", m_lastCameraTarget);
    log::info("[PAGJGameState - describe] m_lastCameraOffset: {}", m_lastCameraOffset);
    log::info("[PAGJGameState - describe] m_cameraDeltaPosition: {}", m_cameraDeltaPosition);
    log::info("[PAGJGameState - describe] m_staticCameraStart: {}", m_staticCameraStart);
    log::info("[PAGJGameState - describe] m_staticCameraPosition: {}", m_staticCameraPosition);
    log::info("[PAGJGameState - describe] m_staticCameraTarget: {}", m_staticCameraTarget);
    log::info("[PAGJGameState - describe] m_staticExitOffset: {}", m_staticExitOffset);
    log::info("[PAGJGameState - describe] m_appliedStaticExitOffset: {}", m_appliedStaticExitOffset);
    log::info("[PAGJGameState - describe] m_staticStartVelocity: {}", m_staticStartVelocity);
    log::info("[PAGJGameState - describe] m_staticCameraDuration: {}", m_staticCameraDuration);
    log::info("[PAGJGameState - describe] m_staticTransitionProgress: {}", m_staticTransitionProgress);
    log::info("[PAGJGameState - describe] m_lastStaticTransitionProgress: {}", m_lastStaticTransitionProgress);
    log::info("[PAGJGameState - describe] m_staticFollowEasing: {}", m_staticFollowEasing);
    log::info("[PAGJGameState - describe] m_staticFollow: {}", m_staticFollow);
    log::info("[PAGJGameState - describe] m_staticFollowGroup: {}", m_staticFollowGroup);
    log::info("[PAGJGameState - describe] m_staticCameraActive: {}", m_staticCameraActive);
    log::info("[PAGJGameState - describe] m_staticExitPending: {}", m_staticExitPending);
    log::info("[PAGJGameState - describe] m_staticExitDuration: {}", m_staticExitDuration);
    log::info("[PAGJGameState - describe] m_staticExitEasingRate: {}", m_staticExitEasingRate);
    log::info("[PAGJGameState - describe] m_staticExitEasing: {}", m_staticExitEasing);
    log::info("[PAGJGameState - describe] m_staticSmoothVelocity: {}", m_staticSmoothVelocity);
    log::info("[PAGJGameState - describe] m_staticSmoothVelocityMod: {}", m_staticSmoothVelocityMod);
    log::info("[PAGJGameState - describe] m_staticExitStartOffset: {}", m_staticExitStartOffset);
    log::info("[PAGJGameState - describe] m_unkPoint25: {}", m_unkPoint25);
    log::info("[PAGJGameState - describe] m_internalCameraOffset: {}", m_internalCameraOffset);
    log::info("[PAGJGameState - describe] m_internalCameraOffsetTarget: {}", m_internalCameraOffsetTarget);
    log::info("[PAGJGameState - describe] m_lastInternalCameraOffsetTarget: {}", m_lastInternalCameraOffsetTarget);
    log::info("[PAGJGameState - describe] m_unkPoint29: {}", m_unkPoint29);
    log::info("[PAGJGameState - describe] m_dualStaticCamera: {}", m_dualStaticCamera);
    log::info("[PAGJGameState - describe] m_gameplayOffsetX: {}", m_gameplayOffsetX);
    log::info("[PAGJGameState - describe] m_gameplayOffsetXDontZoom: {}", m_gameplayOffsetXDontZoom);
    log::info("[PAGJGameState - describe] m_gameplayOffsetY: {}", m_gameplayOffsetY);
    log::info("[PAGJGameState - describe] m_gameplayOffsetYDontZoom: {}", m_gameplayOffsetYDontZoom);
    log::info("[PAGJGameState - describe] m_snapInternalCameraOffset: {}", m_snapInternalCameraOffset);
    log::info("[PAGJGameState - describe] m_middleGroundOffsetY: {}", m_middleGroundOffsetY);
    log::info("[PAGJGameState - describe] m_middleGroundOffsetYTarget: {}", m_middleGroundOffsetYTarget);
    log::info("[PAGJGameState - describe] m_middleGroundOffsetYDuration: {}", m_middleGroundOffsetYDuration);
    log::info("[PAGJGameState - describe] m_cameraMoveXActive: {}", m_cameraMoveXActive);
    log::info("[PAGJGameState - describe] m_cameraMoveYActive: {}", m_cameraMoveYActive);
    log::info("[PAGJGameState - describe] m_cameraPadding: {}", m_cameraPadding);
    log::info("[PAGJGameState - describe] m_cameraEasing: {}", m_cameraEasing);
    log::info("[PAGJGameState - describe] m_unkInt5: {}", m_unkInt5);
    log::info("[PAGJGameState - describe] m_unkInt6: {}", m_unkInt6);
    log::info("[PAGJGameState - describe] m_unkInt7: {}", m_unkInt7);
    log::info("[PAGJGameState - describe] m_unkInt8: {}", m_unkInt8);
    log::info("[PAGJGameState - describe] m_unkInt9: {}", m_unkInt9);
    log::info("[PAGJGameState - describe] m_flyCameraY: {}", m_flyCameraY);
    log::info("[PAGJGameState - describe] m_lastModeHeight: {}", m_lastModeHeight);
    log::info("[PAGJGameState - describe] m_lastCameraZoom: {}", m_lastCameraZoom);
    log::info("[PAGJGameState - describe] m_zoomDuration: {}", m_zoomDuration);
    log::info("[PAGJGameState - describe] m_portalY: {}", m_portalY);
    log::info("[PAGJGameState - describe] m_portalYDuringZoom: {}", m_portalYDuringZoom);
    log::info("[PAGJGameState - describe] m_gravityRelated: {}", m_gravityRelated);
    log::info("[PAGJGameState - describe] m_cameraCenterY: {}", m_cameraCenterY);
    log::info("[PAGJGameState - describe] m_cameraMoveYTarget: {}", m_cameraMoveYTarget);
    log::info("[PAGJGameState - describe] m_flyCameraTargetY: {}", m_flyCameraTargetY);
    log::info("[PAGJGameState - describe] m_unkInt15: {}", m_unkInt15);
    log::info("[PAGJGameState - describe] m_flyGroundActive: {}", m_flyGroundActive);
    log::info("[PAGJGameState - describe] m_isFreeMode: {}", m_isFreeMode);
    log::info("[PAGJGameState - describe] m_disableCameraGridSnap: {}", m_disableCameraGridSnap);
    log::info("[PAGJGameState - describe] m_backgroundSpeedX: {}", m_backgroundSpeedX);
    log::info("[PAGJGameState - describe] m_backgroundSpeedY: {}", m_backgroundSpeedY);
    log::info("[PAGJGameState - describe] m_middlegroundSpeedX: {}", m_middlegroundSpeedX);
    log::info("[PAGJGameState - describe] m_middlegroundSpeedY: {}", m_middlegroundSpeedY);
    log::info("[PAGJGameState - describe] m_cameraAngle: {}", m_cameraAngle);
    log::info("[PAGJGameState - describe] m_targetCameraAngle: {}", m_targetCameraAngle);
    log::info("[PAGJGameState - describe] m_playerStreakBlend: {}", m_playerStreakBlend);
    log::info("[PAGJGameState - describe] m_timeWarp: {}", m_timeWarp);
    log::info("[PAGJGameState - describe] m_timeWarpRelated: {}", m_timeWarpRelated);
    log::info("[PAGJGameState - describe] m_currentChannel: {}", m_currentChannel);
    log::info("[PAGJGameState - describe] m_rotateChannel: {}", m_rotateChannel);
    int i = 0;
    for (std::pair<int, int> l_pair : m_spawnChannelRelated0) {
        log::info("[PAGJGameState - describe] m_spawnChannelRelated0 element {} key: {}", i, l_pair.first);
        log::info("[PAGJGameState - describe] m_spawnChannelRelated0 element {} value: {}", i, l_pair.second);
        i++;
    }
    i = 0;
    for (std::pair<int, bool> l_pair : m_spawnChannelRelated1) {
        log::info("[PAGJGameState - describe] m_spawnChannelRelated1 element {} key: {}", i, l_pair.first);
        log::info("[PAGJGameState - describe] m_spawnChannelRelated1 element {} value: {}", i, l_pair.second);
        i++;
    }
    log::info("[PAGJGameState - describe] m_totalTime: {}", m_totalTime);
    log::info("[PAGJGameState - describe] m_levelTime: {}", m_levelTime);
    log::info("[PAGJGameState - describe] m_bestTime: {}", m_bestTime);
    log::info("[PAGJGameState - describe] m_commandIndex: {}", m_commandIndex);
    log::info("[PAGJGameState - describe] m_unkUint3: {}", m_unkUint3);
    log::info("[PAGJGameState - describe] m_currentProgress: {}", m_currentProgress);
    log::info("[PAGJGameState - describe] m_stepCommandIndex: {}", m_stepCommandIndex);
    log::info("[PAGJGameState - describe] m_positionHistoryTime: {}", m_positionHistoryTime);
    log::info("[PAGJGameState - describe] m_positionHistoryClock: {}", m_positionHistoryClock);
    log::info("[PAGJGameState - describe] m_areaEffectCounter: {}", m_areaEffectCounter);
    int l_objectIndex = -1;
    PAPlayLayer* l_playLayer = static_cast<PAPlayLayer*>(PlayLayer::get());
    if (l_playLayer) l_objectIndex = l_playLayer->getGameObjectIndex(m_lastActivatedPortal1);
    log::info("[PAGJGameState - describe] m_lastActivatedPortal1 objectIndex: {}", l_objectIndex);
    l_objectIndex = -1;
    if (l_playLayer) l_objectIndex = l_playLayer->getGameObjectIndex(m_lastActivatedPortal2);
    log::info("[PAGJGameState - describe] m_lastActivatedPortal2 objectIndex: {}", l_objectIndex);
    log::info("[PAGJGameState - describe] m_cameraPosition: {}", m_cameraPosition);
    log::info("[PAGJGameState - describe] m_isFlipped: {}", m_isFlipped);
    log::info("[PAGJGameState - describe] m_levelFlipProgress: {}", m_levelFlipProgress);
    log::info("[PAGJGameState - describe] m_inSecondHalfOfFlipAnimation: {}", m_inSecondHalfOfFlipAnimation);
    log::info("[PAGJGameState - describe] m_isFlipping: {}", m_isFlipping);
    log::info("[PAGJGameState - describe] m_isDualMode: {}", m_isDualMode);
    log::info("[PAGJGameState - describe] m_dualAnimationProgress: {}", m_dualAnimationProgress);
    int l_size = m_tweenActions.size();
    log::info("[PAGJGameState - describe] m_tweenActions.size(): {}", l_size);
    i = 0;
    for (std::pair<int,GJValueTween> l_pair : m_tweenActions) {
        log::info("[PAGJGameState - describe] m_tweenActions element {} key: {}", i, l_pair.first);
        log::info("[PAGJGameState - describe] m_tweenActions element {} value:", i);
        reinterpret_cast<PAGJValueTween*>(&l_pair.second)->describe();
        i++;
    }
    log::info("[PAGJGameState - describe] m_cameraEdgeValue0: {}", m_cameraEdgeValue0);
    log::info("[PAGJGameState - describe] m_cameraEdgeValue1: {}", m_cameraEdgeValue1);
    log::info("[PAGJGameState - describe] m_cameraEdgeValue2: {}", m_cameraEdgeValue2);
    log::info("[PAGJGameState - describe] m_cameraEdgeValue3: {}", m_cameraEdgeValue3);
    l_size = m_gameObjectPhysics.size();
    log::info("[PAGJGameState - describe] m_gameObjectPhysics.size(): {}", l_size);
    i = 0;
    for (std::pair<int,GameObjectPhysics> l_pair : m_gameObjectPhysics) {
        log::info("[PAGJGameState - describe] m_gameObjectPhysics element {} key: {}", i, l_pair.first);
        log::info("[PAGJGameState - describe] m_gameObjectPhysics element {} value:", i);
        reinterpret_cast<PAGameObjectPhysics*>(&l_pair.second)->describe();
        i++;
    }
    l_size = m_positionHistory.size();
    log::info("[PAGJGameState - describe] m_positionHistory.size(): {}", l_size);
    for (int i = 0; i < l_size; i++) {
        log::info("[PAGJGameState - describe] m_positionHistory[{}]: {}", i, m_positionHistory[i]);
    }
    log::info("[PAGJGameState - describe] m_unkUint10: {}", m_unkUint10);
    log::info("[PAGJGameState - describe] m_positionHistoryIndex: {}", m_positionHistoryIndex);
    log::info("[PAGJGameState - describe] m_cameraHistorySlot: {}", m_cameraHistorySlot);
    log::info("[PAGJGameState - describe] m_cameraStepDiff: {}", m_cameraStepDiff);
    log::info("[PAGJGameState - describe] m_backgroundWidth: {}", m_backgroundWidth);
    log::info("[PAGJGameState - describe] m_timeModRelated: {}", m_timeModRelated);
    log::info("[PAGJGameState - describe] m_timeModRelated2: {}", m_timeModRelated2);
    i = 0;
    for (std::pair<std::pair<int, int>, int> l_pair : m_activatedObjectIDs) {
        log::info("[PAGJGameState - describe] m_activatedObjectIDs element {} key: {}", i, l_pair.first);
        log::info("[PAGJGameState - describe] m_activatedObjectIDs element {} value: {}", i, l_pair.second);
        i++;
    }
    log::info("[PAGJGameState - describe] m_teleportCameraEaseTimer: {}", m_teleportCameraEaseTimer);
    log::info("[PAGJGameState - describe] m_lastCameraEase: {}", m_lastCameraEase);
    log::info("[PAGJGameState - describe] m_cameraLastPosition: {}", m_cameraLastPosition);
    log::info("[PAGJGameState - describe] m_player1GoingSideways: {}", m_player1GoingSideways);
    log::info("[PAGJGameState - describe] m_player1GoingLeft: {}", m_player1GoingLeft);
    log::info("[PAGJGameState - describe] m_audioOnDeath: {}", m_audioOnDeath);
    log::info("[PAGJGameState - describe] m_respawnTime: {}", m_respawnTime);
    log::info("[PAGJGameState - describe] m_noDeathSFX: {}", m_noDeathSFX);
    log::info("[PAGJGameState - describe] m_cameraShakeEnabled: {}", m_cameraShakeEnabled);
    log::info("[PAGJGameState - describe] m_cameraShakeDuration: {}", m_cameraShakeDuration);
    log::info("[PAGJGameState - describe] m_cameraShakeStrength: {}", m_cameraShakeStrength);
    log::info("[PAGJGameState - describe] m_cameraShakeInterval: {}", m_cameraShakeInterval);
    log::info("[PAGJGameState - describe] m_lastShakeTime: {}", m_lastShakeTime);
    log::info("[PAGJGameState - describe] m_cameraShakeOffset: {}", m_cameraShakeOffset);
    log::info("[PAGJGameState - describe] m_dualRelated: {}", m_dualRelated);
    i = 0;
    for (std::pair<int, EnhancedGameObject*> l_pair : m_stateObjects) {
        log::info("[PAGJGameState - describe] m_stateObjects element {} key: {}", i, l_pair.first);
        int l_objectIndex = -1;
        PAPlayLayer* l_playLayer = static_cast<PAPlayLayer*>(PlayLayer::get());
        if (l_playLayer) l_objectIndex = l_playLayer->getGameObjectIndex(l_pair.second);
        log::info("[PAGJGameState - describe] m_stateObjects element {} value (l_objectIndex): {}", i, l_objectIndex);
        i++;
    }
    i = 0;
    for (std::pair<std::pair<GJGameEvent,int>, std::vector<EventTriggerInstance>> l_pair : m_gameEventTriggers) {
        log::info("[PAGJGameState - describe] m_gameEventTriggers element {} key.key: {}", i, *reinterpret_cast<int*>(&l_pair.first.first));
        log::info("[PAGJGameState - describe] m_gameEventTriggers element {} key.value: {}", i, l_pair.first.second);
        log::info("[PAGJGameState - describe] m_gameEventTriggers element {} value:", i);
        l_size = l_pair.second.size();
        log::info("[PAGJGameState - describe] m_gameEventTriggers element {} value.size(): {}", i, l_size);
        for (int j = 0; j < l_size; j++) {
            log::info("[PAGJGameState - describe] m_gameEventTriggers element {} value[{}]:", i, j);
            reinterpret_cast<PAEventTriggerInstance*>(&l_pair.second[j])->describe();
        }
        i++;
    }
    i = 0;
    for (std::pair<std::pair<GJGameEvent,int>, int> l_pair : m_gameEventCounts) {
        log::info("[PAGJGameState - describe] m_gameEventCounts element {} key.key: {}", i, *reinterpret_cast<int*>(&l_pair.first.first));
        log::info("[PAGJGameState - describe] m_gameEventCounts element {} key.value: {}", i, l_pair.first.second);
        log::info("[PAGJGameState - describe] m_gameEventCounts element {} value: {}", i, l_pair.second);
        i++;
    }
    i = 0;
    for (std::pair<int, std::vector<EnterEffectInstance>> l_pair : m_enterEffectInstanceVectors) {
        log::info("[PAGJGameState - describe] m_enterEffectInstanceVectors element {} key.key: {}", i, l_pair.first);
        log::info("[PAGJGameState - describe] m_enterEffectInstanceVectors element {} value:", i);
        l_size = l_pair.second.size();
        log::info("[PAGJGameState - describe] m_enterEffectInstanceVectors element {} value.size(): {}", i, l_size);
        for (int j = 0; j < l_size; j++) {
            log::info("[PAGJGameState - describe] m_enterEffectInstanceVectors element {} value[{}]:", i, j);
            reinterpret_cast<PAEnterEffectInstance*>(&l_pair.second[j])->describe();
        }
        i++;
    }
    i = 0;
    for (std::pair<int, std::vector<EnterEffectInstance>> l_pair : m_exitEffectInstanceVectors) {
        log::info("[PAGJGameState - describe] m_exitEffectInstanceVectors element {} key.key: {}", i, l_pair.first);
        log::info("[PAGJGameState - describe] m_exitEffectInstanceVectors element {} value:", i);
        l_size = l_pair.second.size();
        log::info("[PAGJGameState - describe] m_exitEffectInstanceVectors element {} value.size(): {}", i, l_size);
        for (int j = 0; j < l_size; j++) {
            log::info("[PAGJGameState - describe] m_exitEffectInstanceVectors element {} value[{}]:", i, j);
            reinterpret_cast<PAEnterEffectInstance*>(&l_pair.second[j])->describe();
        }
        i++;
    }
    l_size = m_enterChannelMap.size();
    log::info("[PAGJGameState - describe] m_enterChannelMap.size(): {}", l_size);
    for (int i = 0; i < l_size; i++) {
        log::info("[PAGJGameState - describe] m_enterChannelMap[{}]: {}", i, m_enterChannelMap[i]);
    }
    l_size = m_exitChannelMap.size();
    log::info("[PAGJGameState - describe] m_exitChannelMap.size(): {}", l_size);
    for (int i = 0; i < l_size; i++) {
        log::info("[PAGJGameState - describe] m_exitChannelMap[{}]: {}", i, m_exitChannelMap[i]);
    }
    l_size = m_moveEffectInstances.size();
    log::info("[PAGJGameState - describe] m_moveEffectInstances.size(): {}", l_size);
    for (int i = 0; i < l_size; i++) {
        log::info("[PAGJGameState - describe] m_moveEffectInstances[{}]:", i);
        reinterpret_cast<PAEnterEffectInstance*>(&m_moveEffectInstances[i])->describe();
    }
    l_size = m_rotateEffectInstances.size();
    log::info("[PAGJGameState - describe] m_rotateEffectInstances.size(): {}", l_size);
    for (int i = 0; i < l_size; i++) {
        log::info("[PAGJGameState - describe] m_rotateEffectInstances[{}]:", i);
        reinterpret_cast<PAEnterEffectInstance*>(&m_rotateEffectInstances[i])->describe();
    }
    l_size = m_scaleEffectInstances.size();
    log::info("[PAGJGameState - describe] m_scaleEffectInstances.size(): {}", l_size);
    for (int i = 0; i < l_size; i++) {
        log::info("[PAGJGameState - describe] m_scaleEffectInstances[{}]:", i);
        reinterpret_cast<PAEnterEffectInstance*>(&m_scaleEffectInstances[i])->describe();
    }
    l_size = m_fadeEffectInstances.size();
    log::info("[PAGJGameState - describe] m_fadeEffectInstances.size(): {}", l_size);
    for (int i = 0; i < l_size; i++) {
        log::info("[PAGJGameState - describe] m_fadeEffectInstances[{}]:", i);
        reinterpret_cast<PAEnterEffectInstance*>(&m_fadeEffectInstances[i])->describe();
    }
    l_size = m_tintEffectInstances.size();
    log::info("[PAGJGameState - describe] m_tintEffectInstances.size(): {}", l_size);
    for (int i = 0; i < l_size; i++) {
        log::info("[PAGJGameState - describe] m_tintEffectInstances[{}]:", i);
        reinterpret_cast<PAEnterEffectInstance*>(&m_tintEffectInstances[i])->describe();
    }
    i = 0;
    for (int l_value : m_unsortedAreaEffects) {
        log::info("[PAGJGameState - describe] m_unsortedAreaEffects element {} value: {}", i, l_value);
        i++;
    }
    log::info("[PAGJGameState - describe] m_advancedFollowNeedsSort: {}", m_advancedFollowNeedsSort);
    l_size = m_advanceFollowInstances.size();
    log::info("[PAGJGameState - describe] m_advanceFollowInstances.size(): {}", l_size);
    for (int i = 0; i < l_size; i++) {
        log::info("[PAGJGameState - describe] m_advanceFollowInstances element[{}]:", i);
        reinterpret_cast<PAAdvancedFollowInstance*>(&m_advanceFollowInstances[i])->describe();
    }
    l_size = m_dynamicMoveActions.size();
    log::info("[PAGJGameState - describe] m_dynamicMoveActions.size(): {}", l_size);
    for (int i = 0; i < l_size; i++) {
        log::info("[PAGJGameState - describe] m_dynamicMoveActions[{}]:", i);
        reinterpret_cast<PADynamicObjectAction*>(&m_dynamicMoveActions[i])->describe();
    }
    l_size = m_dynamicRotateActions.size();
    log::info("[PAGJGameState - describe] m_dynamicRotateActions.size(): {}", l_size);
    for (int i = 0; i < l_size; i++) {
        log::info("[PAGJGameState - describe] m_dynamicRotateActions[{}]:", i);
        reinterpret_cast<PADynamicObjectAction*>(&m_dynamicRotateActions[i])->describe();
    }
    log::info("[PAGJGameState - describe] m_instantExitStaticCameraX: {}", m_instantExitStaticCameraX);
    log::info("[PAGJGameState - describe] m_instantExitStaticCameraY: {}", m_instantExitStaticCameraY);
    log::info("[PAGJGameState - describe] m_maxPortalY: {}", m_maxPortalY);
    i = 0;
    for (std::pair<int, std::vector<int>> l_pair : m_sfxTriggerChannels) {
        log::info("[PAGJGameState - describe] m_sfxTriggerChannels element {} key.key: {}", i, l_pair.first);
        log::info("[PAGJGameState - describe] m_sfxTriggerChannels element {} value:", i);
        l_size = l_pair.second.size();
        log::info("[PAGJGameState - describe] m_sfxTriggerChannels element {} value.size(): {}", i, l_size);
        for (int j = 0; j < l_size; j++) {
            log::info("[PAGJGameState - describe] m_sfxTriggerChannels element {} value[{}]: {}", i, j, l_pair.second[j]);
        }
        i++;
    }
    i = 0;
    for (std::pair<std::pair<int, int>, SFXTriggerInstance> l_pair : m_proximityVolumeRelated) {
        log::info("[PAGJGameState - describe] m_proximityVolumeRelated element {} key.key: {}", i, l_pair.first.first);
        log::info("[PAGJGameState - describe] m_proximityVolumeRelated element {} key.value: {}", i, l_pair.first.second);
        log::info("[PAGJGameState - describe] m_proximityVolumeRelated element {} value:", i);
        reinterpret_cast<PASFXTriggerInstance*>(&l_pair.second)->describe();
        i++;
    }
    i = 0;
    for (std::pair<int, SongChannelState> l_pair : m_songChannelStates) {
        log::info("[PAGJGameState - describe] m_songChannelStates element {} key: {}", i, l_pair.first);
        log::info("[PAGJGameState - describe] m_songChannelStates element {} value:", i);
        reinterpret_cast<PASongChannelState*>(&l_pair.second)->describe();
        i++;
    }
    i = 0;
    for (std::pair<int, std::vector<SongTriggerState>> l_pair : m_songTriggerStateVectors) {
        log::info("[PAGJGameState - describe] m_songTriggerStateVectors element {} key.key: {}", i, l_pair.first);
        log::info("[PAGJGameState - describe] m_songTriggerStateVectors element {} value:", i);
        l_size = l_pair.second.size();
        log::info("[PAGJGameState - describe] m_songTriggerStateVectors element {} value.size(): {}", i, l_size);
        for (int j = 0; j < l_size; j++) {
            log::info("[PAGJGameState - describe] m_songTriggerStateVectors element {} value[{}]:", i, j);
            reinterpret_cast<PASongTriggerState*>(&l_pair.second[j])->describe();
        }
        i++;
    }
    l_size = m_sfxTriggerStates.size();
    log::info("[PAGJGameState - describe] m_sfxTriggerStates.size(): {}", l_size);
    for (int i = 0; i < l_size; i++) {
        log::info("[PAGJGameState - describe] m_sfxTriggerStates element {} value:", i);
        reinterpret_cast<PASFXTriggerState*>(&m_sfxTriggerStates[i])->describe();
    }
    log::info("[PAGJGameState - describe] m_mainSongStopped: {}", m_mainSongStopped);
    log::info("[PAGJGameState - describe] m_background: {}", m_background);
    log::info("[PAGJGameState - describe] m_ground: {}", m_ground);
    log::info("[PAGJGameState - describe] m_middleground: {}", m_middleground);
    log::info("[PAGJGameState - describe] m_unlinkDualGravity: {}", m_unlinkDualGravity);
    log::info("[PAGJGameState - describe] m_points: {}", m_points);
    log::info("[PAGJGameState - describe] m_hasObtainedPoints: {}", m_hasObtainedPoints);
    log::info("[PAGJGameState - describe] m_pauseCounter: {}", m_pauseCounter);
    log::info("[PAGJGameState - describe] m_pauseBufferTimer: {}", m_pauseBufferTimer);
}
#endif