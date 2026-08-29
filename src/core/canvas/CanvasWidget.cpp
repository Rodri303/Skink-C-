#include "core/canvas/CanvasWidget.hpp"

#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QTabletEvent>

#include <algorithm>

namespace Skink::Core::Canvas {

CanvasWidget::CanvasWidget(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TabletTracking, true);
    setMouseTracking(true);
    setMinimumSize(480, 320);
    setCursor(Qt::CrossCursor);

    auto settings = m_brush.settings();
    settings.color = QColor("#111111");
    settings.size = 18.0;
    m_brush.setSettings(settings);
}

void CanvasWidget::setBrushSize(qreal size)
{
    auto settings = m_brush.settings();
    settings.size = std::clamp(size, 1.0, 300.0);
    m_brush.setSettings(settings);
}

void CanvasWidget::setBrushColor(const QColor& color)
{
    if (!color.isValid()) {
        return;
    }
    auto settings = m_brush.settings();
    settings.color = color;
    m_brush.setSettings(settings);
}

void CanvasWidget::clearCanvas()
{
    ensureImageSize();
    m_image.fill(Qt::transparent);
    update();
}

void CanvasWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor("#2a2a2a"));

    const QRectF paperRect = rect().adjusted(26, 26, -26, -26);
    painter.fillRect(paperRect, QColor("#f2f0e9"));

    if (!m_image.isNull()) {
        painter.drawImage(QPoint(0, 0), m_image);
    }
}

void CanvasWidget::resizeEvent(QResizeEvent*)
{
    ensureImageSize();
}

void CanvasWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        beginStroke(event->position(), 1.0);
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void CanvasWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (m_drawing && (event->buttons() & Qt::LeftButton)) {
        continueStroke(event->position(), 1.0);
        event->accept();
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_drawing) {
        continueStroke(event->position(), 1.0);
        endStroke();
        event->accept();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

void CanvasWidget::tabletEvent(QTabletEvent* event)
{
    const qreal pressure = std::clamp<qreal>(event->pressure(), 0.0, 1.0);

    switch (event->type()) {
    case QEvent::TabletPress:
        beginStroke(event->position(), pressure);
        break;
    case QEvent::TabletMove:
        if (m_drawing) {
            continueStroke(event->position(), pressure);
        }
        break;
    case QEvent::TabletRelease:
        if (m_drawing) {
            continueStroke(event->position(), pressure);
            endStroke();
        }
        break;
    default:
        break;
    }

    event->accept();
}

void CanvasWidget::ensureImageSize()
{
    if (size().isEmpty()) {
        return;
    }

    if (m_image.size() == size()) {
        return;
    }

    QImage resized(size(), QImage::Format_ARGB32_Premultiplied);
    resized.fill(Qt::transparent);

    if (!m_image.isNull()) {
        QPainter painter(&resized);
        painter.drawImage(QPoint(0, 0), m_image);
    }

    m_image = std::move(resized);
}

void CanvasWidget::beginStroke(const QPointF& position, qreal pressure)
{
    ensureImageSize();
    m_drawing = true;
    m_brush.beginStroke({position, pressure});
    continueStroke(position + QPointF(0.01, 0.01), pressure);
}

void CanvasWidget::continueStroke(const QPointF& position, qreal pressure)
{
    ensureImageSize();
    QPainter painter(&m_image);
    m_brush.continueStroke(painter, {position, pressure});
    update();
}

void CanvasWidget::endStroke()
{
    m_drawing = false;
    m_brush.endStroke();
}

} // namespace Skink::Core::Canvas
