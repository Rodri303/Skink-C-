#pragma once

#include <QWidget>

namespace Skink::Ui::Brush {

class BrushControls final : public QWidget {
    Q_OBJECT

public:
    explicit BrushControls(QWidget* parent = nullptr);

signals:
    void brushSizeChanged(int value);
    void opacityChanged(int value);
    void pressureSensitivityChanged(int value);
};

} // namespace Skink::Ui::Brush
