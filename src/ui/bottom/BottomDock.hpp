#pragma once

#include <QWidget>

namespace Skink::Ui::Bottom {

class BottomDock final : public QWidget {
    Q_OBJECT

public:
    explicit BottomDock(QWidget* parent = nullptr);

signals:
    void zoomInRequested();
    void zoomOutRequested();
    void resetViewRequested();
    void previewRequested();
    void stopRequested();
    void savePsdRequested();
    void processDurationChanged(int seconds);
};

} // namespace Skink::Ui::Bottom
