#include "core/brush/BrushDynamics.hpp"

#include <algorithm>
#include <cmath>

namespace Skink::Core::Brush {

qreal applyPressureResponse(qreal physicalPressure, qreal sensitivity) noexcept
{
    const qreal pressure = std::clamp(physicalPressure, 0.0, 1.0);
    if (qFuzzyIsNull(pressure)) return 0.0;

    const qreal boundedSensitivity = std::clamp(sensitivity, 0.0, 1.0);
    const qreal exponent = 0.35 + boundedSensitivity * 1.65;
    const qreal adjusted = 1.0 - std::pow(
        1.0 - std::max<qreal>(0.01, pressure),
        exponent);
    return std::clamp(adjusted, 0.0, 1.0);
}

BrushDabSettings brushDabSettings(
    BrushPreset preset,
    qreal baseSize,
    qreal globalOpacity,
    qreal pressure) noexcept
{
    const qreal size = std::max<qreal>(0.0, baseSize);
    const qreal opacity = std::clamp(globalOpacity, 0.0, 1.0);
    const qreal boundedPressure = std::clamp(pressure, 0.0, 1.0);

    BrushDabSettings settings;
    switch (preset) {
    case BrushPreset::TransparentInk:
        settings.diameter = size * (0.18 + boundedPressure * 0.90);
        settings.opacity = (0.16 + boundedPressure * 0.25) * opacity;
        settings.mode = BrushRenderingMode::Solid;
        break;
    case BrushPreset::Marker:
        settings.diameter = size * (0.78 + boundedPressure * 0.18);
        settings.opacity = 0.16 * opacity;
        settings.mode = BrushRenderingMode::Solid;
        break;
    case BrushPreset::Pencil:
        settings.diameter = size * (0.38 + boundedPressure * 0.55);
        settings.opacity = (0.08 + boundedPressure * 0.22) * opacity;
        settings.mode = BrushRenderingMode::Grain;
        break;
    case BrushPreset::Airbrush:
        settings.diameter = size * (0.72 + boundedPressure * 0.55);
        settings.opacity = (0.04 + boundedPressure * 0.08) * opacity;
        settings.mode = BrushRenderingMode::Soft;
        break;
    }

    settings.diameter = std::max<qreal>(1.0, settings.diameter);
    settings.opacity = std::clamp(settings.opacity, 0.0, 1.0);
    return settings;
}

qreal strokeSpacingFactor(BrushPreset preset) noexcept
{
    // SWINK expresses spacing as a fraction of radius. StrokeEngine currently
    // multiplies a base diameter, so the factors are converted by one half.
    switch (preset) {
    case BrushPreset::Pencil:
        return 0.12;
    case BrushPreset::Airbrush:
        return 0.09;
    case BrushPreset::TransparentInk:
    case BrushPreset::Marker:
        return 0.07;
    }

    return 0.07;
}

} // namespace Skink::Core::Brush
