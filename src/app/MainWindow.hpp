#pragma once

#include <QMainWindow>

class QCloseEvent;
class QWidget;

namespace Skink::Core::Canvas { class CanvasWidget; }
namespace Skink::Ui::Workspace { class WorkspaceWidget; }
namespace Skink::Ui::ToolRail { class ToolRail; }
namespace Skink::Ui::Brush { class BrushControls; }
namespace Skink::Ui::Bottom { class BottomDock; }
namespace Skink::Ui::Color { class ColorPanel; class ColorPicker; }
namespace Skink::Ui::Docking { class SkinkDockPanel; }

namespace Skink::App {

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    void buildInterface();
    void buildWorkspaceOverlays(QWidget* parent);
    void applyStyle();

    Core::Canvas::CanvasWidget* m_canvas{nullptr};
    Ui::Workspace::WorkspaceWidget* m_workspace{nullptr};
    Ui::ToolRail::ToolRail* m_toolStrip{nullptr};
    Ui::Brush::BrushControls* m_leftControls{nullptr};
    Ui::Bottom::BottomDock* m_bottomDock{nullptr};
    Ui::Color::ColorPanel* m_colorPanel{nullptr};
    Ui::Color::ColorPicker* m_colorPicker{nullptr};
    Ui::Docking::SkinkDockPanel* m_colorDock{nullptr};
    Ui::Docking::SkinkDockPanel* m_layersDock{nullptr};
    Ui::Docking::SkinkDockPanel* m_quickBrushDock{nullptr};
    bool m_initialColorDockPositionApplied{false};
    bool m_initialDockPositionsApplied{false};
    bool m_workspaceStateRestored{false};
};

} // namespace Skink::App
