#pragma once

#include <QWidget>

class QLabel;
class QString;
class QTimer;

namespace Skink::Ui::Hud {

class CanvasHud final : public QWidget {
    Q_OBJECT

public:
    explicit CanvasHud(QWidget* parent = nullptr);

    void showMessage(const QString& message, int durationMs = 0);

private:
    QLabel* m_label{nullptr};
    QTimer* m_hideTimer{nullptr};
};

} // namespace Skink::Ui::Hud
