#include "ui/toolrail/ToolRail.hpp"

#include <QPushButton>
#include <QVBoxLayout>

namespace Skink::Ui::ToolRail {

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
        button->setObjectName(index == 0 ? "stripButtonActive" : "stripButton");
        button->setToolTip(tool.tooltip);
        button->setFixedSize(34, 38);
        layout->addWidget(button);
        connect(button, &QPushButton::clicked, this, tool.signal);
    }
}

} // namespace Skink::Ui::ToolRail
