#include "ui/widgets/ToolButton.hpp"

namespace Skink::Ui::Widgets {

ToolButton::ToolButton(const QString& label, QWidget* parent)
    : QPushButton(label, parent)
{
    setCheckable(true);
    setMinimumHeight(32);
}

} // namespace Skink::Ui::Widgets
