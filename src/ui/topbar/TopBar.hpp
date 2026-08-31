#pragma once

#include <QWidget>

class QColor;
class QFrame;

namespace Skink::Ui::TopBar {

class TopBar final : public QWidget {
    Q_OBJECT

public:
    explicit TopBar(QWidget* parent = nullptr);
    void setActiveColor(const QColor& color);

signals:
    void undoRequested();
    void redoRequested();
    void colorPickerRequested();

private:
    QFrame* m_colorIndicator{nullptr};
};

} // namespace Skink::Ui::TopBar
