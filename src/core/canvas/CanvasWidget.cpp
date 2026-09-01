#include "core/canvas/CanvasWidget.hpp"

#include "core/brush/BrushDynamics.hpp"

#include <QDebug>
#include <QKeyEvent>
#include <QKeySequence>
#include <QMouseEvent>
#include <QPainter>
#include <QPointingDevice>
#include <QTabletEvent>
#include <QWheelEvent>

#include <algorithm>
#include <cmath>
#include <numbers>

namespace Skink::Core::Canvas {

CanvasWidget::CanvasWidget(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TabletTracking, true);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(480, 320);
    setCursor(Qt::CrossCursor);

    applyBrushState();

    m_history.initialize(m_document.image());
}

void CanvasWidget::setBrushState(const Brush::BrushState& state)
{
    m_brushState = state;
    applyBrushState();
}

void CanvasWidget::applyBrushState()
{
    auto settings = m_brush.settings();
    settings.size = m_brushState.size;
    settings.pressureSensitivity = m_brushState.pressureSensitivity / 100.0;
    settings.preset = m_brushState.preset;
    settings.eraser = m_activeTool == Tools::Tool::Eraser;
    if (m_activeTool == Tools::Tool::Eraser) {
        settings.color = Qt::white;
        settings.opacity = 1.0;
    } else {
        settings.color = m_brushState.color;
        settings.opacity = m_brushState.opacity / 100.0;
    }
    m_brush.setSettings(settings);

    auto strokeSettings = m_stroke.settings();
    if (m_activeTool == Tools::Tool::Eraser) {
        strokeSettings.smoothing = 0.25;
        strokeSettings.spacingFactor = 0.14;
    } else {
        strokeSettings.smoothing = 0.35;
        strokeSettings.spacingFactor = Brush::strokeSpacingFactor(m_brushState.preset);
    }
    m_stroke.setSettings(strokeSettings);
}

void CanvasWidget::setActiveTool(Tools::Tool tool)
{
    if (m_activeTool == tool) return;
    if (m_drawing) endStroke();

    m_activeTool = tool;
    applyBrushState();
    updateNavigationCursor();
}

void CanvasWidget::setTemporaryPan(bool active)
{
    if (m_temporaryPan == active) return;

    m_temporaryPan = active;
    if (m_temporaryPan && m_drawing) endStroke();
    if (!m_temporaryPan
        && m_navigationGesture == NavigationGesture::Pan
        && m_panRequiresTemporary) {
        endNavigationGesture();
    }
    updateNavigationCursor();
}

void CanvasWidget::setNavigationModifiers(bool control, bool alt, bool shift)
{
    m_controlHeld = control;
    m_altHeld = alt;
    m_shiftHeld = shift;

    if (m_navigationGesture == NavigationGesture::Rotation && !m_controlHeld) {
        endNavigationGesture();
    } else if (m_navigationGesture == NavigationGesture::DragZoom && !m_altHeld) {
        endNavigationGesture();
    }
}

void CanvasWidget::cancelNavigation()
{
    m_temporaryPan = false;
    m_controlHeld = false;
    m_altHeld = false;
    m_shiftHeld = false;
    endNavigationGesture();
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
    endNavigationGesture();
    m_pan = {};
    const bool zoomChangedValue = !qFuzzyCompare(m_zoom, 1.0);
    const bool rotationChangedValue = !qFuzzyIsNull(m_rotationDegrees);
    m_zoom = 1.0;
    m_rotationDegrees = 0.0;
    update();
    if (zoomChangedValue) emit zoomChanged(zoomPercent());
    if (rotationChangedValue) emit rotationChanged(0);
}

void CanvasWidget::zoomIn()
{
    applyZoom(1.1, rect().center());
}

void CanvasWidget::zoomOut()
{
    applyZoom(1.0 / 1.1, rect().center());
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

    if (beginNavigation(event->position(), event->button(), false)) {
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton && drawingToolActive()) {
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
    if (m_navigationGesture != NavigationGesture::None && !m_navigationUsesTablet) {
        if (!(event->buttons() & m_navigationMouseButton)) {
            endNavigationGesture();
        } else if (m_navigationGesture == NavigationGesture::Pan) {
            updatePan(event->position());
        } else if (m_navigationGesture == NavigationGesture::Rotation) {
            updateRotation(event->position());
        } else if (m_navigationGesture == NavigationGesture::DragZoom) {
            updateDragZoom(event->position());
        }
        event->accept();
        return;
    }

    if ((event->buttons() & Qt::LeftButton)
        && beginNavigation(event->position(), Qt::LeftButton, false)) {
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
    if (m_navigationGesture != NavigationGesture::None
        && !m_navigationUsesTablet
        && event->button() == m_navigationMouseButton) {
        endNavigationGesture();
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
    logTabletDiagnostic(*event);
    const auto documentPosition = mapToDocument(event->position());

    switch (event->type()) {
    case QEvent::TabletPress:
        if (beginNavigation(event->position(), Qt::LeftButton, true)) {
            break;
        }
        if (drawingToolActive() && documentPosition && isInsideDocument(*documentPosition)) {
            beginStroke(tabletSample(*event, *documentPosition));
        }
        break;
    case QEvent::TabletMove:
        if (m_navigationGesture == NavigationGesture::Pan && m_navigationUsesTablet) {
            updatePan(event->position());
        } else if (m_navigationGesture == NavigationGesture::Rotation && m_navigationUsesTablet) {
            updateRotation(event->position());
        } else if (m_navigationGesture == NavigationGesture::DragZoom && m_navigationUsesTablet) {
            updateDragZoom(event->position());
        } else if (event->pressure() > 0.0
                   && beginNavigation(event->position(), Qt::LeftButton, true)) {
            break;
        } else if (m_drawing && documentPosition) {
            continueStroke(tabletSample(*event, *documentPosition));
        }
        break;
    case QEvent::TabletRelease:
        if (m_navigationGesture != NavigationGesture::None && m_navigationUsesTablet) {
            endNavigationGesture();
        } else if (m_drawing) {
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

void CanvasWidget::logTabletDiagnostic(const QTabletEvent& event)
{
    constexpr qint64 moveSampleIntervalMs = 750;

    const bool contact = event.pressure() > 0.0;
    const int pointerType = static_cast<int>(event.pointerType());
    const bool stateChanged = !m_tabletDiagnosticInitialized
        || event.buttons() != m_lastTabletButtons
        || event.modifiers() != m_lastTabletModifiers
        || contact != m_lastTabletContact
        || pointerType != m_lastTabletPointerType;
    const bool transition = event.type() == QEvent::TabletPress
        || event.type() == QEvent::TabletRelease;
    const bool activeMoveSample = event.type() == QEvent::TabletMove
        && (contact || event.buttons() != Qt::NoButton)
        && m_tabletDiagnosticTimer.isValid()
        && m_tabletDiagnosticTimer.elapsed() >= moveSampleIntervalMs;

    if (stateChanged || transition || activeMoveSample) {
        const char* eventName = "TabletOther";
        if (event.type() == QEvent::TabletPress) eventName = "TabletPress";
        if (event.type() == QEvent::TabletMove) eventName = "TabletMove";
        if (event.type() == QEvent::TabletRelease) eventName = "TabletRelease";

        const QPointingDevice* device = event.pointingDevice();
        qInfo().noquote().nospace()
            << "[WACOM-DIAG][TABLET] event=" << eventName
            << " timestamp=" << event.timestamp()
            << " button=" << static_cast<int>(event.button())
            << " buttons=0x" << QString::number(event.buttons().toInt(), 16)
            << " modifiers=0x" << QString::number(event.modifiers().toInt(), 16)
            << " pressure=" << QString::number(event.pressure(), 'f', 4)
            << " contact=" << (contact ? "tip" : "hover")
            << " pointerType=" << pointerType
            << " deviceType="
            << (device ? static_cast<int>(device->type()) : -1)
            << " deviceName=\""
            << (device ? device->name() : QStringLiteral("unknown"))
            << "\" deviceId=" << (device ? device->systemId() : -1)
            << " position=(" << QString::number(event.position().x(), 'f', 1)
            << ',' << QString::number(event.position().y(), 'f', 1) << ')'
            << " router={ctrl=" << m_controlHeld
            << ",alt=" << m_altHeld
            << ",shift=" << m_shiftHeld
            << ",space=" << m_temporaryPan << '}';
        m_tabletDiagnosticTimer.restart();
    }

    if (!m_tabletDiagnosticTimer.isValid()) m_tabletDiagnosticTimer.start();
    m_tabletDiagnosticInitialized = true;
    m_lastTabletButtons = event.buttons();
    m_lastTabletModifiers = event.modifiers();
    m_lastTabletContact = contact;
    m_lastTabletPointerType = pointerType;
}

bool CanvasWidget::drawingToolActive() const noexcept
{
    return m_activeTool == Tools::Tool::Brush || m_activeTool == Tools::Tool::Eraser;
}

bool CanvasWidget::navigationPanActive() const noexcept
{
    return m_activeTool == Tools::Tool::Pan || m_temporaryPan;
}

bool CanvasWidget::beginNavigation(
    const QPointF& position,
    Qt::MouseButton mouseButton,
    bool tablet)
{
    if (m_navigationGesture != NavigationGesture::None) return true;

    if (mouseButton == Qt::MiddleButton) {
        beginPan(position, mouseButton, tablet);
        return true;
    }
    if (mouseButton != Qt::LeftButton) return false;

    if (navigationPanActive()) {
        beginPan(position, mouseButton, tablet);
        return true;
    }
    if (m_controlHeld) {
        beginRotation(position, mouseButton, tablet);
        return true;
    }
    if (m_altHeld) {
        beginDragZoom(position, mouseButton, tablet);
        return true;
    }
    return false;
}

void CanvasWidget::beginPan(const QPointF& position, Qt::MouseButton mouseButton, bool tablet)
{
    if (m_drawing) endStroke();

    m_navigationGesture = NavigationGesture::Pan;
    m_navigationUsesTablet = tablet;
    m_navigationMouseButton = mouseButton;
    m_panRequiresTemporary = m_temporaryPan && m_activeTool != Tools::Tool::Pan;
    m_lastPanPosition = position;
    updateNavigationCursor();
}

void CanvasWidget::updatePan(const QPointF& position)
{
    m_pan += position - m_lastPanPosition;
    m_lastPanPosition = position;
    update();
}

void CanvasWidget::beginRotation(
    const QPointF& position,
    Qt::MouseButton mouseButton,
    bool tablet)
{
    if (m_drawing) endStroke();

    m_navigationGesture = NavigationGesture::Rotation;
    m_navigationUsesTablet = tablet;
    m_navigationMouseButton = mouseButton;
    m_rotationCenter = QPointF(width() / 2.0 + m_pan.x(), height() / 2.0 + m_pan.y());
    m_rotationStartDegrees = m_rotationDegrees;
    m_rotationStartPointerAngle = std::atan2(
        position.y() - m_rotationCenter.y(),
        position.x() - m_rotationCenter.x());
    updateNavigationCursor();
    emit rotationChanged(qRound(m_rotationDegrees));
}

void CanvasWidget::updateRotation(const QPointF& position)
{
    const qreal pointerAngle = std::atan2(
        position.y() - m_rotationCenter.y(),
        position.x() - m_rotationCenter.x());
    const qreal deltaRadians = std::remainder(
        pointerAngle - m_rotationStartPointerAngle,
        2.0 * std::numbers::pi_v<qreal>);
    qreal nextRotation = m_rotationStartDegrees
        + deltaRadians * 180.0 / std::numbers::pi_v<qreal>;
    if (m_shiftHeld) nextRotation = std::round(nextRotation / 15.0) * 15.0;
    if (qFuzzyIsNull(nextRotation - m_rotationDegrees)) return;

    m_rotationDegrees = nextRotation;
    update();
    emit rotationChanged(qRound(m_rotationDegrees));
}

void CanvasWidget::beginDragZoom(
    const QPointF& position,
    Qt::MouseButton mouseButton,
    bool tablet)
{
    if (m_drawing) endStroke();

    m_navigationGesture = NavigationGesture::DragZoom;
    m_navigationUsesTablet = tablet;
    m_navigationMouseButton = mouseButton;
    m_dragZoomLastX = position.x();
    updateNavigationCursor();
    emit zoomChanged(zoomPercent());
}

void CanvasWidget::updateDragZoom(const QPointF& position)
{
    const qreal deltaX = position.x() - m_dragZoomLastX;
    m_dragZoomLastX = position.x();
    const qreal sensitivity = m_shiftHeld ? 0.0022 : 0.0055;
    applyZoom(std::exp(deltaX * sensitivity), rect().center());
}

void CanvasWidget::endNavigationGesture()
{
    m_navigationGesture = NavigationGesture::None;
    m_navigationUsesTablet = false;
    m_panRequiresTemporary = false;
    m_navigationMouseButton = Qt::NoButton;
    updateNavigationCursor();
}

void CanvasWidget::updateNavigationCursor()
{
    if (m_navigationGesture == NavigationGesture::Pan) {
        setCursor(Qt::ClosedHandCursor);
    } else if (m_navigationGesture == NavigationGesture::Rotation) {
        setCursor(Qt::SizeAllCursor);
    } else if (m_navigationGesture == NavigationGesture::DragZoom) {
        setCursor(Qt::SizeHorCursor);
    } else if (navigationPanActive()) {
        setCursor(Qt::OpenHandCursor);
    } else {
        setCursor(Qt::CrossCursor);
    }
}

void CanvasWidget::wheelEvent(QWheelEvent* event)
{
    const auto anchorDocument = mapToDocument(event->position());
    if (!anchorDocument) {
        QWidget::wheelEvent(event);
        return;
    }

    applyZoom(event->angleDelta().y() > 0 ? 1.1 : 1.0 / 1.1, event->position());
    event->accept();
}

void CanvasWidget::applyZoom(qreal factor, const QPointF& anchor)
{
    const auto anchorDocument = mapToDocument(anchor);
    if (!anchorDocument) return;
    const qreal nextZoom = std::clamp(m_zoom * factor, 0.1, 8.0);
    if (qFuzzyCompare(nextZoom, m_zoom)) return;
    m_zoom = nextZoom;
    m_pan += anchor - documentTransform().map(*anchorDocument);
    update();
    emit zoomChanged(zoomPercent());
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
    transform.rotate(m_rotationDegrees);
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

Brush::BrushSample CanvasWidget::applyBrushPressure(const Brush::BrushSample& sample) const
{
    if (m_activeTool == Tools::Tool::Eraser) return sample;

    Brush::BrushSample adjustedSample = sample;
    adjustedSample.pressure = Brush::applyPressureResponse(
        sample.pressure,
        m_brush.settings().pressureSensitivity);
    return adjustedSample;
}

void CanvasWidget::beginStroke(const Brush::BrushSample& sample)
{
    const Brush::BrushSample adjustedSample = applyBrushPressure(sample);
    m_drawing = true;
    m_stroke.beginStroke(adjustedSample);
    m_brush.beginStroke(adjustedSample);

    QPainter painter(&m_document.image());
    Brush::BrushSample dotSample = adjustedSample;
    dotSample.position += QPointF(0.01, 0.01);
    m_brush.continueStroke(painter, dotSample);
    update();
}

void CanvasWidget::continueStroke(const Brush::BrushSample& sample)
{
    if (!m_drawing) return;

    const Brush::BrushSample adjustedSample = applyBrushPressure(sample);
    QPainter painter(&m_document.image());
    const auto samples = m_stroke.processSample(adjustedSample, m_brush.settings().size);
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
