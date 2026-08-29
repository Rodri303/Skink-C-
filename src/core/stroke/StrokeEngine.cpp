#include "core/stroke/StrokeEngine.hpp"

#include <algorithm>
#include <cmath>

namespace Skink::Core::Stroke {

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
            QPointF(
                previous.position.x() + dx * t,
                previous.position.y() + dy * t),
            previous.pressure + (current.pressure - previous.pressure) * t
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

    return {
        QPointF(
            previous.position.x() + (raw.position.x() - previous.position.x()) * follow,
            previous.position.y() + (raw.position.y() - previous.position.y()) * follow),
        previous.pressure + (raw.pressure - previous.pressure) * follow
    };
}

} // namespace Skink::Core::Stroke
