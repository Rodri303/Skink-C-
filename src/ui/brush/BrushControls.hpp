#pragma once

#include <QWidget>

class QSlider;

namespace Skink::Ui::Brush {

class BrushControls final : public QWidget {
    Q_OBJECT

public:
    explicit BrushControls(QWidget* parent = nullptr);

public slots:
    void setBrushSize(int value);
    void setOpacity(int value);
    void setPressureSensitivity(int value);

signals:
    void brushSizeChanged(int value);
    void opacityChanged(int value);
    void pressureSensitivityChanged(int value);

private:
    QSlider* m_sizeSlider{nullptr};
    QSlider* m_opacitySlider{nullptr};
    QSlider* m_pressureSlider{nullptr};
};

} // namespace Skink::Ui::Brush
