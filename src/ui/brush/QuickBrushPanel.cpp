#include "ui/brush/QuickBrushPanel.hpp"

#include <QButtonGroup>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace Skink::Ui::Brush {

QuickBrushPanel::QuickBrushPanel(QWidget* parent) : QFrame(parent)
{
    setObjectName("floatingPanel");
    setFixedSize(330, 254);
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    auto* header = new QWidget(this);
    header->setObjectName("panelHeader");
    header->setFixedHeight(46);
    auto* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(20, 0, 20, 0);
    auto* title = new QLabel("PINCEL RAPIDO", header);
    title->setObjectName("panelTitle");
    headerLayout->addWidget(title);
    headerLayout->addStretch();
    layout->addWidget(header);
    auto* gridHost = new QWidget(this);
    auto* grid = new QGridLayout(gridHost);
    grid->setContentsMargins(12, 12, 12, 12);
    grid->setSpacing(8);
    const QString names[] = {"Tinta transparente", "Marcador", "Lapiz", "Aerografo"};
    auto* group = new QButtonGroup(this);
    group->setExclusive(true);
    for (int index = 0; index < 4; ++index) {
        auto* card = new QPushButton(names[index] + "\n\n----------", gridHost);
        card->setObjectName("quickCard");
        card->setCheckable(true);
        card->setChecked(index == 1);
        card->setMinimumHeight(92);
        group->addButton(card, index);
        grid->addWidget(card, index / 2, index % 2);
    }
    for (int index = 0; index < 4; ++index) {
        auto* card = group->button(index);
        connect(card, &QPushButton::toggled, this, [this, names, index](bool checked) {
            if (checked) emit presetSelected(names[index]);
        });
    }
    layout->addWidget(gridHost, 1);
}

} // namespace Skink::Ui::Brush
