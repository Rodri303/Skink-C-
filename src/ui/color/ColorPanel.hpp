#pragma once

#include <QColor>
#include <QWidget>

class QFrame;
class QLabel;
class QSlider;

namespace Skink::Ui::Color {

class ColorWheel;

class ColorPanel final : public QWidget {
    Q_OBJECT

public:
    explicit ColorPanel(QWidget* parent = nullptr);

    [[nodiscard]] QColor currentColor() const;
    void setColor(const QColor& color);

signals:
    void colorChanged(const QColor& color);

private:
    void updateColorFromControls();
    void updateVisuals();

    ColorWheel* m_wheel{nullptr};
    QSlider* m_saturation{nullptr};
    QSlider* m_value{nullptr};
    QLabel* m_saturationValue{nullptr};
    QLabel* m_valueValue{nullptr};
    QFrame* m_preview{nullptr};
    QLabel* m_hexValue{nullptr};
    QColor m_currentColor{"#151515"};
};

} // namespace Skink::Ui::Color
