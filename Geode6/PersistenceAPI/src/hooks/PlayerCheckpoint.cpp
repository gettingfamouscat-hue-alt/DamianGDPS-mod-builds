#include "hooks/PlayerCheckpoint.hpp"
#include "hooks/PlayLayer.hpp"
#include "hooks/cocos2d/CCArray.hpp"
#include "hooks/cocos2d/CCNode.hpp"
#include "util/debug.hpp"

using namespace geode::prelude;
using namespace persistenceAPI;

void PAPlayerCheckpoint::load(Stream& i_stream) {
    reinterpret_cast<PACCNode*>(this)->load(i_stream);
    i_stream >> *this;
}

void PAPlayerCheckpoint::save(Stream& o_stream) {
    reinterpret_cast<PACCNode*>(this)->save(o_stream);
    o_stream << *this;
}

inline void persistenceAPI::operator>>(Stream& i_stream, PAPlayerCheckpoint& o_value) {
    i_stream >> o_value.m_position;
    SEPARATOR_I
    i_stream >> o_value.m_lastPosition;
    SEPARATOR_I
    i_stream >> o_value.m_yVelocityUnrounded;
    SEPARATOR_I
    i_stream >> o_value.m_isUpsideDown;
    SEPARATOR_I
    i_stream >> o_value.m_isSideways;
    SEPARATOR_I
    i_stream >> o_value.m_isShip;
    SEPARATOR_I
    i_stream >> o_value.m_isBall;
    SEPARATOR_I
    i_stream >> o_value.m_isBird;
    SEPARATOR_I
    i_stream >> o_value.m_isSwing;
    SEPARATOR_I
    i_stream >> o_value.m_isDart;
    SEPARATOR_I
    i_stream >> o_value.m_isRobot;
    SEPARATOR_I
    i_stream >> o_value.m_isSpider;
    SEPARATOR_I
    i_stream >> o_value.m_isOnGround;
    SEPARATOR_I
    i_stream >> o_value.m_ghostType;
    SEPARATOR_I
    if (i_stream.getPAVersion() > 1) {
        i_stream >> o_value.m_isMini;
    } else {
        i_stream.read(reinterpret_cast<char*>(&o_value.m_isMini), 4);
    }
    SEPARATOR_I
    i_stream >> o_value.m_playerSpeed;
    SEPARATOR_I
    i_stream >> o_value.m_isHidden;
    SEPARATOR_I
    i_stream >> o_value.m_isGoingLeft;
    SEPARATOR_I
    if (i_stream.getPAVersion() > 1) {
        i_stream >> o_value.m_maybeReverseSpeed;
        SEPARATOR_I
        i_stream >> o_value.m_jumpBuffered;
        SEPARATOR_I
        i_stream >> o_value.m_isDashing;
        SEPARATOR_I
        i_stream >> o_value.m_dashStartTimeold;
        SEPARATOR_I
        i_stream >> o_value.m_dashX;
        SEPARATOR_I
        i_stream >> o_value.m_dashY;
        SEPARATOR_I
        i_stream >> o_value.m_dashAngle;
        SEPARATOR_I
        i_stream >> o_value.m_dashStartTime;
        SEPARATOR_I
        i_stream >> o_value.m_dashRing;
    } else {
        i_stream.read(reinterpret_cast<char*>(&o_value) + offsetof(PAPlayerCheckpoint,m_isGoingLeft) + sizeof(bool), 34);
    }
    SEPARATOR_I
    i_stream >> o_value.m_platformerCheckpoint;
    SEPARATOR_I
    if (i_stream.getPAVersion() > 1) {
        i_stream >> o_value.m_lastFlipTime;
    } else {
        i_stream.read(reinterpret_cast<char*>(&o_value) + offsetof(PAPlayerCheckpoint,m_platformerCheckpoint) + sizeof(bool), 7);
        SEPARATOR_I
        i_stream.read(reinterpret_cast<char*>(&o_value) + offsetof(PAPlayerCheckpoint,m_lastFlipTime), sizeof(bool));
        SEPARATOR_I
        i_stream.read(reinterpret_cast<char*>(&o_value) + offsetof(PAPlayerCheckpoint,m_lastFlipTime) + sizeof(float), sizeof(float));
    }
    SEPARATOR_I
    i_stream >> o_value.m_gravityMod;
    SEPARATOR_I
    i_stream >> o_value.m_objectSnappedTo;
    SEPARATOR_I
    i_stream >> o_value.m_snapDistance;
    SEPARATOR_I
    i_stream >> o_value.m_accelerationOrSpeed;
    SEPARATOR_I
    if (i_stream.getPAVersion() > 1) {
        i_stream >> o_value.m_decreaseBoostSlide;
    } else {
        i_stream.read(reinterpret_cast<char*>(&o_value) + offsetof(PAPlayerCheckpoint,m_decreaseBoostSlide), sizeof(int));
    }
    SEPARATOR_I
    i_stream >> o_value.m_followRelated;
    VEC_SEPARATOR_I
    i_stream >> o_value.m_playerFollowFloats;
    VEC_SEPARATOR_I
    i_stream >> o_value.m_followSampleTimer;
    SEPARATOR_I
    i_stream >> o_value.m_isOnSlope;
    SEPARATOR_I
    i_stream >> o_value.m_wasOnSlope;
    SEPARATOR_I
    i_stream >> o_value.m_slopeVelocity;
    SEPARATOR_I
    i_stream >> o_value.m_rotation;
    SEPARATOR_I
    i_stream >> o_value.m_wasTeleported;
    SEPARATOR_I
    i_stream >> o_value.m_fixGravityBug;
    SEPARATOR_I
    i_stream >> o_value.m_reverseSync;
    SEPARATOR_I
    i_stream >> o_value.m_yVelocityBeforeSlope;
    SEPARATOR_I
    i_stream >> o_value.m_slopeStartTime;
    SEPARATOR_I
    i_stream >> o_value.m_justPlacedStreak;
    SEPARATOR_I
    i_stream >> o_value.m_wasCollidingTopOrBottom;
    SEPARATOR_I
    i_stream >> o_value.m_lastCollisionBottom;
    SEPARATOR_I
    i_stream >> o_value.m_lastCollisionTop;
    SEPARATOR_I
    i_stream >> o_value.m_lastCollisionLeft;
    SEPARATOR_I
    i_stream >> o_value.m_lastCollisionRight;
    SEPARATOR_I
    i_stream >> o_value.m_prevCollisionBottom;
    SEPARATOR_I
    i_stream >> o_value.m_prevCollisionTop;
    SEPARATOR_I
    i_stream >> o_value.m_currentSlope2;
    SEPARATOR_I
    i_stream >> o_value.m_preLastGroundObject;
    SEPARATOR_I
    i_stream >> o_value.m_slopeAngle;
    SEPARATOR_I
    i_stream >> o_value.m_slopeSlidingMaybeRotated;
    SEPARATOR_I
    i_stream >> o_value.m_quickCheckpointMode;
    SEPARATOR_I
    i_stream >> o_value.m_collidedObject;
    SEPARATOR_I
    i_stream >> o_value.m_lastGroundObject;
    SEPARATOR_I
    i_stream >> o_value.m_collidingWithLeft;
    SEPARATOR_I
    i_stream >> o_value.m_collidingWithRight;
    SEPARATOR_I
    i_stream >> o_value.m_scaleXRelated2;
    SEPARATOR_I
    i_stream >> o_value.m_groundYVelocity;
    SEPARATOR_I
    i_stream >> o_value.m_yVelocityRelated;
    SEPARATOR_I
    i_stream >> o_value.m_scaleXRelated3;
    SEPARATOR_I
    i_stream >> o_value.m_collidingBetweenSteps;
    SEPARATOR_I
    i_stream >> o_value.m_scaleXRelated4;
    SEPARATOR_I
    i_stream >> o_value.m_scaleXRelated5;
    SEPARATOR_I
    i_stream >> o_value.m_isCollidingWithSlope;
    SEPARATOR_I
    i_stream >> o_value.m_isBallRotating;
    SEPARATOR_I
    i_stream >> o_value.m_noPotentialSlope;
    SEPARATOR_I
    i_stream >> o_value.m_currentPotentialSlope;
    SEPARATOR_I
    i_stream >> o_value.m_currentSlope;
    SEPARATOR_I
    i_stream >> o_value.m_slopeCenterOffset;
    SEPARATOR_I
    i_stream >> o_value.m_collidingWithSlopeId;
    SEPARATOR_I
    i_stream >> o_value.m_slopeIsDownhill;
    SEPARATOR_I
    i_stream >> o_value.m_slopeAngleRadians;
    SEPARATOR_I
    i_stream >> o_value.m_rotationSpeed;
    SEPARATOR_I
    i_stream >> o_value.m_rotateSpeed;
    SEPARATOR_I
    i_stream >> o_value.m_isRotating;
    SEPARATOR_I
    i_stream >> o_value.m_ballSpinning;
    SEPARATOR_I
    i_stream >> o_value.m_speedMultiplier;
    SEPARATOR_I
    i_stream >> o_value.m_yStart;
    SEPARATOR_I
    i_stream >> o_value.m_gravity;
    SEPARATOR_I
    i_stream >> o_value.m_trailingParticleLife;
    SEPARATOR_I
    i_stream >> o_value.m_gameModeChangedTime;
    SEPARATOR_I
    i_stream >> o_value.m_ringJumpedSincePress;
    SEPARATOR_I
    i_stream >> o_value.m_hasStartedFalling;
    SEPARATOR_I
    i_stream >> o_value.m_shouldTryPlacingCheckpoint;
    SEPARATOR_I
    i_stream >> o_value.m_playEffects;
    SEPARATOR_I
    i_stream >> o_value.m_landedSinceFlip;
    SEPARATOR_I
    i_stream >> o_value.m_lastSpiderFlipTime;
    SEPARATOR_I
    i_stream >> o_value.m_unkBool5;
    SEPARATOR_I
    i_stream >> o_value.m_usedRingThisStep;
    SEPARATOR_I
    i_stream >> o_value.m_usedRings;
    USET_SEPARATOR_I
    i_stream >> o_value.m_specialGroundHit;
    SEPARATOR_I
    i_stream >> o_value.m_landParticlesAngle;
    SEPARATOR_I
    i_stream >> o_value.m_landParticlesGravityY;
    SEPARATOR_I
    i_stream >> o_value.m_slopeRotation;
    SEPARATOR_I
    i_stream >> o_value.m_currentSlopeYVelocity;
    SEPARATOR_I
    i_stream >> o_value.m_lastSlopeSnapY;
    SEPARATOR_I
    i_stream >> o_value.m_lastSlopeSnapOffset;
    SEPARATOR_I
    i_stream >> o_value.m_lastSlopeSnapAtFarEdge;
    SEPARATOR_I
    i_stream >> o_value.m_lastSlopeSnapAtNearEdge;
    SEPARATOR_I
    i_stream >> o_value.m_isAccelerating;
    SEPARATOR_I
    i_stream >> o_value.m_isCurrentSlopeTop;
    SEPARATOR_I
    i_stream >> o_value.m_collidedTopMinY;
    SEPARATOR_I
    i_stream >> o_value.m_collidedBottomMaxY;
    SEPARATOR_I
    i_stream >> o_value.m_collidedLeftMaxX;
    SEPARATOR_I
    i_stream >> o_value.m_collidedRightMinX;
    SEPARATOR_I
    i_stream >> o_value.m_wasJumpBuffered;
    SEPARATOR_I
    i_stream >> o_value.m_wasRobotJump;
    SEPARATOR_I
    i_stream >> o_value.m_stateJumpBuffered;
    SEPARATOR_I
    i_stream >> o_value.m_statePreJump;
    SEPARATOR_I
    i_stream >> o_value.m_touchedRing;
    SEPARATOR_I
    i_stream >> o_value.m_touchedCustomRing;
    SEPARATOR_I
    i_stream >> o_value.m_touchedGravityPortal;
    SEPARATOR_I
    i_stream >> o_value.m_brokeBlockSincePress;
    SEPARATOR_I
    i_stream >> o_value.m_touchedPad;
    SEPARATOR_I
    i_stream >> o_value.m_yVelocity;
    SEPARATOR_I
    i_stream >> o_value.m_fallSpeed;
    SEPARATOR_I
    i_stream >> o_value.m_slopeResnapped;
    SEPARATOR_I
    i_stream >> o_value.m_reverseCooldown;
    SEPARATOR_I
    i_stream >> o_value.m_reverseSyncStep;
    SEPARATOR_I
    i_stream >> o_value.m_ballRollSpeed;
    SEPARATOR_I
    i_stream >> o_value.m_shipRotation;
    SEPARATOR_I
    i_stream >> o_value.m_lastPortalPos;
    SEPARATOR_I
    i_stream >> o_value.m_unkUnused3;
    SEPARATOR_I
    i_stream >> o_value.m_touchingGround;
    SEPARATOR_I
    i_stream >> o_value.m_lastLandTime;
    SEPARATOR_I
    i_stream >> o_value.m_decelerationRamp;
    SEPARATOR_I
    i_stream >> o_value.m_isRising;
    SEPARATOR_I
    i_stream >> o_value.m_lastGroundXDeltaTime;
    SEPARATOR_I
    i_stream >> o_value.m_isLocked;
    SEPARATOR_I
    i_stream >> o_value.m_controlsDisabled;
    SEPARATOR_I
    i_stream >> o_value.m_lastGroundedPos;
    SEPARATOR_I
    bool l_hasTouchingRingsArray;
    i_stream >> l_hasTouchingRingsArray;
    SEPARATOR_I
    if(l_hasTouchingRingsArray) {
        o_value.m_touchingRings = CCArray::create();
        CC_SAFE_RETAIN(o_value.m_touchingRings);
        static_cast<PACCArray*>(o_value.m_touchingRings)->load<GameObject>(i_stream);
        ARR_SEPARATOR_I
    }
    i_stream >> o_value.m_touchedRings;
    USET_SEPARATOR_I
    i_stream >> o_value.m_lastActivatedPortal;
    SEPARATOR_I
    i_stream >> o_value.m_totalTime;
    SEPARATOR_I
    i_stream >> o_value.m_diagonalYStep;
    SEPARATOR_I
    i_stream >> o_value.m_defaultMiniIcon;
    SEPARATOR_I
    i_stream >> o_value.m_swapColors;
    SEPARATOR_I
    i_stream >> o_value.m_switchDashFireColor;
    SEPARATOR_I
    i_stream >> o_value.m_stateOnGround;
    SEPARATOR_I
    i_stream >> o_value.m_unusedFloatA80;
    SEPARATOR_I
    i_stream >> o_value.m_stateBoostX;
    SEPARATOR_I
    i_stream >> o_value.m_stateBoostY;
    SEPARATOR_I
    i_stream >> o_value.m_stateNoWallPush;
    SEPARATOR_I
    i_stream >> o_value.m_stateScale;
    SEPARATOR_I
    i_stream >> o_value.m_platformerXVelocity;
    SEPARATOR_I
    i_stream >> o_value.m_holdingRight;
    SEPARATOR_I
    i_stream >> o_value.m_holdingLeft;
    SEPARATOR_I
    i_stream >> o_value.m_leftPressedFirst;
    SEPARATOR_I
    i_stream >> o_value.m_platformXMove;
    SEPARATOR_I
    i_stream >> o_value.m_hasPlatformMomentum;
    SEPARATOR_I
    i_stream >> o_value.m_platformMomentum;
    SEPARATOR_I
    i_stream >> o_value.m_forcedSlopeSlide;
    SEPARATOR_I
    i_stream >> o_value.m_isSliding;
    SEPARATOR_I
    i_stream >> o_value.m_slopeSpeedBonus;
    SEPARATOR_I
    i_stream >> o_value.m_isOnIce;
    SEPARATOR_I
    i_stream >> o_value.m_platformRideTime;
    SEPARATOR_I
    i_stream >> o_value.m_wasTouchingGround;
    SEPARATOR_I
    i_stream >> o_value.m_slopeJumpCount;
    SEPARATOR_I
    i_stream >> o_value.m_slopeJumpResetTime;
    SEPARATOR_I
    i_stream >> o_value.m_changedDirectionsTime;
    SEPARATOR_I
    i_stream >> o_value.m_slopeEndTime;
    SEPARATOR_I
    i_stream >> o_value.m_isMoving;
    SEPARATOR_I
    i_stream >> o_value.m_forcedSlideLeft;
    SEPARATOR_I
    i_stream >> o_value.m_forcedSlideRight;
    SEPARATOR_I
    i_stream >> o_value.m_lastSlideLeft;
    SEPARATOR_I
    i_stream >> o_value.m_slideSlopeAngle;
    SEPARATOR_I
    i_stream >> o_value.m_unkUnused2;
    SEPARATOR_I
    i_stream >> o_value.m_stateNoAutoJump;
    SEPARATOR_I
    i_stream >> o_value.m_stateDartSlide;
    SEPARATOR_I
    i_stream >> o_value.m_stateHitHead;
    SEPARATOR_I
    i_stream >> o_value.m_stateFlipGravity;
    SEPARATOR_I
    i_stream >> o_value.m_stateForce;
    SEPARATOR_I
    i_stream >> o_value.m_stateForceVector;
    SEPARATOR_I
    i_stream >> o_value.m_affectedByForces;
    SEPARATOR_I
    i_stream >> o_value.m_touchedForceIds;
    MAP_SEPARATOR_I
    i_stream >> o_value.m_fallStartY;
    SEPARATOR_I
}

inline void persistenceAPI::operator<<(Stream& o_stream, PAPlayerCheckpoint& i_value) {
    o_stream << i_value.m_position;
    SEPARATOR_O
    o_stream << i_value.m_lastPosition;
    SEPARATOR_O
    o_stream << i_value.m_yVelocityUnrounded;
    SEPARATOR_O
    o_stream << i_value.m_isUpsideDown;
    SEPARATOR_O
    o_stream << i_value.m_isSideways;
    SEPARATOR_O
    o_stream << i_value.m_isShip;
    SEPARATOR_O
    o_stream << i_value.m_isBall;
    SEPARATOR_O
    o_stream << i_value.m_isBird;
    SEPARATOR_O
    o_stream << i_value.m_isSwing;
    SEPARATOR_O
    o_stream << i_value.m_isDart;
    SEPARATOR_O
    o_stream << i_value.m_isRobot;
    SEPARATOR_O
    o_stream << i_value.m_isSpider;
    SEPARATOR_O
    o_stream << i_value.m_isOnGround;
    SEPARATOR_O
    o_stream << i_value.m_ghostType;
    SEPARATOR_O
    o_stream << i_value.m_isMini;
    SEPARATOR_O
    o_stream << i_value.m_playerSpeed;
    SEPARATOR_O
    o_stream << i_value.m_isHidden;
    SEPARATOR_O
    o_stream << i_value.m_isGoingLeft;
    SEPARATOR_O
    o_stream << i_value.m_maybeReverseSpeed;
    SEPARATOR_O
    o_stream << i_value.m_jumpBuffered;
    SEPARATOR_O
    o_stream << i_value.m_isDashing;
    SEPARATOR_O
    o_stream << i_value.m_dashStartTimeold;
    SEPARATOR_O
    o_stream << i_value.m_dashX;
    SEPARATOR_O
    o_stream << i_value.m_dashY;
    SEPARATOR_O
    o_stream << i_value.m_dashAngle;
    SEPARATOR_O
    o_stream << i_value.m_dashStartTime;
    SEPARATOR_O
    o_stream << i_value.m_dashRing;
    SEPARATOR_O
    o_stream << i_value.m_platformerCheckpoint;
    SEPARATOR_O
    o_stream << i_value.m_lastFlipTime;
    SEPARATOR_O
    o_stream << i_value.m_gravityMod;
    SEPARATOR_O
    o_stream << i_value.m_objectSnappedTo;
    SEPARATOR_O
    o_stream << i_value.m_snapDistance;
    SEPARATOR_O
    o_stream << i_value.m_accelerationOrSpeed;
    SEPARATOR_O
    o_stream << i_value.m_decreaseBoostSlide;
    SEPARATOR_O
    o_stream << i_value.m_followRelated;
    VEC_SEPARATOR_O
    o_stream << i_value.m_playerFollowFloats;
    VEC_SEPARATOR_O
    o_stream << i_value.m_followSampleTimer;
    SEPARATOR_O
    o_stream << i_value.m_isOnSlope;
    SEPARATOR_O
    o_stream << i_value.m_wasOnSlope;
    SEPARATOR_O
    o_stream << i_value.m_slopeVelocity;
    SEPARATOR_O
    o_stream << i_value.m_rotation;
    SEPARATOR_O
    o_stream << i_value.m_wasTeleported;
    SEPARATOR_O
    o_stream << i_value.m_fixGravityBug;
    SEPARATOR_O
    o_stream << i_value.m_reverseSync;
    SEPARATOR_O
    o_stream << i_value.m_yVelocityBeforeSlope;
    SEPARATOR_O
    o_stream << i_value.m_slopeStartTime;
    SEPARATOR_O
    o_stream << i_value.m_justPlacedStreak;
    SEPARATOR_O
    o_stream << i_value.m_wasCollidingTopOrBottom;
    SEPARATOR_O
    o_stream << i_value.m_lastCollisionBottom;
    SEPARATOR_O
    o_stream << i_value.m_lastCollisionTop;
    SEPARATOR_O
    o_stream << i_value.m_lastCollisionLeft;
    SEPARATOR_O
    o_stream << i_value.m_lastCollisionRight;
    SEPARATOR_O
    o_stream << i_value.m_prevCollisionBottom;
    SEPARATOR_O
    o_stream << i_value.m_prevCollisionTop;
    SEPARATOR_O
    o_stream << i_value.m_currentSlope2;
    SEPARATOR_O
    o_stream << i_value.m_preLastGroundObject;
    SEPARATOR_O
    o_stream << i_value.m_slopeAngle;
    SEPARATOR_O
    o_stream << i_value.m_slopeSlidingMaybeRotated;
    SEPARATOR_O
    o_stream << i_value.m_quickCheckpointMode;
    SEPARATOR_O
    o_stream << i_value.m_collidedObject;
    SEPARATOR_O
    o_stream << i_value.m_lastGroundObject;
    SEPARATOR_O
    o_stream << i_value.m_collidingWithLeft;
    SEPARATOR_O
    o_stream << i_value.m_collidingWithRight;
    SEPARATOR_O
    o_stream << i_value.m_scaleXRelated2;
    SEPARATOR_O
    o_stream << i_value.m_groundYVelocity;
    SEPARATOR_O
    o_stream << i_value.m_yVelocityRelated;
    SEPARATOR_O
    o_stream << i_value.m_scaleXRelated3;
    SEPARATOR_O
    o_stream << i_value.m_collidingBetweenSteps;
    SEPARATOR_O
    o_stream << i_value.m_scaleXRelated4;
    SEPARATOR_O
    o_stream << i_value.m_scaleXRelated5;
    SEPARATOR_O
    o_stream << i_value.m_isCollidingWithSlope;
    SEPARATOR_O
    o_stream << i_value.m_isBallRotating;
    SEPARATOR_O
    o_stream << i_value.m_noPotentialSlope;
    SEPARATOR_O
    o_stream << i_value.m_currentPotentialSlope;
    SEPARATOR_O
    o_stream << i_value.m_currentSlope;
    SEPARATOR_O
    o_stream << i_value.m_slopeCenterOffset;
    SEPARATOR_O
    o_stream << i_value.m_collidingWithSlopeId;
    SEPARATOR_O
    o_stream << i_value.m_slopeIsDownhill;
    SEPARATOR_O
    o_stream << i_value.m_slopeAngleRadians;
    SEPARATOR_O
    o_stream << i_value.m_rotationSpeed;
    SEPARATOR_O
    o_stream << i_value.m_rotateSpeed;
    SEPARATOR_O
    o_stream << i_value.m_isRotating;
    SEPARATOR_O
    o_stream << i_value.m_ballSpinning;
    SEPARATOR_O
    o_stream << i_value.m_speedMultiplier;
    SEPARATOR_O
    o_stream << i_value.m_yStart;
    SEPARATOR_O
    o_stream << i_value.m_gravity;
    SEPARATOR_O
    o_stream << i_value.m_trailingParticleLife;
    SEPARATOR_O
    o_stream << i_value.m_gameModeChangedTime;
    SEPARATOR_O
    o_stream << i_value.m_ringJumpedSincePress;
    SEPARATOR_O
    o_stream << i_value.m_hasStartedFalling;
    SEPARATOR_O
    o_stream << i_value.m_shouldTryPlacingCheckpoint;
    SEPARATOR_O
    o_stream << i_value.m_playEffects;
    SEPARATOR_O
    o_stream << i_value.m_landedSinceFlip;
    SEPARATOR_O
    o_stream << i_value.m_lastSpiderFlipTime;
    SEPARATOR_O
    o_stream << i_value.m_unkBool5;
    SEPARATOR_O
    o_stream << i_value.m_usedRingThisStep;
    SEPARATOR_O
    o_stream << i_value.m_usedRings;
    USET_SEPARATOR_O
    o_stream << i_value.m_specialGroundHit;
    SEPARATOR_O
    o_stream << i_value.m_landParticlesAngle;
    SEPARATOR_O
    o_stream << i_value.m_landParticlesGravityY;
    SEPARATOR_O
    o_stream << i_value.m_slopeRotation;
    SEPARATOR_O
    o_stream << i_value.m_currentSlopeYVelocity;
    SEPARATOR_O
    o_stream << i_value.m_lastSlopeSnapY;
    SEPARATOR_O
    o_stream << i_value.m_lastSlopeSnapOffset;
    SEPARATOR_O
    o_stream << i_value.m_lastSlopeSnapAtFarEdge;
    SEPARATOR_O
    o_stream << i_value.m_lastSlopeSnapAtNearEdge;
    SEPARATOR_O
    o_stream << i_value.m_isAccelerating;
    SEPARATOR_O
    o_stream << i_value.m_isCurrentSlopeTop;
    SEPARATOR_O
    o_stream << i_value.m_collidedTopMinY;
    SEPARATOR_O
    o_stream << i_value.m_collidedBottomMaxY;
    SEPARATOR_O
    o_stream << i_value.m_collidedLeftMaxX;
    SEPARATOR_O
    o_stream << i_value.m_collidedRightMinX;
    SEPARATOR_O
    o_stream << i_value.m_wasJumpBuffered;
    SEPARATOR_O
    o_stream << i_value.m_wasRobotJump;
    SEPARATOR_O
    o_stream << i_value.m_stateJumpBuffered;
    SEPARATOR_O
    o_stream << i_value.m_statePreJump;
    SEPARATOR_O
    o_stream << i_value.m_touchedRing;
    SEPARATOR_O
    o_stream << i_value.m_touchedCustomRing;
    SEPARATOR_O
    o_stream << i_value.m_touchedGravityPortal;
    SEPARATOR_O
    o_stream << i_value.m_brokeBlockSincePress;
    SEPARATOR_O
    o_stream << i_value.m_touchedPad;
    SEPARATOR_O
    o_stream << i_value.m_yVelocity;
    SEPARATOR_O
    o_stream << i_value.m_fallSpeed;
    SEPARATOR_O
    o_stream << i_value.m_slopeResnapped;
    SEPARATOR_O
    o_stream << i_value.m_reverseCooldown;
    SEPARATOR_O
    o_stream << i_value.m_reverseSyncStep;
    SEPARATOR_O
    o_stream << i_value.m_ballRollSpeed;
    SEPARATOR_O
    o_stream << i_value.m_shipRotation;
    SEPARATOR_O
    o_stream << i_value.m_lastPortalPos;
    SEPARATOR_O
    o_stream << i_value.m_unkUnused3;
    SEPARATOR_O
    o_stream << i_value.m_touchingGround;
    SEPARATOR_O
    o_stream << i_value.m_lastLandTime;
    SEPARATOR_O
    o_stream << i_value.m_decelerationRamp;
    SEPARATOR_O
    o_stream << i_value.m_isRising;
    SEPARATOR_O
    o_stream << i_value.m_lastGroundXDeltaTime;
    SEPARATOR_O
    o_stream << i_value.m_isLocked;
    SEPARATOR_O
    o_stream << i_value.m_controlsDisabled;
    SEPARATOR_O
    o_stream << i_value.m_lastGroundedPos;
    SEPARATOR_O
    bool l_hasTouchingRingsArray = false;
    if (i_value.m_touchingRings) {
        l_hasTouchingRingsArray = true;
    }
    o_stream << l_hasTouchingRingsArray;
    SEPARATOR_O
    if (l_hasTouchingRingsArray) {
        static_cast<PACCArray*>(i_value.m_touchingRings)->save<GameObject>(o_stream);
        ARR_SEPARATOR_O
    }
    o_stream << i_value.m_touchedRings;
    USET_SEPARATOR_O
    o_stream << i_value.m_lastActivatedPortal;
    SEPARATOR_O
    o_stream << i_value.m_totalTime;
    SEPARATOR_O
    o_stream << i_value.m_diagonalYStep;
    SEPARATOR_O
    o_stream << i_value.m_defaultMiniIcon;
    SEPARATOR_O
    o_stream << i_value.m_swapColors;
    SEPARATOR_O
    o_stream << i_value.m_switchDashFireColor;
    SEPARATOR_O
    o_stream << i_value.m_stateOnGround;
    SEPARATOR_O
    o_stream << i_value.m_unusedFloatA80;
    SEPARATOR_O
    o_stream << i_value.m_stateBoostX;
    SEPARATOR_O
    o_stream << i_value.m_stateBoostY;
    SEPARATOR_O
    o_stream << i_value.m_stateNoWallPush;
    SEPARATOR_O
    o_stream << i_value.m_stateScale;
    SEPARATOR_O
    o_stream << i_value.m_platformerXVelocity;
    SEPARATOR_O
    o_stream << i_value.m_holdingRight;
    SEPARATOR_O
    o_stream << i_value.m_holdingLeft;
    SEPARATOR_O
    o_stream << i_value.m_leftPressedFirst;
    SEPARATOR_O
    o_stream << i_value.m_platformXMove;
    SEPARATOR_O
    o_stream << i_value.m_hasPlatformMomentum;
    SEPARATOR_O
    o_stream << i_value.m_platformMomentum;
    SEPARATOR_O
    o_stream << i_value.m_forcedSlopeSlide;
    SEPARATOR_O
    o_stream << i_value.m_isSliding;
    SEPARATOR_O
    o_stream << i_value.m_slopeSpeedBonus;
    SEPARATOR_O
    o_stream << i_value.m_isOnIce;
    SEPARATOR_O
    o_stream << i_value.m_platformRideTime;
    SEPARATOR_O
    o_stream << i_value.m_wasTouchingGround;
    SEPARATOR_O
    o_stream << i_value.m_slopeJumpCount;
    SEPARATOR_O
    o_stream << i_value.m_slopeJumpResetTime;
    SEPARATOR_O
    o_stream << i_value.m_changedDirectionsTime;
    SEPARATOR_O
    o_stream << i_value.m_slopeEndTime;
    SEPARATOR_O
    o_stream << i_value.m_isMoving;
    SEPARATOR_O
    o_stream << i_value.m_forcedSlideLeft;
    SEPARATOR_O
    o_stream << i_value.m_forcedSlideRight;
    SEPARATOR_O
    o_stream << i_value.m_lastSlideLeft;
    SEPARATOR_O
    o_stream << i_value.m_slideSlopeAngle;
    SEPARATOR_O
    o_stream << i_value.m_unkUnused2;
    SEPARATOR_O
    o_stream << i_value.m_stateNoAutoJump;
    SEPARATOR_O
    o_stream << i_value.m_stateDartSlide;
    SEPARATOR_O
    o_stream << i_value.m_stateHitHead;
    SEPARATOR_O
    o_stream << i_value.m_stateFlipGravity;
    SEPARATOR_O
    o_stream << i_value.m_stateForce;
    SEPARATOR_O
    o_stream << i_value.m_stateForceVector;
    SEPARATOR_O
    o_stream << i_value.m_affectedByForces;
    SEPARATOR_O
    o_stream << i_value.m_touchedForceIds;
    MAP_SEPARATOR_O
    o_stream << i_value.m_fallStartY;
    SEPARATOR_O
}

#if defined(PA_DEBUG) && defined(PA_DESCRIBE)
void PAPlayerCheckpoint::describe() {
    int l_objectIndex = -1;
    PAPlayLayer* l_playLayer = static_cast<PAPlayLayer*>(PlayLayer::get());
    log::info("[PAPlayerCheckpoint - describe] m_position: {}", m_position);
    log::info("[PAPlayerCheckpoint - describe] m_lastPosition: {}", m_lastPosition);
    log::info("[PAPlayerCheckpoint - describe] m_yVelocityUnrounded: {}", m_yVelocityUnrounded);
    log::info("[PAPlayerCheckpoint - describe] m_isUpsideDown: {}", m_isUpsideDown);
    log::info("[PAPlayerCheckpoint - describe] m_isSideways: {}", m_isSideways);
    log::info("[PAPlayerCheckpoint - describe] m_isShip: {}", m_isShip);
    log::info("[PAPlayerCheckpoint - describe] m_isBall: {}", m_isBall);
    log::info("[PAPlayerCheckpoint - describe] m_isBird: {}", m_isBird);
    log::info("[PAPlayerCheckpoint - describe] m_isSwing: {}", m_isSwing);
    log::info("[PAPlayerCheckpoint - describe] m_isDart: {}", m_isDart);
    log::info("[PAPlayerCheckpoint - describe] m_isRobot: {}", m_isRobot);
    log::info("[PAPlayerCheckpoint - describe] m_isSpider: {}", m_isSpider);
    log::info("[PAPlayerCheckpoint - describe] m_isOnGround: {}", m_isOnGround);
    log::info("[PAPlayerCheckpoint - describe] m_ghostType: {}", *reinterpret_cast<int*>(&m_ghostType));
    log::info("[PAPlayerCheckpoint - describe] m_isMini: {}", m_isMini);
    log::info("[PAPlayerCheckpoint - describe] m_playerSpeed: {}", m_playerSpeed);
    log::info("[PAPlayerCheckpoint - describe] m_isHidden: {}", m_isHidden);
    log::info("[PAPlayerCheckpoint - describe] m_isGoingLeft: {}", m_isGoingLeft);
    log::info("[PAPlayerCheckpoint - describe] m_maybeReverseSpeed: {}", m_maybeReverseSpeed);
    log::info("[PAPlayerCheckpoint - describe] m_isDashing: {}", m_isDashing);
    log::info("[PAPlayerCheckpoint - describe] m_dashX: {}", m_dashX);
    log::info("[PAPlayerCheckpoint - describe] m_dashY: {}", m_dashY);
    log::info("[PAPlayerCheckpoint - describe] m_dashAngle: {}", m_dashAngle);
    log::info("[PAPlayerCheckpoint - describe] m_dashStartTime: {}", m_dashStartTime);
    if (l_playLayer) l_objectIndex = l_playLayer->getGameObjectIndex(m_dashRing);
    log::info("[PAPlayerCheckpoint - describe] m_dashRing: {}", l_objectIndex);
    log::info("[PAPlayerCheckpoint - describe] m_platformerCheckpoint: {}", m_platformerCheckpoint);
    log::info("[PAPlayerCheckpoint - describe] m_lastFlipTime: {}", m_lastFlipTime);
    log::info("[PAPlayerCheckpoint - describe] m_gravityMod: {}", m_gravityMod);
    log::info("[PAPlayerCheckpoint - describe] m_decreaseBoostSlide: {}", m_decreaseBoostSlide);
    log::info("[PAPlayerCheckpoint - describe] m_followRelated: {}", m_followRelated);
    int l_size = m_playerFollowFloats.size();
    log::info("[PAPlayerCheckpoint - describe] m_playerFollowFloats.size(): {}", l_size);
    for (int i = 0; i < l_size; i++) {
        log::info("[PAPlayerCheckpoint - describe] m_playerFollowFloats[{}]: {}", i, m_playerFollowFloats[i]);
    }
    log::info("[PAPlayerCheckpoint - describe] m_jumpBuffered: {}", m_jumpBuffered);
    log::info("[PAPlayerCheckpoint - describe] m_dashStartTimeold: {}", m_dashStartTimeold);
    if (l_playLayer) l_objectIndex = l_playLayer->getGameObjectIndex(m_objectSnappedTo);
    log::info("[PAPlayerCheckpoint - describe] m_objectSnappedTo: {}", m_objectSnappedTo);
    log::info("[PAPlayerCheckpoint - describe] m_snapDistance: {}", m_snapDistance);
    log::info("[PAPlayerCheckpoint - describe] m_accelerationOrSpeed: {}", m_accelerationOrSpeed);
    log::info("[PAPlayerCheckpoint - describe] m_followSampleTimer: {}", m_followSampleTimer);
    log::info("[PAPlayerCheckpoint - describe] m_isOnSlope: {}", m_isOnSlope);
    log::info("[PAPlayerCheckpoint - describe] m_wasOnSlope: {}", m_wasOnSlope);
    log::info("[PAPlayerCheckpoint - describe] m_slopeVelocity: {}", m_slopeVelocity);
    log::info("[PAPlayerCheckpoint - describe] m_rotation: {}", m_rotation);
    log::info("[PAPlayerCheckpoint - describe] m_wasTeleported: {}", m_wasTeleported);
    log::info("[PAPlayerCheckpoint - describe] m_fixGravityBug: {}", m_fixGravityBug);
    log::info("[PAPlayerCheckpoint - describe] m_reverseSync: {}", m_reverseSync);
    log::info("[PAPlayerCheckpoint - describe] m_yVelocityBeforeSlope: {}", m_yVelocityBeforeSlope);
    log::info("[PAPlayerCheckpoint - describe] m_slopeStartTime: {}", m_slopeStartTime);
    log::info("[PAPlayerCheckpoint - describe] m_justPlacedStreak: {}", m_justPlacedStreak);
    log::info("[PAPlayerCheckpoint - describe] m_lastCollisionBottom: {}", m_lastCollisionBottom);
    log::info("[PAPlayerCheckpoint - describe] m_lastCollisionTop: {}", m_lastCollisionTop);
    log::info("[PAPlayerCheckpoint - describe] m_lastCollisionLeft: {}", m_lastCollisionLeft);
    log::info("[PAPlayerCheckpoint - describe] m_lastCollisionRight: {}", m_lastCollisionRight);
    log::info("[PAPlayerCheckpoint - describe] m_prevCollisionBottom: {}", m_prevCollisionBottom);
    log::info("[PAPlayerCheckpoint - describe] m_prevCollisionTop: {}", m_prevCollisionTop);
    if (l_playLayer) l_objectIndex = l_playLayer->getGameObjectIndex(m_currentSlope2);
    log::info("[PAPlayerCheckpoint - describe] m_currentSlope2: {}", m_currentSlope2);
    if (l_playLayer) l_objectIndex = l_playLayer->getGameObjectIndex(m_preLastGroundObject);
    log::info("[PAPlayerCheckpoint - describe] m_preLastGroundObject: {}", m_preLastGroundObject);
    log::info("[PAPlayerCheckpoint - describe] m_slopeAngle: {}", m_slopeAngle);
    log::info("[PAPlayerCheckpoint - describe] m_slopeSlidingMaybeRotated: {}", m_slopeSlidingMaybeRotated);
    log::info("[PAPlayerCheckpoint - describe] m_quickCheckpointMode: {}", m_quickCheckpointMode);
    if (l_playLayer) l_objectIndex = l_playLayer->getGameObjectIndex(m_collidedObject);
    log::info("[PAPlayerCheckpoint - describe] m_collidedObject: {}", m_collidedObject);
    if (l_playLayer) l_objectIndex = l_playLayer->getGameObjectIndex(m_lastGroundObject);
    log::info("[PAPlayerCheckpoint - describe] m_lastGroundObject: {}", m_lastGroundObject);
    if (l_playLayer) l_objectIndex = l_playLayer->getGameObjectIndex(m_collidingWithLeft);
    log::info("[PAPlayerCheckpoint - describe] m_collidingWithLeft: {}", m_collidingWithLeft);
    if (l_playLayer) l_objectIndex = l_playLayer->getGameObjectIndex(m_collidingWithRight);
    log::info("[PAPlayerCheckpoint - describe] m_collidingWithRight: {}", m_collidingWithRight);
    log::info("[PAPlayerCheckpoint - describe] m_scaleXRelated2: {}", m_scaleXRelated2);
    log::info("[PAPlayerCheckpoint - describe] m_groundYVelocity: {}", m_groundYVelocity);
    log::info("[PAPlayerCheckpoint - describe] m_yVelocityRelated: {}", m_yVelocityRelated);
    log::info("[PAPlayerCheckpoint - describe] m_scaleXRelated3: {}", m_scaleXRelated3);
    log::info("[PAPlayerCheckpoint - describe] m_scaleXRelated4: {}", m_scaleXRelated4);
    log::info("[PAPlayerCheckpoint - describe] m_scaleXRelated5: {}", m_scaleXRelated5);
    log::info("[PAPlayerCheckpoint - describe] m_isCollidingWithSlope: {}", m_isCollidingWithSlope);
    log::info("[PAPlayerCheckpoint - describe] m_isBallRotating: {}", m_isBallRotating);
    log::info("[PAPlayerCheckpoint - describe] m_noPotentialSlope: {}", m_noPotentialSlope);
    if (l_playLayer) l_objectIndex = l_playLayer->getGameObjectIndex(m_currentPotentialSlope);
    log::info("[PAPlayerCheckpoint - describe] m_currentPotentialSlope: {}", m_currentPotentialSlope);
    if (l_playLayer) l_objectIndex = l_playLayer->getGameObjectIndex(m_currentSlope);
    log::info("[PAPlayerCheckpoint - describe] m_currentSlope: {}", m_currentSlope);
    log::info("[PAPlayerCheckpoint - describe] m_slopeCenterOffset: {}", m_slopeCenterOffset);
    log::info("[PAPlayerCheckpoint - describe] m_collidingWithSlopeId: {}", m_collidingWithSlopeId);
    log::info("[PAPlayerCheckpoint - describe] m_slopeIsDownhill: {}", m_slopeIsDownhill);
    log::info("[PAPlayerCheckpoint - describe] m_slopeAngleRadians: {}", m_slopeAngleRadians);
    log::info("[PAPlayerCheckpoint - describe] m_rotationSpeed: {}", m_rotationSpeed);
    log::info("[PAPlayerCheckpoint - describe] m_rotateSpeed: {}", m_rotateSpeed);
    log::info("[PAPlayerCheckpoint - describe] m_isRotating: {}", m_isRotating);
    log::info("[PAPlayerCheckpoint - describe] m_ballSpinning: {}", m_ballSpinning);
    log::info("[PAPlayerCheckpoint - describe] m_speedMultiplier: {}", m_speedMultiplier);
    log::info("[PAPlayerCheckpoint - describe] m_yStart: {}", m_yStart);
    log::info("[PAPlayerCheckpoint - describe] m_gravity: {}", m_gravity);
    log::info("[PAPlayerCheckpoint - describe] m_trailingParticleLife: {}", m_trailingParticleLife);
    log::info("[PAPlayerCheckpoint - describe] m_gameModeChangedTime: {}", m_gameModeChangedTime);
    log::info("[PAPlayerCheckpoint - describe] m_ringJumpedSincePress: {}", m_ringJumpedSincePress);
    log::info("[PAPlayerCheckpoint - describe] m_hasStartedFalling: {}", m_hasStartedFalling);
    log::info("[PAPlayerCheckpoint - describe] m_shouldTryPlacingCheckpoint: {}", m_shouldTryPlacingCheckpoint);
    log::info("[PAPlayerCheckpoint - describe] m_playEffects: {}", m_playEffects);
    log::info("[PAPlayerCheckpoint - describe] m_landedSinceFlip: {}", m_landedSinceFlip);
    log::info("[PAPlayerCheckpoint - describe] m_lastSpiderFlipTime: {}", m_lastSpiderFlipTime);
    log::info("[PAPlayerCheckpoint - describe] m_unkBool5: {}", m_unkBool5);
    log::info("[PAPlayerCheckpoint - describe] m_usedRingThisStep: {}", m_usedRingThisStep);
    int i = 0;
    for (int l_value : m_usedRings) {
        log::info("[PAPlayerCheckpoint - describe] m_usedRings element {} value: {}", i, l_value);
        i++;
    }
    log::info("[PAPlayerCheckpoint - describe] m_specialGroundHit: {}", m_specialGroundHit);
    log::info("[PAPlayerCheckpoint - describe] m_landParticlesAngle: {}", m_landParticlesAngle);
    log::info("[PAPlayerCheckpoint - describe] m_landParticlesGravityY: {}", m_landParticlesGravityY);
    log::info("[PAPlayerCheckpoint - describe] m_slopeRotation: {}", m_slopeRotation);
    log::info("[PAPlayerCheckpoint - describe] m_currentSlopeYVelocity: {}", m_currentSlopeYVelocity);
    log::info("[PAPlayerCheckpoint - describe] m_lastSlopeSnapY: {}", m_lastSlopeSnapY);
    log::info("[PAPlayerCheckpoint - describe] m_lastSlopeSnapY: {}", m_lastSlopeSnapY);
    log::info("[PAPlayerCheckpoint - describe] m_lastSlopeSnapAtFarEdge: {}", m_lastSlopeSnapAtFarEdge);
    log::info("[PAPlayerCheckpoint - describe] m_lastSlopeSnapAtNearEdge: {}", m_lastSlopeSnapAtNearEdge);
    log::info("[PAPlayerCheckpoint - describe] m_isAccelerating: {}", m_isAccelerating);
    log::info("[PAPlayerCheckpoint - describe] m_isCurrentSlopeTop: {}", m_isCurrentSlopeTop);
    log::info("[PAPlayerCheckpoint - describe] m_collidedTopMinY: {}", m_collidedTopMinY);
    log::info("[PAPlayerCheckpoint - describe] m_collidedBottomMaxY: {}", m_collidedBottomMaxY);
    log::info("[PAPlayerCheckpoint - describe] m_collidedLeftMaxX: {}", m_collidedLeftMaxX);
    log::info("[PAPlayerCheckpoint - describe] m_collidedRightMinX: {}", m_collidedRightMinX);
    log::info("[PAPlayerCheckpoint - describe] m_wasJumpBuffered: {}", m_wasJumpBuffered);
    log::info("[PAPlayerCheckpoint - describe] m_wasRobotJump: {}", m_wasRobotJump);
    log::info("[PAPlayerCheckpoint - describe] m_stateJumpBuffered: {}", m_stateJumpBuffered);
    log::info("[PAPlayerCheckpoint - describe] m_statePreJump: {}", m_statePreJump);
    log::info("[PAPlayerCheckpoint - describe] m_touchedRing: {}", m_touchedRing);
    log::info("[PAPlayerCheckpoint - describe] m_touchedCustomRing: {}", m_touchedCustomRing);
    log::info("[PAPlayerCheckpoint - describe] m_touchedGravityPortal: {}", m_touchedGravityPortal);
    log::info("[PAPlayerCheckpoint - describe] m_brokeBlockSincePress: {}", m_brokeBlockSincePress);
    log::info("[PAPlayerCheckpoint - describe] m_touchedPad: {}", m_touchedPad);
    log::info("[PAPlayerCheckpoint - describe] m_yVelocity: {}", m_yVelocity);
    log::info("[PAPlayerCheckpoint - describe] m_fallSpeed: {}", m_fallSpeed);
    log::info("[PAPlayerCheckpoint - describe] m_slopeResnapped: {}", m_slopeResnapped);
    log::info("[PAPlayerCheckpoint - describe] m_reverseCooldown: {}", m_reverseCooldown);
    log::info("[PAPlayerCheckpoint - describe] m_reverseSyncStep: {}", m_reverseSyncStep);
    log::info("[PAPlayerCheckpoint - describe] m_ballRollSpeed: {}", m_ballRollSpeed);
    log::info("[PAPlayerCheckpoint - describe] m_shipRotation: {}", m_shipRotation);
    log::info("[PAPlayerCheckpoint - describe] m_lastPortalPos: {}", m_lastPortalPos);
    log::info("[PAPlayerCheckpoint - describe] m_unkUnused3: {}", m_unkUnused3);
    log::info("[PAPlayerCheckpoint - describe] m_touchingGround: {}", m_touchingGround);
    log::info("[PAPlayerCheckpoint - describe] m_lastLandTime: {}", m_lastLandTime);
    log::info("[PAPlayerCheckpoint - describe] m_decelerationRamp: {}", m_decelerationRamp);
    log::info("[PAPlayerCheckpoint - describe] m_isRising: {}", m_isRising);
    log::info("[PAPlayerCheckpoint - describe] m_lastGroundXDeltaTime: {}", m_lastGroundXDeltaTime);
    log::info("[PAPlayerCheckpoint - describe] m_isLocked: {}", m_isLocked);
    log::info("[PAPlayerCheckpoint - describe] m_controlsDisabled: {}", m_controlsDisabled);
    log::info("[PAPlayerCheckpoint - describe] m_lastGroundedPos: {}", m_lastGroundedPos);
    i = 0;
    for (GameObject* l_value : CCArrayExt<GameObject*>(m_touchingRings)) {
        log::info("[PAPlayerCheckpoint - describe] m_touchingRings element {} value: {}", i, l_value);
        i++;
    }
    i = 0;
    for (int l_value : m_touchedRings) {
        log::info("[PAPlayerCheckpoint - describe] m_touchedRings element {} value: {}", i, l_value);
        i++;
    }
    if (l_playLayer) l_objectIndex = l_playLayer->getGameObjectIndex(m_lastActivatedPortal);
    log::info("[PAPlayerCheckpoint - describe] m_lastActivatedPortal: {}", m_lastActivatedPortal);
    log::info("[PAPlayerCheckpoint - describe] m_totalTime: {}", m_totalTime);
    log::info("[PAPlayerCheckpoint - describe] m_diagonalYStep: {}", m_diagonalYStep);
    log::info("[PAPlayerCheckpoint - describe] m_defaultMiniIcon: {}", m_defaultMiniIcon);
    log::info("[PAPlayerCheckpoint - describe] m_swapColors: {}", m_swapColors);
    log::info("[PAPlayerCheckpoint - describe] m_switchDashFireColor: {}", m_switchDashFireColor);
    log::info("[PAPlayerCheckpoint - describe] m_stateOnGround: {}", m_stateOnGround);
    log::info("[PAPlayerCheckpoint - describe] m_stateUnk: {}", m_stateUnk);
    log::info("[PAPlayerCheckpoint - describe] m_stateNoStickX: {}", m_stateNoStickX);
    log::info("[PAPlayerCheckpoint - describe] m_stateNoStickY: {}", m_stateNoStickY);
    log::info("[PAPlayerCheckpoint - describe] m_stateUnk2: {}", m_stateUnk2);
    log::info("[PAPlayerCheckpoint - describe] m_stateBoostX: {}", m_stateBoostX);
    log::info("[PAPlayerCheckpoint - describe] m_stateBoostY: {}", m_stateBoostY);
    log::info("[PAPlayerCheckpoint - describe] m_stateNoWallPush: {}", m_stateNoWallPush);
    log::info("[PAPlayerCheckpoint - describe] m_stateScale: {}", m_stateScale);
    log::info("[PAPlayerCheckpoint - describe] m_platformerXVelocity: {}", m_platformerXVelocity);
    log::info("[PAPlayerCheckpoint - describe] m_holdingRight: {}", m_holdingRight);
    log::info("[PAPlayerCheckpoint - describe] m_holdingLeft: {}", m_holdingLeft);
    log::info("[PAPlayerCheckpoint - describe] m_leftPressedFirst: {}", m_leftPressedFirst);
    log::info("[PAPlayerCheckpoint - describe] m_platformXMove: {}", m_platformXMove);
    log::info("[PAPlayerCheckpoint - describe] m_hasPlatformMomentum: {}", m_hasPlatformMomentum);
    log::info("[PAPlayerCheckpoint - describe] m_platformMomentum: {}", m_platformMomentum);
    log::info("[PAPlayerCheckpoint - describe] m_hasPlatformMomentum: {}", m_hasPlatformMomentum);
    log::info("[PAPlayerCheckpoint - describe] m_isSliding: {}", m_isSliding);
    log::info("[PAPlayerCheckpoint - describe] m_slopeSpeedBonus: {}", m_slopeSpeedBonus);
    log::info("[PAPlayerCheckpoint - describe] m_isOnIce: {}", m_isOnIce);
    log::info("[PAPlayerCheckpoint - describe] m_platformRideTime: {}", m_platformRideTime);
    log::info("[PAPlayerCheckpoint - describe] m_wasTouchingGround: {}", m_wasTouchingGround);
    log::info("[PAPlayerCheckpoint - describe] m_slopeJumpCount: {}", m_slopeJumpCount);
    log::info("[PAPlayerCheckpoint - describe] m_slopeJumpResetTime: {}", m_slopeJumpResetTime);
    log::info("[PAPlayerCheckpoint - describe] m_changedDirectionsTime: {}", m_changedDirectionsTime);
    log::info("[PAPlayerCheckpoint - describe] m_slopeEndTime: {}", m_slopeEndTime);
    log::info("[PAPlayerCheckpoint - describe] m_isMoving: {}", m_isMoving);
    log::info("[PAPlayerCheckpoint - describe] m_forcedSlideLeft: {}", m_forcedSlideLeft);
    log::info("[PAPlayerCheckpoint - describe] m_forcedSlideRight: {}", m_forcedSlideRight);
    log::info("[PAPlayerCheckpoint - describe] m_lastSlideLeft: {}", m_lastSlideLeft);
    log::info("[PAPlayerCheckpoint - describe] m_slideSlopeAngle: {}", m_slideSlopeAngle);
    log::info("[PAPlayerCheckpoint - describe] m_unkUnused2: {}", m_unkUnused2);
    log::info("[PAPlayerCheckpoint - describe] m_stateNoAutoJump: {}", m_stateNoAutoJump);
    log::info("[PAPlayerCheckpoint - describe] m_stateDartSlide: {}", m_stateDartSlide);
    log::info("[PAPlayerCheckpoint - describe] m_stateHitHead: {}", m_stateHitHead);
    log::info("[PAPlayerCheckpoint - describe] m_stateFlipGravity: {}", m_stateFlipGravity);
    log::info("[PAPlayerCheckpoint - describe] m_stateForce: {}", m_stateForce);
    log::info("[PAPlayerCheckpoint - describe] m_stateForceVector: {}", m_stateForceVector);
    log::info("[PAPlayerCheckpoint - describe] m_affectedByForces: {}", m_affectedByForces);
    i = 0;
    for (std::pair<int, int> l_pair : m_touchedForceIds) {
        log::info("[PAPlayerCheckpoint - describe] m_touchedForceIds element {} key: {}", i, l_pair.first);
        log::info("[PAPlayerCheckpoint - describe] m_touchedForceIds element {} value: {}", i, l_pair.second);
        i++;
    }
    log::info("[PAPlayerCheckpoint - describe] m_fallStartY: {}", m_fallStartY);
}
#endif