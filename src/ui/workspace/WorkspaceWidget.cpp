#include "ui/workspace/WorkspaceWidget.hpp"

#include "core/canvas/CanvasWidget.hpp"
#include "ui/hud/CanvasHud.hpp"

#include <QResizeEvent>
#include <QShowEvent>

#include <algorithm>

namespace Skink::Ui::Workspace {

namespace {
constexpr int kOverlayMargin = 18;
constexpr int kToolRailTop = 52;
constexpr int kControlsTop = 232;
constexpr int kHudTop = 16;
}

WorkspaceWidget::WorkspaceWidget(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("workspace");
    setMinimumHeight(500);
    m_canvasHud = new Hud::CanvasHud(this);
}

void WorkspaceWidget::setCanvas(Core::Canvas::CanvasWidget* canvas)
{
    m_canvas = canvas;
    updateOverlayGeometry();
}

void WorkspaceWidget::setLeftOverlays(QWidget* toolRail, QWidget* brushControls)
{
    m_toolRail = toolRail;
    m_brushControls = brushControls;
    updateOverlayGeometry();
}

void WorkspaceWidget::showZoomHud(int percent)
{
    m_canvasHud->showMessage(QStringLiteral("%1%").arg(percent));
    updateOverlayGeometry();
}

void WorkspaceWidget::showBrushSizeHud(int size)
{
    m_canvasHud->showMessage(QStringLiteral("TAMAÑO %1").arg(size));
    updateOverlayGeometry();
}

void WorkspaceWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    updateOverlayGeometry();
}

void WorkspaceWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    updateOverlayGeometry();
}

void WorkspaceWidget::updateOverlayGeometry()
{
    if (m_canvas) m_canvas->setGeometry(rect());
    if (m_toolRail) m_toolRail->move(kOverlayMargin, kToolRailTop);
    if (m_brushControls) m_brushControls->move(kOverlayMargin, kControlsTop);
    if (m_canvasHud) {
        const int centeredX = std::max(0, (width() - m_canvasHud->width()) / 2);
        m_canvasHud->move(centeredX, kHudTop);
    }

    if (m_canvas) m_canvas->lower();
    if (m_toolRail) m_toolRail->raise();
    if (m_brushControls) m_brushControls->raise();
    if (m_canvasHud) m_canvasHud->raise();
}

} // namespace Skink::Ui::Workspace
