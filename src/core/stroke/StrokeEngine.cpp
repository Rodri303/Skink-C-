#include "core/stroke/StrokeEngine.hpp"

#include <algorithm>
#include <cmath>

namespace Skink::Core::Stroke {

namespace {
qreal lerp(qreal a, qreal b, qreal t)
{
    return a + (b - a) * t;
}
}

void StrokeEngine::setSettings(const StrokeSettings& settings)
{
    m_settings.smoothing = std::clamp(settings.smoothing, 0.0, 0.95);
    m_settings.spacingFactor = std::clamp(settings.spacingFactor, 0.01, 1.0);
    m_settings.minimumSpacing = std::max<qreal>(0.05, settings.minimumSpacing);
}

StrokeSettings StrokeEngine::settings() const noexcept
{
    return m_settings;
}

void StrokeEngine::beginStroke(const Brush::BrushSample& sample)
{
    m_previousSmooth = sample;
}

std::vector<Brush::BrushSample> StrokeEngine::processSample(
    const Brush::BrushSample& rawSample,
    qreal brushSize)
{
    if (!m_previousSmooth.has_value()) {
        beginStroke(rawSample);
        return {rawSample};
    }

    const Brush::BrushSample current = smooth(rawSample);
    const Brush::BrushSample previous = *m_previousSmooth;

    const qreal dx = current.position.x() - previous.position.x();
    const qreal dy = current.position.y() - previous.position.y();
    const qreal distance = std::hypot(dx, dy);
    const qreal spacing = std::max(
        m_settings.minimumSpacing,
        std::max<qreal>(1.0, brushSize) * m_settings.spacingFactor);
    const int steps = std::max(1, static_cast<int>(std::ceil(distance / spacing)));

    std::vector<Brush::BrushSample> samples;
    samples.reserve(static_cast<std::size_t>(steps));

    for (int index = 1; index <= steps; ++index) {
        const qreal t = static_cast<qreal>(index) / static_cast<qreal>(steps);
        samples.push_back({
            QPointF(lerp(previous.position.x(), current.position.x(), t),
                    lerp(previous.position.y(), current.position.y(), t)),
            lerp(previous.pressure, current.pressure, t),
            lerp(previous.xTilt, current.xTilt, t),
            lerp(previous.yTilt, current.yTilt, t),
            lerp(previous.rotation, current.rotation, t),
            lerp(previous.tangentialPressure, current.tangentialPressure, t),
            static_cast<quint64>(lerp(
                static_cast<qreal>(previous.timestamp),
                static_cast<qreal>(current.timestamp),
                t))
        });
    }

    m_previousSmooth = current;
    return samples;
}

void StrokeEngine::endStroke()
{
    m_previousSmooth.reset();
}

Brush::BrushSample StrokeEngine::smooth(const Brush::BrushSample& raw) const
{
    if (!m_previousSmooth.has_value()) {
        return raw;
    }

    const qreal follow = std::max<qreal>(0.05, 1.0 - m_settings.smoothing);
    const auto& previous = *m_previousSmooth;

    Brush::BrushSample sample = raw;
    sample.position = QPointF(
        lerp(previous.position.x(), raw.position.x(), follow),
        lerp(previous.position.y(), raw.position.y(), follow));
    sample.pressure = lerp(previous.pressure, raw.pressure, follow);
    sample.xTilt = lerp(previous.xTilt, raw.xTilt, follow);
    sample.yTilt = lerp(previous.yTilt, raw.yTilt, follow);
    sample.rotation = lerp(previous.rotation, raw.rotation, follow);
    sample.tangentialPressure = lerp(previous.tangentialPressure, raw.tangentialPressure, follow);
    return sample;
}

} // namespace Skink::Core::Stroke
