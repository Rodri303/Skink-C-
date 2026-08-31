#pragma once

#include "core/brush/BasicBrushEngine.hpp"
#include "core/document/DrawingDocument.hpp"
#include "core/history/HistoryManager.hpp"
#include "core/stroke/StrokeEngine.hpp"

#include <QPointF>
#include <QTransform>
#include <QWidget>

#include <optional>

namespace Skink::Core::Canvas {

class CanvasWidget final : public QWidget {
    Q_OBJECT

public:
    explicit CanvasWidget(QWidget* parent = nullptr);

    void setBrushSize(qreal size);
    void setBrushColor(const QColor& color);
    void clearCanvas();
    void undo();
    void redo();
    void resetView();
    void zoomIn();
    void zoomOut();
    [[nodiscard]] int zoomPercent() const;

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void tabletEvent(QTabletEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    [[nodiscard]] QTransform documentTransform() const;
    [[nodiscard]] std::optional<QPointF> mapToDocument(const QPointF& viewportPosition) const;
    [[nodiscard]] bool isInsideDocument(const QPointF& documentPosition) const;
    [[nodiscard]] Brush::BrushSample tabletSample(const QTabletEvent& event, const QPointF& documentPosition) const;

    void beginStroke(const Brush::BrushSample& sample);
    void continueStroke(const Brush::BrushSample& sample);
    void endStroke();

    Document::DrawingDocument m_document;
    History::HistoryManager m_history{24};
    Stroke::StrokeEngine m_stroke;
    Brush::BasicBrushEngine m_brush;

    bool m_drawing{false};
    bool m_panning{false};
    QPointF m_pan;
    QPointF m_lastPanPosition;
    qreal m_zoom{1.0};
};

} // namespace Skink::Core::Canvas
