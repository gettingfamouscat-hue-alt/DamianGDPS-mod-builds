#include "hooks/KeyframeObject.hpp"
#include "hooks/cocos2d/CCNode.hpp"
#include "hooks/tk_spline.hpp"
#include "util/debug.hpp"

using namespace geode::prelude;
using namespace persistenceAPI;

void PAKeyframeObject::load(Stream& i_stream) {
    i_stream >> *this;
}

void PAKeyframeObject::save(Stream& o_stream) {
    o_stream << *this;
}

inline void persistenceAPI::operator>>(Stream& i_stream, PAKeyframeObject& o_value) {
    if (i_stream.getPAVersion() > 1) {
        i_stream >> o_value.m_duration;
        SEPARATOR_I
        i_stream >> o_value.m_easingType;
        SEPARATOR_I
        i_stream >> o_value.m_easingRate;
        SEPARATOR_I
        i_stream >> o_value.m_refOnly;
        SEPARATOR_I
        i_stream >> o_value.m_spawnGroup;
        SEPARATOR_I
        i_stream >> o_value.m_spawnDelay;
        SEPARATOR_I
        i_stream >> o_value.m_proximity;
        SEPARATOR_I
        i_stream >> o_value.m_spawned;
        SEPARATOR_I
        i_stream >> o_value.m_curve;
    } else {
        i_stream.read(reinterpret_cast<char*>(&o_value), 32);
    }
    VEC_SEPARATOR_I
    if (i_stream.getPAVersion() > 1) {
        reinterpret_cast<PAtk_spline*>(&o_value.m_spline1)->load(i_stream);
        VEC_SEPARATOR_I
        reinterpret_cast<PAtk_spline*>(&o_value.m_spline2)->load(i_stream);
        i_stream >> o_value.m_curveLength;
        SEPARATOR_I
        i_stream >> o_value.m_refRunDuration;
        SEPARATOR_I
        i_stream >> o_value.m_timeMode;
        SEPARATOR_I
        i_stream >> o_value.m_keyframeObject;
        SEPARATOR_I
        i_stream >> o_value.m_positionDelta;
        SEPARATOR_I
        i_stream >> o_value.m_position;
        SEPARATOR_I
        i_stream >> o_value.m_scaleX;
        SEPARATOR_I
        i_stream >> o_value.m_scaleY;
        SEPARATOR_I
        i_stream >> o_value.m_rotation;
        SEPARATOR_I
        i_stream >> o_value.m_unk1b0;
        SEPARATOR_I
        i_stream >> o_value.m_opacity;
    } else {
        i_stream >> o_value.m_spline1.m_x;
        VEC_SEPARATOR_I
        i_stream >> o_value.m_spline1.m_y;
        VEC_SEPARATOR_I
        i_stream >> o_value.m_spline1.m_b;
        VEC_SEPARATOR_I
        i_stream >> o_value.m_spline1.m_c;
        VEC_SEPARATOR_I
        i_stream >> o_value.m_spline1.m_d;
        VEC_SEPARATOR_I
        SEPARATOR_I
        i_stream.read(reinterpret_cast<char*>(&o_value) + offsetof(PAKeyframeObject,m_spline1) + sizeof(std::vector<double>)*5, 41);
        SEPARATOR_I
        VEC_SEPARATOR_I
        i_stream >> o_value.m_spline2.m_x;
        VEC_SEPARATOR_I
        i_stream >> o_value.m_spline2.m_y;
        VEC_SEPARATOR_I
        i_stream >> o_value.m_spline2.m_b;
        VEC_SEPARATOR_I
        i_stream >> o_value.m_spline2.m_c;
        VEC_SEPARATOR_I
        i_stream >> o_value.m_spline2.m_d;
        VEC_SEPARATOR_I
        SEPARATOR_I
        i_stream.read(reinterpret_cast<char*>(&o_value) + offsetof(PAKeyframeObject,m_spline2) + sizeof(std::vector<double>)*5, 128);
    }
    SEPARATOR_I
}

inline void persistenceAPI::operator<<(Stream& o_stream, PAKeyframeObject& i_value) {
    o_stream << i_value.m_duration;
    SEPARATOR_O
    o_stream << i_value.m_easingType;
    SEPARATOR_O
    o_stream << i_value.m_easingRate;
    SEPARATOR_O
    o_stream << i_value.m_refOnly;
    SEPARATOR_O
    o_stream << i_value.m_spawnGroup;
    SEPARATOR_O
    o_stream << i_value.m_spawnDelay;
    SEPARATOR_O
    o_stream << i_value.m_proximity;
    SEPARATOR_O
    o_stream << i_value.m_spawned;
    SEPARATOR_O
    o_stream << i_value.m_curve;
    VEC_SEPARATOR_O
    reinterpret_cast<PAtk_spline*>(&i_value.m_spline1)->save(o_stream);
    VEC_SEPARATOR_O
    reinterpret_cast<PAtk_spline*>(&i_value.m_spline2)->save(o_stream);
    o_stream << i_value.m_curveLength;
    SEPARATOR_O
    o_stream << i_value.m_refRunDuration;
    SEPARATOR_O
    o_stream << i_value.m_timeMode;
    SEPARATOR_O
    o_stream << i_value.m_keyframeObject;
    SEPARATOR_O
    o_stream << i_value.m_positionDelta;
    SEPARATOR_O
    o_stream << i_value.m_position;
    SEPARATOR_O
    o_stream << i_value.m_scaleX;
    SEPARATOR_O
    o_stream << i_value.m_scaleY;
    SEPARATOR_O
    o_stream << i_value.m_rotation;
    SEPARATOR_O
    o_stream << i_value.m_unk1b0;
    SEPARATOR_O
    o_stream << i_value.m_opacity;
    SEPARATOR_O
}

#if defined(PA_DEBUG) && defined(PA_DESCRIBE)
void PAKeyframeObject::describe() {
    log::info("[PAKeyframeObject - describe] m_duration: {}", m_duration);
    log::info("[PAKeyframeObject - describe] m_easingType: {}", m_easingType);
    log::info("[PAKeyframeObject - describe] m_easingRate: {}", m_easingRate);
    log::info("[PAKeyframeObject - describe] m_refOnly: {}", m_refOnly);
    log::info("[PAKeyframeObject - describe] m_spawnGroup: {}", m_spawnGroup);
    log::info("[PAKeyframeObject - describe] m_spawnDelay: {}", m_spawnDelay);
    log::info("[PAKeyframeObject - describe] m_proximity: {}", m_proximity);
    log::info("[PAKeyframeObject - describe] m_spawned: {}", m_spawned);
    log::info("[PAKeyframeObject - describe] m_curve: {}", m_curve);
    reinterpret_cast<PAtk_spline*>(&m_spline1)->describe();
    reinterpret_cast<PAtk_spline*>(&m_spline2)->describe();
    log::info("[PAKeyframeObject - describe] m_curveLength: {}", m_curveLength);
    log::info("[PAKeyframeObject - describe] m_refRunDuration: {}", m_refRunDuration);
    log::info("[PAKeyframeObject - describe] m_timeMode: {}", m_timeMode);
    log::info("[PAKeyframeObject - describe] m_keyframeObject: {}", m_keyframeObject);
    log::info("[PAKeyframeObject - describe] m_positionDelta: {}", m_positionDelta);
    log::info("[PAKeyframeObject - describe] m_position: {}", m_position);
    log::info("[PAKeyframeObject - describe] m_scaleX: {}", m_scaleX);
    log::info("[PAKeyframeObject - describe] m_scaleY: {}", m_scaleY);
    log::info("[PAKeyframeObject - describe] m_rotation: {}", m_rotation);
    log::info("[PAKeyframeObject - describe] m_unk1b0: {}", m_unk1b0);
    log::info("[PAKeyframeObject - describe] m_opacity: {}", m_opacity);
}
#endif