#pragma once
#include <algorithm>
#include <cmath>

namespace CompletionFlow {
    constexpr bool deferResult(bool animationStarted, bool completed, bool endScreenShown) {
        return (animationStarted || completed) && !endScreenShown;
    }
    inline float reportablePercent(float percent, bool completed) {
        if (!std::isfinite(percent)) return 0.f;
        return std::clamp(percent, 0.f, completed ? 100.f : 99.99f);
    }
}
