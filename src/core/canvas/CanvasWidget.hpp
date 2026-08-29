#pragma once

#include "core/brush/BasicBrushEngine.hpp"

#include <QImage>
#include <QWidget>

namespace Skink::Core::Canvas {

class CanvasWidget final : public QWidget {
    Q_OBJECT

public:
    explicit CanvasWidget(QWidget* parent = nullptr);

    void setBrushSize(qreal size);
    void setBrushColor(const QColor& color);
    void clearCanvas();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void tabletEvent(QTabletEvent* event) override;

private:
    void ensureImageSize();
    void beginStroke(const QPointF& position, qreal pressure);
    void continueStroke(const QPointF& position, qreal pressure);
    void endStroke();

    QImage m_image;
    Brush::BasicBrushEngine m_brush;
    bool m_drawing{false};
};

} // namespace Skink::Core::Canvas
