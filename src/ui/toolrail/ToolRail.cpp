#include "ui/toolrail/ToolRail.hpp"

#include <QPushButton>
#include <QStyle>
#include <QVBoxLayout>

namespace Skink::Ui::ToolRail {

namespace {

void setActiveProperty(QPushButton* button, bool active)
{
    button->setProperty("activeTool", active);
    button->style()->unpolish(button);
    button->style()->polish(button);
}

} // namespace

ToolRail::ToolRail(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("toolStrip");
    setFixedSize(42, 173);
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 6, 4, 6);
    layout->setSpacing(3);

    struct ToolSpec { const char* label; const char* tooltip; void (ToolRail::*signal)(); };
    const ToolSpec tools[] = {{"B", "Pincel", &ToolRail::brushRequested}, {"S", "Selección", &ToolRail::selectionRequested}, {"P", "Mover", &ToolRail::panRequested}, {"T", "Transformar", &ToolRail::transformRequested}};
    for (int index = 0; index < 4; ++index) {
        const ToolSpec& tool = tools[index];
        auto* button = new QPushButton(tool.label, this);
        button->setObjectName("stripButton");
        button->setToolTip(tool.tooltip);
        button->setFixedSize(34, 38);
        layout->addWidget(button);
        connect(button, &QPushButton::clicked, this, tool.signal);

        if (index == 0) m_brushButton = button;
        if (index == 1) m_selectionButton = button;
        if (index == 2) m_panButton = button;
        if (index == 3) m_transformButton = button;
    }

    setActiveTool(Core::Tools::Tool::Brush);
}

void ToolRail::setActiveTool(Core::Tools::Tool tool)
{
    setActiveProperty(m_brushButton, tool == Core::Tools::Tool::Brush);
    setActiveProperty(m_selectionButton, tool == Core::Tools::Tool::Selection);
    setActiveProperty(m_panButton, tool == Core::Tools::Tool::Pan);
    setActiveProperty(m_transformButton, tool == Core::Tools::Tool::Transform);
}

} // namespace Skink::Ui::ToolRail
