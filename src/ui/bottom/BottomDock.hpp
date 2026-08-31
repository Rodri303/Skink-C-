#pragma once

#include <QWidget>

class QLabel;

namespace Skink::Ui::Bottom {

class BottomDock final : public QWidget {
    Q_OBJECT

public:
    explicit BottomDock(QWidget* parent = nullptr);
    void setZoomPercent(int percent);

signals:
    void zoomInRequested();
    void zoomOutRequested();
    void resetViewRequested();
    void previewRequested();
    void stopRequested();
    void savePsdRequested();
    void processDurationChanged(int seconds);

private:
    QLabel* m_zoomPercent{nullptr};
};

} // namespace Skink::Ui::Bottom
