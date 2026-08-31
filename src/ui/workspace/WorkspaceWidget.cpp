#include "ui/workspace/WorkspaceWidget.hpp"

#include "core/canvas/CanvasWidget.hpp"

#include <QResizeEvent>
#include <QShowEvent>

#include <algorithm>

namespace Skink::Ui::Workspace {

namespace {
constexpr int kOverlayMargin = 18;
constexpr int kToolRailTop = 52;
constexpr int kControlsTop = 232;
}

WorkspaceWidget::WorkspaceWidget(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("workspace");
    setMinimumHeight(500);
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

    const auto rightAlignedX = [this](const QWidget* overlay) {
        return std::max(kOverlayMargin, width() - overlay->width() - kOverlayMargin);
    };

    if (m_canvas) m_canvas->lower();
    for (QWidget* overlay : {m_toolRail, m_brushControls}) {
        if (overlay) overlay->raise();
    }
}

} // namespace Skink::Ui::Workspace
