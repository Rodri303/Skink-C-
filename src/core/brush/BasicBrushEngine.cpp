#include "core/brush/BasicBrushEngine.hpp"
#include "core/brush/BrushDynamics.hpp"

#include <QPainter>
#include <QPen>

#include <algorithm>

namespace Skink::Core::Brush {

void BasicBrushEngine::setSettings(const BrushSettings& settings)
{
    m_settings = settings;
}

BrushSettings BasicBrushEngine::settings() const
{
    return m_settings;
}

void BasicBrushEngine::beginStroke(const BrushSample& sample)
{
    m_previous = sample;
}

void BasicBrushEngine::continueStroke(QPainter& painter, const BrushSample& sample)
{
    if (!m_previous.has_value()) {
        m_previous = sample;
        return;
    }

    if (m_settings.eraser) {
        const qreal pressure = std::clamp(sample.pressure, 0.05, 1.0);
        QColor color = m_settings.color;
        color.setAlphaF(1.0);

        QPen pen(color);
        pen.setWidthF(std::max<qreal>(1.0, m_settings.size * pressure));
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::RoundJoin);

        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen(pen);
        painter.drawLine(m_previous->position, sample.position);

        m_previous = sample;
        return;
    }

    const qreal pressure = std::clamp(sample.pressure, 0.0, 1.0);
    const BrushDabSettings dab = brushDabSettings(
        m_settings.preset,
        m_settings.size,
        m_settings.opacity,
        pressure);
    QColor color = m_settings.color;
    color.setAlphaF(dab.opacity);

    QPen pen(color);
    pen.setWidthF(dab.diameter);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(pen);
    painter.drawLine(m_previous->position, sample.position);

    m_previous = sample;
}

void BasicBrushEngine::endStroke()
{
    m_previous.reset();
}

} // namespace Skink::Core::Brush
