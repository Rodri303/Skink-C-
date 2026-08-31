#include "core/canvas/CanvasWidget.hpp"

#include <QKeyEvent>
#include <QKeySequence>
#include <QMouseEvent>
#include <QPainter>
#include <QTabletEvent>
#include <QWheelEvent>

#include <algorithm>

namespace Skink::Core::Canvas {

CanvasWidget::CanvasWidget(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TabletTracking, true);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(480, 320);
    setCursor(Qt::CrossCursor);

    auto settings = m_brush.settings();
    settings.color = QColor("#111111");
    settings.size = 18.0;
    m_brush.setSettings(settings);

    m_history.initialize(m_document.image());
}

void CanvasWidget::setBrushSize(qreal size)
{
    auto settings = m_brush.settings();
    settings.size = std::clamp(size, 1.0, 300.0);
    m_brush.setSettings(settings);
}

void CanvasWidget::setBrushColor(const QColor& color)
{
    if (!color.isValid()) return;
    auto settings = m_brush.settings();
    settings.color = color;
    m_brush.setSettings(settings);
}

void CanvasWidget::clearCanvas()
{
    m_document.clear();
    m_history.push(m_document.image());
    update();
}

void CanvasWidget::undo()
{
    QImage image = m_document.image();
    if (m_history.undo(image)) {
        m_document.replaceImage(image);
        update();
    }
}

void CanvasWidget::redo()
{
    QImage image = m_document.image();
    if (m_history.redo(image)) {
        m_document.replaceImage(image);
        update();
    }
}

void CanvasWidget::resetView()
{
    m_pan = {};
    m_zoom = 1.0;
    update();
}

void CanvasWidget::zoomIn()
{
    m_zoom = std::clamp(m_zoom * 1.1, 0.1, 8.0);
    update();
}

void CanvasWidget::zoomOut()
{
    m_zoom = std::clamp(m_zoom / 1.1, 0.1, 8.0);
    update();
}

int CanvasWidget::zoomPercent() const
{
    return qRound(m_zoom * 100.0);
}

void CanvasWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor("#252629"));
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QTransform transform = documentTransform();
    painter.setTransform(transform);

    const QRectF paperRect(QPointF(0.0, 0.0), m_document.size());

    painter.fillRect(paperRect.translated(0.0, 18.0), QColor(0, 0, 0, 34));
    painter.fillRect(paperRect.translated(0.0, 9.0), QColor(0, 0, 0, 24));
    painter.fillRect(paperRect, QColor("#ffffff"));
    painter.drawImage(QPointF(0.0, 0.0), m_document.image());
}

void CanvasWidget::mousePressEvent(QMouseEvent* event)
{
    setFocus(Qt::MouseFocusReason);

    if (event->button() == Qt::MiddleButton) {
        m_panning = true;
        m_lastPanPosition = event->position();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton) {
        const auto documentPosition = mapToDocument(event->position());
        if (documentPosition && isInsideDocument(*documentPosition)) {
            beginStroke({*documentPosition, 1.0, 0.0, 0.0, 0.0, 0.0, event->timestamp()});
            event->accept();
            return;
        }
    }

    QWidget::mousePressEvent(event);
}

void CanvasWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (m_panning && (event->buttons() & Qt::MiddleButton)) {
        const QPointF delta = event->position() - m_lastPanPosition;
        m_pan += delta;
        m_lastPanPosition = event->position();
        update();
        event->accept();
        return;
    }

    if (m_drawing && (event->buttons() & Qt::LeftButton)) {
        const auto documentPosition = mapToDocument(event->position());
        if (documentPosition) {
            continueStroke({*documentPosition, 1.0, 0.0, 0.0, 0.0, 0.0, event->timestamp()});
            event->accept();
            return;
        }
    }

    QWidget::mouseMoveEvent(event);
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton && m_panning) {
        m_panning = false;
        setCursor(Qt::CrossCursor);
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton && m_drawing) {
        const auto documentPosition = mapToDocument(event->position());
        if (documentPosition) {
            continueStroke({*documentPosition, 1.0, 0.0, 0.0, 0.0, 0.0, event->timestamp()});
        }
        endStroke();
        event->accept();
        return;
    }

    QWidget::mouseReleaseEvent(event);
}

void CanvasWidget::tabletEvent(QTabletEvent* event)
{
    setFocus(Qt::OtherFocusReason);
    const auto documentPosition = mapToDocument(event->position());

    switch (event->type()) {
    case QEvent::TabletPress:
        if (documentPosition && isInsideDocument(*documentPosition)) {
            beginStroke(tabletSample(*event, *documentPosition));
        }
        break;
    case QEvent::TabletMove:
        if (m_drawing && documentPosition) {
            continueStroke(tabletSample(*event, *documentPosition));
        }
        break;
    case QEvent::TabletRelease:
        if (m_drawing) {
            if (documentPosition && event->pressure() > 0.0) {
                continueStroke(tabletSample(*event, *documentPosition));
            }
            endStroke();
        }
        break;
    default:
        break;
    }

    event->accept();
}

void CanvasWidget::wheelEvent(QWheelEvent* event)
{
    const auto anchorDocument = mapToDocument(event->position());
    if (!anchorDocument) {
        QWidget::wheelEvent(event);
        return;
    }

    const qreal factor = event->angleDelta().y() > 0 ? 1.1 : 1.0 / 1.1;
    const qreal nextZoom = std::clamp(m_zoom * factor, 0.1, 8.0);
    if (qFuzzyCompare(nextZoom, m_zoom)) {
        event->accept();
        return;
    }

    m_zoom = nextZoom;
    const QPointF mappedAfterZoom = documentTransform().map(*anchorDocument);
    m_pan += event->position() - mappedAfterZoom;
    update();
    event->accept();
}

void CanvasWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->matches(QKeySequence::Undo)) {
        undo();
        event->accept();
        return;
    }
    if (event->matches(QKeySequence::Redo)) {
        redo();
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_0) {
        resetView();
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}

QTransform CanvasWidget::documentTransform() const
{
    QTransform transform;
    transform.translate(width() / 2.0 + m_pan.x(), height() / 2.0 + m_pan.y());
    transform.scale(m_zoom, m_zoom);
    transform.translate(-m_document.size().width() / 2.0, -m_document.size().height() / 2.0);
    return transform;
}

std::optional<QPointF> CanvasWidget::mapToDocument(const QPointF& viewportPosition) const
{
    bool invertible = false;
    const QTransform inverse = documentTransform().inverted(&invertible);
    if (!invertible) return std::nullopt;
    return inverse.map(viewportPosition);
}

bool CanvasWidget::isInsideDocument(const QPointF& documentPosition) const
{
    return documentPosition.x() >= 0.0
        && documentPosition.y() >= 0.0
        && documentPosition.x() < m_document.size().width()
        && documentPosition.y() < m_document.size().height();
}

Brush::BrushSample CanvasWidget::tabletSample(
    const QTabletEvent& event,
    const QPointF& documentPosition) const
{
    return {
        documentPosition,
        std::clamp<qreal>(event.pressure(), 0.0, 1.0),
        static_cast<qreal>(event.xTilt()),
        static_cast<qreal>(event.yTilt()),
        event.rotation(),
        event.tangentialPressure(),
        event.timestamp()
    };
}

void CanvasWidget::beginStroke(const Brush::BrushSample& sample)
{
    m_drawing = true;
    m_stroke.beginStroke(sample);
    m_brush.beginStroke(sample);

    QPainter painter(&m_document.image());
    Brush::BrushSample dotSample = sample;
    dotSample.position += QPointF(0.01, 0.01);
    m_brush.continueStroke(painter, dotSample);
    update();
}

void CanvasWidget::continueStroke(const Brush::BrushSample& sample)
{
    if (!m_drawing) return;

    QPainter painter(&m_document.image());
    const auto samples = m_stroke.processSample(sample, m_brush.settings().size);
    for (const auto& processedSample : samples) {
        m_brush.continueStroke(painter, processedSample);
    }
    update();
}

void CanvasWidget::endStroke()
{
    m_drawing = false;
    m_stroke.endStroke();
    m_brush.endStroke();
    m_history.push(m_document.image());
}

} // namespace Skink::Core::Canvas
