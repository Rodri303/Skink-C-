#pragma once

#include <QWidget>

class QColor;

namespace Skink::Ui::TopBar {

class TopBar final : public QWidget {
    Q_OBJECT

public:
    explicit TopBar(QWidget* parent = nullptr);

signals:
    void undoRequested();
    void redoRequested();
    void colorSelected(const QColor& color);
};

} // namespace Skink::Ui::TopBar
