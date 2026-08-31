#pragma once

#include <QWidget>

class QSlider;

namespace Skink::Ui::Brush {

class BrushControls final : public QWidget {
    Q_OBJECT

public:
    explicit BrushControls(QWidget* parent = nullptr);

    [[nodiscard]] int brushSize() const;

public slots:
    void setBrushSize(int value);

signals:
    void brushSizeChanged(int value);
    void opacityChanged(int value);
    void pressureSensitivityChanged(int value);

private:
    QSlider* m_sizeSlider{nullptr};
};

} // namespace Skink::Ui::Brush
