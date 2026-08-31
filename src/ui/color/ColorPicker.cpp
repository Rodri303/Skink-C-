#include "ui/color/ColorPicker.hpp"

#include <QColorDialog>
#include <QDialog>

namespace Skink::Ui::Color {

ColorPicker::ColorPicker(QObject* parent)
    : QObject(parent)
{
}

QColor ColorPicker::currentColor() const
{
    return m_currentColor;
}

void ColorPicker::setCurrentColor(const QColor& color)
{
    if (!color.isValid()) return;

    m_currentColor = color.toRgb();
    m_currentColor.setAlpha(255);
}

void ColorPicker::open(QWidget* parent)
{
    QColorDialog dialog(m_currentColor, parent);
    dialog.setWindowTitle("Color del pincel");
    dialog.setOption(QColorDialog::ShowAlphaChannel, false);

    if (dialog.exec() != QDialog::Accepted) return;

    QColor selectedColor = dialog.selectedColor();
    if (!selectedColor.isValid()) return;

    selectedColor = selectedColor.toRgb();
    selectedColor.setAlpha(255);
    m_currentColor = selectedColor;
    emit colorSelected(m_currentColor);
}

} // namespace Skink::Ui::Color
