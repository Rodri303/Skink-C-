#pragma once

#include <QImage>
#include <QPointF>
#include <QRectF>
#include <QWidget>

namespace Skink::Ui::Color {

class ColorWheel final : public QWidget {
    Q_OBJECT

public:
    explicit ColorWheel(QWidget* parent = nullptr);

    [[nodiscard]] int hue() const;
    void setHue(int hue);

signals:
    void hueChanged(int hue);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    [[nodiscard]] QRectF wheelRect() const;
    [[nodiscard]] bool isOnWheel(const QPointF& position) const;
    void rebuildWheelImage();
    void updateHueFromPosition(const QPointF& position);

    QImage m_wheelImage;
    int m_hue{0};
    bool m_dragging{false};
};

} // namespace Skink::Ui::Color
