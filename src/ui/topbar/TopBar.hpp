#pragma once

#include "core/tools/ToolController.hpp"

#include <QWidget>

class QColor;
class QFrame;
class QPushButton;

namespace Skink::Ui::TopBar {

class TopBar final : public QWidget {
    Q_OBJECT

public:
    explicit TopBar(QWidget* parent = nullptr);
    void setActiveColor(const QColor& color);
    void setActiveTool(Core::Tools::Tool tool);

signals:
    void undoRequested();
    void redoRequested();
    void brushRequested();
    void eraserRequested();
    void quickBrushPanelRequested();
    void layersPanelRequested();
    void colorPickerRequested();

private:
    QFrame* m_colorIndicator{nullptr};
    QPushButton* m_brushButton{nullptr};
    QPushButton* m_eraserButton{nullptr};
};

} // namespace Skink::Ui::TopBar
