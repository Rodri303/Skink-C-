#pragma once

#include <QMainWindow>

class QWidget;

namespace Skink::Core::Canvas { class CanvasWidget; }
namespace Skink::Ui::Workspace { class WorkspaceWidget; }
namespace Skink::Ui::ToolRail { class ToolRail; }
namespace Skink::Ui::Brush { class BrushControls; }

namespace Skink::App {

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void buildInterface();
    void buildWorkspaceOverlays(QWidget* parent);
    void buildBottomBar(QWidget* parent);
    void applyStyle();

    Core::Canvas::CanvasWidget* m_canvas{nullptr};
    Ui::Workspace::WorkspaceWidget* m_workspace{nullptr};
    Ui::ToolRail::ToolRail* m_toolStrip{nullptr};
    Ui::Brush::BrushControls* m_leftControls{nullptr};
    QWidget* m_layersPanel{nullptr};
    QWidget* m_quickBrushPanel{nullptr};
};

} // namespace Skink::App
