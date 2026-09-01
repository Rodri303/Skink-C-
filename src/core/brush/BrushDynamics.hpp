#pragma once

#include "core/brush/BrushState.hpp"

#include <QtGlobal>

namespace Skink::Core::Brush {

enum class BrushRenderingMode {
    Solid,
    Grain,
    Soft
};

struct BrushDabSettings {
    qreal diameter{1.0};
    qreal opacity{1.0};
    BrushRenderingMode mode{BrushRenderingMode::Solid};
};

[[nodiscard]] qreal applyPressureResponse(
    qreal physicalPressure,
    qreal sensitivity) noexcept;

[[nodiscard]] BrushDabSettings brushDabSettings(
    BrushPreset preset,
    qreal baseSize,
    qreal globalOpacity,
    qreal pressure) noexcept;

[[nodiscard]] qreal strokeSpacingFactor(BrushPreset preset) noexcept;

} // namespace Skink::Core::Brush
