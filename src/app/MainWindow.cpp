#include "app/MainWindow.hpp"

#include "core/canvas/CanvasWidget.hpp"
#include "ui/workspace/WorkspaceWidget.hpp"
#include "ui/topbar/TopBar.hpp"
#include "ui/toolrail/ToolRail.hpp"
#include "ui/brush/BrushControls.hpp"
#include "ui/brush/QuickBrushPanel.hpp"
#include "ui/layers/LayersPanel.hpp"
#include "ui/docking/SkinkDockPanel.hpp"

#include <QComboBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QShowEvent>
#include <QVBoxLayout>
#include <QWidget>

namespace Skink::App {

namespace {

QPushButton* makeButton(QWidget* parent, const QString& text, const char* objectName)
{
    auto* button = new QPushButton(text, parent);
    button->setObjectName(objectName);
    button->setCursor(Qt::PointingHandCursor);
    return button;
}

QLabel* makeLabel(QWidget* parent, const QString& text, const char* objectName = nullptr)
{
    auto* label = new QLabel(text, parent);
    if (objectName) label->setObjectName(objectName);
    return label;
}

QWidget* makeControlBlock(
    QWidget* parent,
    const QString& title,
    const QString& subtitle,
    const QString& initialOutput,
    QSlider** sliderOut,
    int minimum,
    int maximum,
    int value)
{
    auto* block = new QWidget(parent);
    block->setObjectName("controlBlock");
    auto* row = new QHBoxLayout(block);
    row->setContentsMargins(0, 0, 0, 0);
    row->setSpacing(16);

    auto* sliderFrame = new QFrame(block);
    sliderFrame->setObjectName("sliderColumn");
    sliderFrame->setFixedSize(50, 120);
    auto* sliderLayout = new QVBoxLayout(sliderFrame);
    sliderLayout->setContentsMargins(14, 10, 14, 10);

    auto* slider = new QSlider(Qt::Vertical, sliderFrame);
    slider->setRange(minimum, maximum);
    slider->setValue(value);
    sliderLayout->addWidget(slider, 1, Qt::AlignCenter);

    auto* copy = new QWidget(block);
    copy->setObjectName("controlCopy");
    auto* copyLayout = new QVBoxLayout(copy);
    copyLayout->setContentsMargins(0, 12, 0, 0);
    copyLayout->setSpacing(4);

    auto* titleLabel = makeLabel(copy, title, "controlTitle");
    auto* subLabel = makeLabel(copy, subtitle, "controlSubtitle");
    subLabel->setWordWrap(true);
    auto* output = makeLabel(copy, initialOutput, "controlOutput");

    copyLayout->addWidget(titleLabel);
    copyLayout->addWidget(subLabel);
    copyLayout->addWidget(output);
    copyLayout->addStretch(1);

    row->addWidget(sliderFrame);
    row->addWidget(copy, 1);

    if (sliderOut) *sliderOut = slider;
    return block;
}

} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Skink — SW!NK Native C++");
    resize(1440, 900);
    setMinimumSize(1100, 700);

    buildInterface();
    applyStyle();
}

void MainWindow::buildInterface()
{
    auto* root = new QWidget(this);
    root->setObjectName("appRoot");
    auto* layout = new QVBoxLayout(root);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto* topBar = new Ui::TopBar::TopBar(root);
    layout->addWidget(topBar);

    connect(topBar, &Ui::TopBar::TopBar::undoRequested, this, [this] { if (m_canvas) m_canvas->undo(); });
    connect(topBar, &Ui::TopBar::TopBar::redoRequested, this, [this] { if (m_canvas) m_canvas->redo(); });
    connect(topBar, &Ui::TopBar::TopBar::colorSelected, this, [this](const QColor& color) { if (m_canvas) m_canvas->setBrushColor(color); });

    m_workspace = new Ui::Workspace::WorkspaceWidget(root);
    layout->addWidget(m_workspace, 1);

    m_canvas = new Core::Canvas::CanvasWidget(m_workspace);
    m_canvas->setObjectName("canvasShell");
    m_workspace->setCanvas(m_canvas);
    m_canvas->show();

    buildWorkspaceOverlays(m_workspace);

    auto* bottomBar = new QWidget(root);
    bottomBar->setObjectName("bottomBar");
    bottomBar->setFixedHeight(72);
    buildBottomBar(bottomBar);
    layout->addWidget(bottomBar);

    setCentralWidget(root);
}

void MainWindow::buildWorkspaceOverlays(QWidget* parent)
{
    m_toolStrip = new Ui::ToolRail::ToolRail(parent);

    m_leftControls = new Ui::Brush::BrushControls(parent);
    connect(m_leftControls, &Ui::Brush::BrushControls::brushSizeChanged, this, [this](int value) {
        if (m_canvas) m_canvas->setBrushSize(value);
    });

    auto* layersPanel = new Ui::Layers::LayersPanel(this);
    auto* quickBrushPanel = new Ui::Brush::QuickBrushPanel(this);
    m_layersDock = new Ui::Docking::SkinkDockPanel("CAPAS", layersPanel, this);
    m_quickBrushDock = new Ui::Docking::SkinkDockPanel("PINCEL RAPIDO", quickBrushPanel, this);
    addDockWidget(Qt::RightDockWidgetArea, m_layersDock);
    addDockWidget(Qt::RightDockWidgetArea, m_quickBrushDock);
    m_layersDock->setFloating(true);
    m_quickBrushDock->setFloating(true);
    m_workspace->setLeftOverlays(m_toolStrip, m_leftControls);
    return;

}

void MainWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);
    if (m_initialDockPositionsApplied) return;

    m_initialDockPositionsApplied = true;
    const auto placeDock = [this](Ui::Docking::SkinkDockPanel* dock, int top) {
        const QPoint localPosition(width() - dock->width() - 20, top);
        dock->move(mapToGlobal(localPosition));
    };
    placeDock(m_layersDock, 30);
    placeDock(m_quickBrushDock, 485);
}

void MainWindow::buildBottomBar(QWidget* parent)
{
    auto* layout = new QHBoxLayout(parent);
    layout->setContentsMargins(20, 0, 20, 0);
    layout->setSpacing(14);

    auto* recordDot = new QLabel(parent);
    recordDot->setObjectName("recordDot");
    recordDot->setFixedSize(14, 14);
    layout->addWidget(recordDot);
    layout->addWidget(makeLabel(parent, "GRABANDO", "bottomStrong"));
    layout->addWidget(makeLabel(parent, "00:00:00", "bottomText"));
    layout->addWidget(makeLabel(parent, QStringLiteral("⌄"), "bottomText"));

    layout->addStretch(1);

    auto* processWrap = new QWidget(parent);
    auto* processLayout = new QHBoxLayout(processWrap);
    processLayout->setContentsMargins(0, 0, 0, 0);
    processLayout->setSpacing(10);

    auto* durationWrap = new QWidget(processWrap);
    auto* durationLayout = new QVBoxLayout(durationWrap);
    durationLayout->setContentsMargins(0, 0, 0, 0);
    durationLayout->setSpacing(1);
    durationLayout->addWidget(makeLabel(durationWrap, "PROCESO", "processCaption"));
    auto* duration = new QComboBox(durationWrap);
    duration->setObjectName("processSelect");
    duration->addItems({"30 seg", "1 min", "3 min", "5 min"});
    duration->setCurrentText("3 min");
    durationLayout->addWidget(duration);

    processLayout->addWidget(durationWrap);
    processLayout->addWidget(makeButton(processWrap, QStringLiteral("▶  VISTA PREVIA"), "bottomButton"));
    processLayout->addWidget(makeButton(processWrap, QStringLiteral("■  DETENER"), "bottomButton"));
    processLayout->addWidget(makeButton(processWrap, "GUARDAR PNG", "bottomButton"));
    layout->addWidget(processWrap);

    layout->addStretch(1);

    auto* zoom = new QWidget(parent);
    zoom->setObjectName("zoomReadout");
    auto* zoomLayout = new QHBoxLayout(zoom);
    zoomLayout->setContentsMargins(8, 0, 8, 0);
    zoomLayout->setSpacing(8);
    zoomLayout->addWidget(makeButton(zoom, QStringLiteral("−"), "zoomButton"));
    zoomLayout->addWidget(makeLabel(zoom, "100%", "bottomText"));
    zoomLayout->addWidget(makeButton(zoom, QStringLiteral("＋"), "zoomButton"));
    layout->addWidget(zoom);

    auto* center = makeButton(parent, "C   CENTRAR", "bottomButton");
    layout->addWidget(center);
    layout->addWidget(makeButton(parent, QStringLiteral("⌄"), "squareBottomButton"));

    connect(center, &QPushButton::clicked, this, [this] { if (m_canvas) m_canvas->resetView(); });
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

        #paintTool, #paintToolActive, #colorTool {
            border: 0;
            border-radius: 13px;
            background: transparent;
            color: #d8d8da;
            font-size: 24px;
        }

        #paintTool:hover, #paintToolActive, #colorTool:hover {
            background: #1d1f22;
        }

        #paintToolActive {
            border-bottom: 2px solid #1688ff;
        }

        #colorTool {
            color: #151515;
            font-size: 31px;
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

        #stripButton, #stripButtonActive {
            border: 0;
            border-radius: 8px;
            background: transparent;
            color: #d1d1d3;
            font-size: 18px;
        }

        #stripButton:hover {
            background: #1d1f22;
        }

        #stripButtonActive {
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
