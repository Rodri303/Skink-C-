#include "app/MainWindow.hpp"

#include "core/canvas/CanvasWidget.hpp"
#include "ui/panels/DraggablePanel.hpp"
#include "ui/widgets/ToolButton.hpp"

#include <QColorDialog>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>

namespace Skink::App {

using Skink::Ui::Panels::DraggablePanel;
using Skink::Ui::Widgets::ToolButton;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Skink — C++ Foundation 0.1");
    resize(1440, 900);
    setMinimumSize(900, 600);

    buildInterface();
    applyStyle();
}

void MainWindow::buildInterface()
{
    auto* workspace = new QWidget(this);
    workspace->setObjectName("workspace");
    setCentralWidget(workspace);

    m_canvas = new Core::Canvas::CanvasWidget(workspace);
    m_canvas->setGeometry(245, 70, 950, 760);

    buildPanels(workspace);
}

void MainWindow::buildPanels(QWidget* workspace)
{
    auto* tools = new DraggablePanel("TOOLS", workspace);
    tools->setGeometry(18, 70, 200, 250);
    const QStringList toolNames{"Brush", "Eraser", "Select", "Line", "Rect", "Hand", "Zoom"};
    for (const auto& name : toolNames) {
        tools->contentLayout()->addWidget(new ToolButton(name, tools));
    }

    auto* brush = new DraggablePanel("BRUSH", workspace);
    brush->setGeometry(18, 340, 200, 190);
    auto* sizeLabel = new QLabel("Size", brush);
    auto* size = new QSlider(Qt::Horizontal, brush);
    size->setRange(1, 160);
    size->setValue(18);
    brush->contentLayout()->addWidget(sizeLabel);
    brush->contentLayout()->addWidget(size);
    connect(size, &QSlider::valueChanged, m_canvas, [this](int value) {
        m_canvas->setBrushSize(value);
    });

    auto* clear = new QPushButton("Clear canvas", brush);
    brush->contentLayout()->addWidget(clear);
    connect(clear, &QPushButton::clicked, m_canvas, &Core::Canvas::CanvasWidget::clearCanvas);

    auto* layers = new DraggablePanel("LAYERS", workspace);
    layers->setGeometry(1215, 70, 205, 245);
    auto* blend = new QComboBox(layers);
    blend->addItems({"Normal", "Multiply", "Screen", "Overlay"});
    layers->contentLayout()->addWidget(new QLabel("Layer 1", layers));
    layers->contentLayout()->addWidget(blend);
    layers->contentLayout()->addWidget(new QPushButton("+ New layer", layers));
    layers->contentLayout()->addWidget(new QLabel("Layer engine: next phase", layers));

    auto* color = new DraggablePanel("COLOR", workspace);
    color->setGeometry(1215, 335, 205, 155);
    auto* chooseColor = new QPushButton("Choose color", color);
    color->contentLayout()->addWidget(chooseColor);
    connect(chooseColor, &QPushButton::clicked, this, [this] {
        const QColor selected = QColorDialog::getColor(QColor("#111111"), this, "Brush color");
        if (selected.isValid()) {
            m_canvas->setBrushColor(selected);
        }
    });

    auto* inspector = new DraggablePanel("INSPECTOR", workspace);
    inspector->setGeometry(1215, 510, 205, 190);
    inspector->contentLayout()->addWidget(new QLabel("Native C++ / Qt 6", inspector));
    inspector->contentLayout()->addWidget(new QLabel("Canvas: active", inspector));
    inspector->contentLayout()->addWidget(new QLabel("Brush interface: active", inspector));
    inspector->contentLayout()->addWidget(new QLabel("Panels: draggable", inspector));

    tools->show();
    brush->show();
    layers->show();
    color->show();
    inspector->show();
}

void MainWindow::applyStyle()
{
    setStyleSheet(R"(
        QMainWindow, #workspace {
            background: #171717;
            color: #e8e6df;
        }

        #draggablePanel {
            background: #232323;
            border: 1px solid #3b3b3b;
            border-radius: 8px;
        }

        #panelTitle {
            color: #d8d5cc;
            font-size: 11px;
            font-weight: 700;
            letter-spacing: 1px;
            padding: 4px 2px 7px 2px;
            border-bottom: 1px solid #3b3b3b;
        }

        QLabel {
            color: #c8c6c0;
        }

        QPushButton, QComboBox, QSpinBox {
            background: #303030;
            color: #eeeeea;
            border: 1px solid #484848;
            border-radius: 5px;
            padding: 6px 8px;
        }

        QPushButton:hover {
            background: #383838;
        }

        QPushButton:checked {
            background: #7d2424;
            border-color: #a33a3a;
        }

        QSlider::groove:horizontal {
            height: 4px;
            background: #444444;
            border-radius: 2px;
        }

        QSlider::handle:horizontal {
            width: 14px;
            margin: -5px 0;
            background: #b33b3b;
            border-radius: 7px;
        }
    )");
}

} // namespace Skink::App
