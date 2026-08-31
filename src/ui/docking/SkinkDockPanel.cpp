#include "ui/docking/SkinkDockPanel.hpp"

namespace Skink::Ui::Docking {

SkinkDockPanel::SkinkDockPanel(const QString& title, QWidget* content, QWidget* parent)
    : QDockWidget(title, parent)
{
    setObjectName(title + QStringLiteral("Dock"));
    setWidget(content);
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setFeatures(
        QDockWidget::DockWidgetMovable
        | QDockWidget::DockWidgetFloatable
        | QDockWidget::DockWidgetClosable);
}

} // namespace Skink::Ui::Docking
