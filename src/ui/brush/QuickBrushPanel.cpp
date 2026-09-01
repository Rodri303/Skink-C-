#include "ui/brush/QuickBrushPanel.hpp"

#include <QButtonGroup>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
#include <QVBoxLayout>

#include <array>

namespace Skink::Ui::Brush {

QuickBrushPanel::QuickBrushPanel(QWidget* parent) : QFrame(parent)
{
    setObjectName("floatingPanel");
    setMinimumSize(330, 254);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
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
    const std::array<QString, 4> names = {
        "Tinta transparente", "Marcador", "Lapiz", "Aerografo"};
    const std::array<Core::Brush::BrushPreset, 4> presets = {
        Core::Brush::BrushPreset::TransparentInk,
        Core::Brush::BrushPreset::Marker,
        Core::Brush::BrushPreset::Pencil,
        Core::Brush::BrushPreset::Airbrush};
    m_presetGroup = new QButtonGroup(this);
    m_presetGroup->setExclusive(true);
    for (int index = 0; index < 4; ++index) {
        auto* card = new QPushButton(names[index] + "\n\n----------", gridHost);
        card->setObjectName("quickCard");
        card->setCheckable(true);
        card->setChecked(index == 1);
        card->setMinimumHeight(92);
        m_presetGroup->addButton(card, static_cast<int>(presets[index]));
        grid->addWidget(card, index / 2, index % 2);
        connect(card, &QPushButton::toggled, this, [this, preset = presets[index]](bool checked) {
            if (checked) emit presetSelected(preset);
        });
    }
    layout->addWidget(gridHost, 1);
}

void QuickBrushPanel::setActivePreset(Core::Brush::BrushPreset preset)
{
    if (auto* button = m_presetGroup->button(static_cast<int>(preset))) {
        button->setChecked(true);
    }
}

} // namespace Skink::Ui::Brush
