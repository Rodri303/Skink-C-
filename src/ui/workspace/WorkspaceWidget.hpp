#pragma once

#include <QWidget>

namespace Skink::Core::Canvas { class CanvasWidget; }
namespace Skink::Ui::Hud { class CanvasHud; }

namespace Skink::Ui::Workspace {

class WorkspaceWidget final : public QWidget {
    Q_OBJECT

public:
    explicit WorkspaceWidget(QWidget* parent = nullptr);

    void setCanvas(Core::Canvas::CanvasWidget* canvas);
    void setLeftOverlays(QWidget* toolRail, QWidget* brushControls);
    void showZoomHud(int percent);
    void showBrushSizeHud(int size);
    void showRotationHud(int degrees);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    void updateOverlayGeometry();

    Core::Canvas::CanvasWidget* m_canvas{nullptr};
    QWidget* m_toolRail{nullptr};
    QWidget* m_brushControls{nullptr};
    Hud::CanvasHud* m_canvasHud{nullptr};
};

} // namespace Skink::Ui::Workspace
