#pragma once

#include <QColor>
#include <QObject>

class QWidget;

namespace Skink::Ui::Color {

class ColorPicker final : public QObject {
    Q_OBJECT

public:
    explicit ColorPicker(QObject* parent = nullptr);

    [[nodiscard]] QColor currentColor() const;
    void setCurrentColor(const QColor& color);
    void selectColor(const QColor& color);
    void open(QWidget* parent);

signals:
    void colorSelected(const QColor& color);

private:
    QColor m_currentColor{"#151515"};
};

} // namespace Skink::Ui::Color
