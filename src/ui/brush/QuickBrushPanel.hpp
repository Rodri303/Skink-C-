#pragma once

#include <QFrame>

namespace Skink::Ui::Brush {

class QuickBrushPanel final : public QFrame {
    Q_OBJECT

public:
    explicit QuickBrushPanel(QWidget* parent = nullptr);

signals:
    void presetSelected(const QString& preset);
};

} // namespace Skink::Ui::Brush
