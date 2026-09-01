#include "core/brush/BrushController.hpp"

#include <algorithm>

namespace Skink::Core::Brush {

BrushController::BrushController(QObject* parent)
    : QObject(parent)
{
}

const BrushState& BrushController::state() const noexcept
{
    return m_state;
}

void BrushController::setSize(int size)
{
    const int boundedSize = std::clamp(size, kMinimumBrushSize, kMaximumBrushSize);
    if (m_state.size == boundedSize) return;

    m_state.size = boundedSize;
    emit sizeChanged(m_state.size);
    notifyStateChanged();
}

void BrushController::adjustSize(int delta)
{
    setSize(m_state.size + delta);
}

void BrushController::setOpacity(int opacity)
{
    const int boundedOpacity = std::clamp(
        opacity,
        kMinimumBrushOpacity,
        kMaximumBrushOpacity);
    if (m_state.opacity == boundedOpacity) return;

    m_state.opacity = boundedOpacity;
    emit opacityChanged(m_state.opacity);
    notifyStateChanged();
}

void BrushController::setPressureSensitivity(int sensitivity)
{
    const int boundedSensitivity = std::clamp(
        sensitivity,
        kMinimumPressureSensitivity,
        kMaximumPressureSensitivity);
    if (m_state.pressureSensitivity == boundedSensitivity) return;

    m_state.pressureSensitivity = boundedSensitivity;
    emit pressureSensitivityChanged(m_state.pressureSensitivity);
    notifyStateChanged();
}

void BrushController::setColor(const QColor& color)
{
    if (!color.isValid()) return;

    QColor opaqueColor = color.toRgb();
    opaqueColor.setAlpha(255);
    if (m_state.color == opaqueColor) return;

    m_state.color = opaqueColor;
    emit colorChanged(m_state.color);
    notifyStateChanged();
}

void BrushController::setPreset(BrushPreset preset)
{
    if (m_state.preset == preset) return;

    m_state.preset = preset;
    emit presetChanged(m_state.preset);
    notifyStateChanged();
}

void BrushController::notifyStateChanged()
{
    emit stateChanged(m_state);
}

} // namespace Skink::Core::Brush
