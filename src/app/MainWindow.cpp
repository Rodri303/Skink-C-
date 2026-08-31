#include "app/MainWindow.hpp"

#include "core/canvas/CanvasWidget.hpp"
#include "core/tools/ToolController.hpp"
#include "ui/input/ShortcutRouter.hpp"
#include "ui/workspace/WorkspaceWidget.hpp"
#include "ui/workspace/WorkspacePersistence.hpp"
#include "ui/topbar/TopBar.hpp"
#include "ui/toolrail/ToolRail.hpp"
#include "ui/brush/BrushControls.hpp"
#include "ui/brush/QuickBrushPanel.hpp"
#include "ui/bottom/BottomDock.hpp"
#include "ui/color/ColorPanel.hpp"
#include "ui/color/ColorPicker.hpp"
#include "ui/layers/LayersPanel.hpp"
#include "ui/docking/SkinkDockPanel.hpp"

#include <QCloseEvent>
#include <QShowEvent>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>

namespace Skink::App {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Skink — SW!NK Native C++");
    resize(1440, 900);
    setMinimumSize(1100, 700);

    buildInterface();
    applyStyle();
    m_workspaceStateRestored = Ui::Workspace::WorkspacePersistence::restore(*this);
}

void MainWindow::buildInterface()
{
    auto* root = new QWidget(this);
    root->setObjectName("appRoot");
    auto* layout = new QVBoxLayout(root);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_toolController = new Core::Tools::ToolController(this);
    m_topBar = new Ui::TopBar::TopBar(root);
    m_colorPicker = new Ui::Color::ColorPicker(this);
    m_topBar->setActiveColor(m_colorPicker->currentColor());
    layout->addWidget(m_topBar);

    connect(m_topBar, &Ui::TopBar::TopBar::undoRequested, this, [this] { if (m_canvas) m_canvas->undo(); });
    connect(m_topBar, &Ui::TopBar::TopBar::redoRequested, this, [this] { if (m_canvas) m_canvas->redo(); });
    connect(m_topBar, &Ui::TopBar::TopBar::brushRequested, this, [this] {
        m_toolController->setActiveTool(Core::Tools::Tool::Brush);
    });
    connect(m_topBar, &Ui::TopBar::TopBar::eraserRequested, this, [this] {
        m_toolController->setActiveTool(Core::Tools::Tool::Eraser);
    });
    connect(m_topBar, &Ui::TopBar::TopBar::quickBrushPanelRequested, this, [this] {
        showDockPanel(m_quickBrushDock);
    });
    connect(m_topBar, &Ui::TopBar::TopBar::layersPanelRequested, this, [this] {
        if (!m_layersDock) return;

        if (m_layersDock->isVisible()) {
            m_layersDock->hide();
        } else {
            showDockPanel(m_layersDock);
        }
    });
    connect(m_topBar, &Ui::TopBar::TopBar::colorPickerRequested, this, [this] {
        if (!m_colorDock) return;
        if (!m_initialColorDockPositionApplied) {
            // Provisional placement: SWINK had no custom color panel geometry.
            const QPoint localPosition(width() - m_colorDock->width() - 370, 70);
            m_colorDock->move(mapToGlobal(localPosition));
            m_initialColorDockPositionApplied = true;
        }
        m_colorDock->show();
        m_colorDock->raise();
        if (m_colorDock->isFloating()) m_colorDock->activateWindow();
    });
    connect(m_colorPicker, &Ui::Color::ColorPicker::colorSelected, this, [this](const QColor& color) {
        if (m_canvas) m_canvas->setBrushColor(color);
        if (m_colorPanel) m_colorPanel->setColor(color);
        m_topBar->setActiveColor(color);
    });
    connect(m_toolController, &Core::Tools::ToolController::activeToolChanged,
            m_topBar, &Ui::TopBar::TopBar::setActiveTool);
    m_topBar->setActiveTool(m_toolController->activeTool());

    m_workspace = new Ui::Workspace::WorkspaceWidget(root);
    layout->addWidget(m_workspace, 1);

    m_canvas = new Core::Canvas::CanvasWidget(m_workspace);
    m_canvas->setObjectName("canvasShell");
    m_workspace->setCanvas(m_canvas);
    m_canvas->show();
    connect(m_toolController, &Core::Tools::ToolController::activeToolChanged,
            m_canvas, &Core::Canvas::CanvasWidget::setActiveTool);
    m_canvas->setActiveTool(m_toolController->activeTool());

    buildWorkspaceOverlays(m_workspace);

    m_bottomDock = new Ui::Bottom::BottomDock(root);
    m_bottomDock->setZoomPercent(m_canvas->zoomPercent());
    connect(m_bottomDock, &Ui::Bottom::BottomDock::zoomInRequested, m_canvas, &Core::Canvas::CanvasWidget::zoomIn);
    connect(m_bottomDock, &Ui::Bottom::BottomDock::zoomOutRequested, m_canvas, &Core::Canvas::CanvasWidget::zoomOut);
    connect(m_bottomDock, &Ui::Bottom::BottomDock::resetViewRequested, m_canvas, &Core::Canvas::CanvasWidget::resetView);
    connect(m_canvas, &Core::Canvas::CanvasWidget::zoomChanged, m_bottomDock, &Ui::Bottom::BottomDock::setZoomPercent);
    connect(m_canvas, &Core::Canvas::CanvasWidget::zoomChanged, m_workspace, &Ui::Workspace::WorkspaceWidget::showZoomHud);
    layout->addWidget(m_bottomDock);

    setCentralWidget(root);

    m_colorPanel = new Ui::Color::ColorPanel(this);
    m_colorPanel->setColor(m_colorPicker->currentColor());
    m_colorDock = new Ui::Docking::SkinkDockPanel("COLOR", m_colorPanel, this);
    m_colorDock->setObjectName("ColorDock");
    addDockWidget(Qt::RightDockWidgetArea, m_colorDock);
    m_colorDock->setFloating(true);
    m_colorDock->resize(300, 450);
    m_colorDock->hide();
    connect(m_colorPanel, &Ui::Color::ColorPanel::colorChanged,
            m_colorPicker, &Ui::Color::ColorPicker::selectColor);

    m_shortcutRouter = new Ui::Input::ShortcutRouter(this);
    connect(m_shortcutRouter, &Ui::Input::ShortcutRouter::brushRequested, this, [this] {
        m_toolController->setActiveTool(Core::Tools::Tool::Brush);
    });
    connect(m_shortcutRouter, &Ui::Input::ShortcutRouter::eraserRequested, this, [this] {
        m_toolController->setActiveTool(Core::Tools::Tool::Eraser);
    });
    connect(m_shortcutRouter, &Ui::Input::ShortcutRouter::resetViewRequested,
            m_canvas, &Core::Canvas::CanvasWidget::resetView);
    connect(m_shortcutRouter, &Ui::Input::ShortcutRouter::temporaryPanChanged,
            m_canvas, &Core::Canvas::CanvasWidget::setTemporaryPan);
    connect(m_shortcutRouter, &Ui::Input::ShortcutRouter::undoRequested,
            m_canvas, &Core::Canvas::CanvasWidget::undo);
    connect(m_shortcutRouter, &Ui::Input::ShortcutRouter::redoRequested,
            m_canvas, &Core::Canvas::CanvasWidget::redo);
    connect(m_shortcutRouter, &Ui::Input::ShortcutRouter::brushSizeStepRequested,
            this, [this](int delta) {
                const int nextSize = std::clamp(m_leftControls->brushSize() + delta, 1, 160);
                m_leftControls->setBrushSize(nextSize);
            });
}

void MainWindow::showDockPanel(Ui::Docking::SkinkDockPanel* dock)
{
    if (!dock) return;

    dock->show();
    dock->raise();
    if (dock->isFloating()) dock->activateWindow();
}

void MainWindow::buildWorkspaceOverlays(QWidget* parent)
{
    m_toolStrip = new Ui::ToolRail::ToolRail(parent);
    connect(m_toolStrip, &Ui::ToolRail::ToolRail::brushRequested, this, [this] {
        m_toolController->setActiveTool(Core::Tools::Tool::Brush);
    });
    connect(m_toolStrip, &Ui::ToolRail::ToolRail::selectionRequested, this, [this] {
        m_toolController->setActiveTool(Core::Tools::Tool::Selection);
    });
    connect(m_toolStrip, &Ui::ToolRail::ToolRail::panRequested, this, [this] {
        m_toolController->setActiveTool(Core::Tools::Tool::Pan);
    });
    connect(m_toolStrip, &Ui::ToolRail::ToolRail::transformRequested, this, [this] {
        m_toolController->setActiveTool(Core::Tools::Tool::Transform);
    });
    connect(m_toolController, &Core::Tools::ToolController::activeToolChanged,
            m_toolStrip, &Ui::ToolRail::ToolRail::setActiveTool);
    m_toolStrip->setActiveTool(m_toolController->activeTool());

    m_leftControls = new Ui::Brush::BrushControls(parent);
    connect(m_leftControls, &Ui::Brush::BrushControls::brushSizeChanged, this, [this](int value) {
        if (m_canvas) m_canvas->setBrushSize(value);
        if (m_workspace) m_workspace->showBrushSizeHud(value);
    });

    auto* layersPanel = new Ui::Layers::LayersPanel(this);
    auto* quickBrushPanel = new Ui::Brush::QuickBrushPanel(this);
    connect(quickBrushPanel, &Ui::Brush::QuickBrushPanel::presetSelected, this, [this](const QString&) {
        m_toolController->setActiveTool(Core::Tools::Tool::Brush);
    });
    m_layersDock = new Ui::Docking::SkinkDockPanel("CAPAS", layersPanel, this);
    m_quickBrushDock = new Ui::Docking::SkinkDockPanel("PINCEL RAPIDO", quickBrushPanel, this);
    m_layersDock->setObjectName("LayersDock");
    m_quickBrushDock->setObjectName("QuickBrushDock");
    addDockWidget(Qt::RightDockWidgetArea, m_layersDock);
    addDockWidget(Qt::RightDockWidgetArea, m_quickBrushDock);
    m_layersDock->setFloating(true);
    m_quickBrushDock->setFloating(true);
    m_workspace->setLeftOverlays(m_toolStrip, m_leftControls);
    return;

}

void MainWindow::closeEvent(QCloseEvent* event)
{
    Ui::Workspace::WorkspacePersistence::save(*this);
    QMainWindow::closeEvent(event);
}

void MainWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);
    if (m_initialDockPositionsApplied) return;

    m_initialDockPositionsApplied = true;
    if (m_workspaceStateRestored) {
        m_initialColorDockPositionApplied = true;
        Ui::Workspace::WorkspacePersistence::ensureVisible(*this);
        return;
    }

    const auto placeDock = [this](Ui::Docking::SkinkDockPanel* dock, int top) {
        const QPoint localPosition(width() - dock->width() - 20, top);
        dock->move(mapToGlobal(localPosition));
    };
    placeDock(m_layersDock, 30);
    placeDock(m_quickBrushDock, 485);
}

void MainWindow::applyStyle()
{
    setStyleSheet(R"(
        * {
            font-family: "Segoe UI";
        }

        QMainWindow, #appRoot, #workspace {
            background: #252629;
            color: #f3f3f3;
        }

        #topBar, #bottomBar {
            background: #0f1012;
        }

        #topBar {
            border-bottom: 1px solid #303135;
        }

        #bottomBar {
            border-top: 1px solid #2b2c30;
        }

        #wordmark {
            color: #f3f3f3;
            font-size: 27px;
            font-weight: 800;
            font-style: italic;
        }

        QPushButton {
            outline: none;
        }

        #topTextButton {
            min-width: 58px;
            height: 40px;
            border: 0;
            background: transparent;
            color: #b9bbc0;
            font-size: 15px;
        }

        #topTextButton:hover {
            color: white;
        }

        #topDivider {
            background: #3b3c40;
        }

        #topIconButton {
            width: 40px;
            height: 40px;
            border: 0;
            background: transparent;
            color: #aaaaaa;
            font-size: 27px;
        }

        #topIconButton:hover {
            color: white;
        }

        #appTitle {
            color: #999ba1;
            font-size: 12px;
        }

        #penStatus {
            color: #dce9dc;
            background: rgba(72,160,91,0.12);
            border: 1px solid rgba(93,190,112,0.35);
            border-radius: 11px;
            padding: 3px 7px;
            font-size: 10px;
        }

        #paintTool, #colorTool {
            border: 0;
            border-radius: 13px;
            background: transparent;
            color: #d8d8da;
            font-size: 24px;
        }

        #paintTool:hover, #paintTool[activeTool="true"], #colorTool:hover {
            background: #1d1f22;
        }

        #paintTool[activeTool="true"] {
            border-bottom: 2px solid #1688ff;
        }

        #colorTool {
            padding: 0;
        }

        #canvasShell {
            background: #252629;
        }

        #toolStrip, #sliderColumn, #floatingPanel {
            background: #0e0f11;
            border: 1px solid rgba(255,255,255,0.05);
        }

        #toolStrip, #sliderColumn {
            border-radius: 13px;
        }

        #floatingPanel {
            border-radius: 15px;
        }

        #stripButton {
            border: 0;
            border-radius: 8px;
            background: transparent;
            color: #d1d1d3;
            font-size: 18px;
        }

        #stripButton:hover {
            background: #1d1f22;
        }

        #stripButton[activeTool="true"] {
            color: #1688ff;
        }

        #sliderColumn QSlider::groove:vertical {
            width: 3px;
            background: #3a3b3f;
            border-radius: 1px;
        }

        #sliderColumn QSlider::sub-page:vertical {
            background: #d7d7d8;
        }

        #sliderColumn QSlider::handle:vertical {
            width: 14px;
            height: 14px;
            margin: 0 -6px;
            border-radius: 7px;
            background: #eeeeee;
        }

        #controlTitle {
            color: #eeeeef;
            font-size: 11px;
            font-weight: 700;
        }

        #controlSubtitle {
            color: #babcc1;
            font-size: 11px;
        }

        #controlOutput {
            color: white;
            font-size: 11px;
        }

        #panelHeader {
            background: #0e0f11;
            border-bottom: 1px solid #27282b;
        }

        #panelTitle {
            color: #eeeeef;
            font-size: 12px;
            font-weight: 700;
        }

        #panelHeaderButton, #layerIconButton {
            border: 0;
            background: transparent;
            color: #aaaaaa;
            font-size: 18px;
        }

        #layerRow {
            background: #0e0f11;
            border-bottom: 1px solid #252629;
        }

        #layerRow:checked {
            background: #14304f;
        }

        #layerThumb, #layerThumbDark {
            border-radius: 5px;
            background: #e8e0d4;
        }

        #layerThumbDark {
            background: #111214;
            border: 1px solid #44464a;
        }

        #layerName {
            color: #f0f0f0;
            font-size: 13px;
        }

        #layerMeta {
            color: #a8abb0;
            font-size: 10px;
        }

        #quickCard, #quickCardSelected {
            border: 1px solid #2a2b2f;
            border-radius: 11px;
            background: #17181a;
            color: #bdbfc4;
            text-align: left;
            padding: 9px;
            font-size: 10px;
        }

        #quickCard:hover {
            background: #1d1e21;
        }

        #quickCardSelected, #quickCard:checked {
            border: 1px solid #1d8dff;
            color: white;
        }

        #recordDot {
            background: #de4242;
            border-radius: 7px;
        }

        #bottomStrong {
            color: #f0f0f0;
            font-size: 11px;
            font-weight: 700;
        }

        #bottomText {
            color: #d0d1d4;
            font-size: 11px;
        }

        #processCaption {
            color: #999999;
            font-size: 8px;
        }

        #processSelect {
            border: 0;
            background: transparent;
            color: white;
            font-size: 11px;
        }

        #processSelect QAbstractItemView {
            background: #17181a;
            color: white;
            selection-background-color: #2487ff;
        }

        #bottomButton, #squareBottomButton {
            height: 38px;
            border: 0;
            border-radius: 9px;
            background: #1b1c1f;
            color: #dddddf;
            padding: 0 15px;
            font-size: 10px;
        }

        #bottomButton:hover, #squareBottomButton:hover {
            background: #25272b;
        }

        #squareBottomButton {
            min-width: 42px;
            max-width: 42px;
        }

        #zoomReadout {
            min-height: 38px;
            max-height: 38px;
            border-radius: 9px;
            background: #17181a;
        }

        #zoomButton {
            border: 0;
            background: transparent;
            color: #aaaaaa;
            font-size: 16px;
        }
    )");
}

} // namespace Skink::App
