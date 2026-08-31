#include "ui/color/ColorWheel.hpp"

#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>

#include <algorithm>
#include <cmath>

namespace Skink::Ui::Color {

namespace {

// Provisional geometry: SWINK did not define a custom color wheel.
constexpr int kWheelSize = 220;
constexpr qreal kOuterMargin = 8.0;
constexpr qreal kRingThickness = 24.0;
constexpr qreal kPi = 3.14159265358979323846;

} // namespace

ColorWheel::ColorWheel(QWidget* parent)
    : QWidget(parent)
{
    setFixedSize(kWheelSize, kWheelSize);
    setCursor(Qt::CrossCursor);
    setMouseTracking(true);
}

int ColorWheel::hue() const
{
    return m_hue;
}

void ColorWheel::setHue(int hue)
{
    const int normalizedHue = ((hue % 360) + 360) % 360;
    if (normalizedHue == m_hue) return;

    m_hue = normalizedHue;
    update();
    emit hueChanged(m_hue);
}

void ColorWheel::paintEvent(QPaintEvent*)
{
    if (m_wheelImage.size() != size()) rebuildWheelImage();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawImage(QPoint(0, 0), m_wheelImage);

    const QRectF bounds = wheelRect();
    const QPointF center = bounds.center();
    const qreal outerRadius = bounds.width() / 2.0;
    const qreal innerRadius = outerRadius - kRingThickness;
    const qreal indicatorRadius = (outerRadius + innerRadius) / 2.0;
    const qreal angle = (m_hue - 90.0) * kPi / 180.0;
    const QPointF indicator(
        center.x() + std::cos(angle) * indicatorRadius,
        center.y() + std::sin(angle) * indicatorRadius);

    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QColor(0, 0, 0, 180), 5.0));
    painter.drawEllipse(indicator, 6.0, 6.0);
    painter.setPen(QPen(Qt::white, 2.0));
    painter.drawEllipse(indicator, 6.0, 6.0);

    painter.setPen(QPen(QColor(255, 255, 255, 20), 1.0));
    painter.drawEllipse(bounds.adjusted(kRingThickness, kRingThickness, -kRingThickness, -kRingThickness));
}

void ColorWheel::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && isOnWheel(event->position())) {
        m_dragging = true;
        updateHueFromPosition(event->position());
        event->accept();
        return;
    }

    QWidget::mousePressEvent(event);
}

void ColorWheel::mouseMoveEvent(QMouseEvent* event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        updateHueFromPosition(event->position());
        event->accept();
        return;
    }

    QWidget::mouseMoveEvent(event);
}

void ColorWheel::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_dragging) {
        m_dragging = false;
        updateHueFromPosition(event->position());
        event->accept();
        return;
    }

    QWidget::mouseReleaseEvent(event);
}

QRectF ColorWheel::wheelRect() const
{
    const qreal diameter = std::min(width(), height()) - (2.0 * kOuterMargin);
    return {
        (width() - diameter) / 2.0,
        (height() - diameter) / 2.0,
        diameter,
        diameter
    };
}

bool ColorWheel::isOnWheel(const QPointF& position) const
{
    const QRectF bounds = wheelRect();
    const QPointF delta = position - bounds.center();
    const qreal distance = std::hypot(delta.x(), delta.y());
    const qreal outerRadius = bounds.width() / 2.0;
    const qreal innerRadius = outerRadius - kRingThickness;
    return distance >= innerRadius - 4.0 && distance <= outerRadius + 4.0;
}

void ColorWheel::rebuildWheelImage()
{
    m_wheelImage = QImage(size(), QImage::Format_ARGB32_Premultiplied);
    m_wheelImage.fill(Qt::transparent);

    const QRectF bounds = wheelRect();
    const QPointF center = bounds.center();
    const qreal outerRadius = bounds.width() / 2.0;
    const qreal innerRadius = outerRadius - kRingThickness;

    for (int y = 0; y < height(); ++y) {
        for (int x = 0; x < width(); ++x) {
            const qreal dx = (x + 0.5) - center.x();
            const qreal dy = (y + 0.5) - center.y();
            const qreal distance = std::hypot(dx, dy);
            if (distance < innerRadius || distance > outerRadius) continue;

            const qreal degrees = std::atan2(dy, dx) * 180.0 / kPi;
            const int hue = static_cast<int>(std::round(std::fmod(degrees + 450.0, 360.0))) % 360;
            m_wheelImage.setPixelColor(x, y, QColor::fromHsv(hue, 255, 255));
        }
    }
}

void ColorWheel::updateHueFromPosition(const QPointF& position)
{
    const QPointF delta = position - wheelRect().center();
    if (std::hypot(delta.x(), delta.y()) < 2.0) return;

    const qreal degrees = std::atan2(delta.y(), delta.x()) * 180.0 / kPi;
    setHue(static_cast<int>(std::round(std::fmod(degrees + 450.0, 360.0))) % 360);
}

} // namespace Skink::Ui::Color
