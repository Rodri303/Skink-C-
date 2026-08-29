#pragma once

#include <QMainWindow>

namespace Skink::Core::Canvas { class CanvasWidget; }
namespace Skink::Ui::Panels { class DraggablePanel; }

namespace Skink::App {

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void buildInterface();
    void buildPanels(QWidget* workspace);
    void applyStyle();

    Core::Canvas::CanvasWidget* m_canvas{nullptr};
};

} // namespace Skink::App
