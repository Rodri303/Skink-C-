#pragma once

#include <QMainWindow>

class QResizeEvent;
class QWidget;

namespace Skink::Core::Canvas { class CanvasWidget; }

namespace Skink::App {

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void buildInterface();
    void buildTopBar(QWidget* parent);
    void buildWorkspaceOverlays(QWidget* parent);
    void buildBottomBar(QWidget* parent);
    void positionWorkspaceOverlays();
    void applyStyle();

    Core::Canvas::CanvasWidget* m_canvas{nullptr};
    QWidget* m_workspace{nullptr};
    QWidget* m_toolStrip{nullptr};
    QWidget* m_leftControls{nullptr};
    QWidget* m_layersPanel{nullptr};
    QWidget* m_quickBrushPanel{nullptr};
};

} // namespace Skink::App
