#pragma once

#include <QMainWindow>

class QCloseEvent;
class QWidget;

namespace Skink::Core::Canvas { class CanvasWidget; }
namespace Skink::Core::Brush { class BrushController; }
namespace Skink::Core::Tools { class ToolController; }
namespace Skink::Ui::Workspace { class WorkspaceWidget; }
namespace Skink::Ui::Input { class ShortcutRouter; }
namespace Skink::Ui::TopBar { class TopBar; }
namespace Skink::Ui::ToolRail { class ToolRail; }
namespace Skink::Ui::Brush { class BrushControls; class QuickBrushPanel; }
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
    void showDockPanel(Ui::Docking::SkinkDockPanel* dock);
    void applyStyle();

    Core::Canvas::CanvasWidget* m_canvas{nullptr};
    Core::Brush::BrushController* m_brushController{nullptr};
    Core::Tools::ToolController* m_toolController{nullptr};
    Ui::Workspace::WorkspaceWidget* m_workspace{nullptr};
    Ui::Input::ShortcutRouter* m_shortcutRouter{nullptr};
    Ui::TopBar::TopBar* m_topBar{nullptr};
    Ui::ToolRail::ToolRail* m_toolStrip{nullptr};
    Ui::Brush::BrushControls* m_leftControls{nullptr};
    Ui::Brush::QuickBrushPanel* m_quickBrushPanel{nullptr};
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
