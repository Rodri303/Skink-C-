#include "ui/layers/LayersPanel.hpp"

#include <QButtonGroup>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
#include <QVBoxLayout>

namespace Skink::Ui::Layers {
namespace {

QPushButton* makeButton(QWidget* parent, const QString& text, const char* objectName)
{
    auto* button = new QPushButton(text, parent);
    button->setObjectName(objectName);
    return button;
}

QPushButton* makeRow(QWidget* parent, const QString& name, const QString& meta, bool selected, int index, LayersPanel* panel)
{
    auto* row = new QPushButton(parent);
    row->setObjectName("layerRow");
    row->setCheckable(true);
    row->setChecked(selected);
    row->setFixedHeight(66);
    auto* layout = new QHBoxLayout(row);
    layout->setContentsMargins(13, 0, 13, 0);
    layout->setSpacing(8);

    auto* visibility = makeButton(row, "O", "layerIconButton");
    visibility->setCheckable(true);
    visibility->setChecked(true);
    visibility->setFixedWidth(20);
    auto* thumbnail = new QFrame(row);
    thumbnail->setObjectName(selected ? "layerThumbDark" : "layerThumb");
    thumbnail->setFixedSize(48, 48);
    auto* title = new QLabel(name, row);
    title->setObjectName("layerName");
    auto* details = new QLabel(meta, row);
    details->setObjectName("layerMeta");
    auto* menu = makeButton(row, "...", "layerIconButton");
    menu->setFixedWidth(20);

    layout->addWidget(visibility);
    layout->addWidget(thumbnail);
    layout->addWidget(title, 1);
    layout->addWidget(details);
    layout->addWidget(menu);
    QObject::connect(visibility, &QPushButton::toggled, panel, [panel, index](bool visible) { emit panel->visibilityToggled(index, visible); });
    return row;
}

} // namespace

LayersPanel::LayersPanel(QWidget* parent) : QFrame(parent)
{
    setObjectName("floatingPanel");
    setMinimumSize(330, 376);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    auto* header = new QWidget(this);
    header->setObjectName("panelHeader");
    header->setFixedHeight(46);
    auto* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(20, 0, 20, 0);
    auto* title = new QLabel("CAPAS", header);
    title->setObjectName("panelTitle");
    auto* add = makeButton(header, "+", "panelHeaderButton");
    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(add);
    layout->addWidget(header);
    auto* layerGroup = new QButtonGroup(this);
    layerGroup->setExclusive(true);
    const QString names[] = {"Capa 4", "Capa 3", "Capa 2", "Capa 1", "Fondo"};
    const QString metadata[] = {"100% N", "55% N", "100% N", "100% N", "LOCK"};
    for (int index = 0; index < 5; ++index) {
        auto* row = makeRow(this, names[index], metadata[index], index == 0, index, this);
        layerGroup->addButton(row, index);
        layout->addWidget(row);
    }
    connect(layerGroup, &QButtonGroup::idToggled, this, [this](int index, bool checked) {
        if (checked) emit layerSelected(index);
    });
    connect(add, &QPushButton::clicked, this, &LayersPanel::addLayerRequested);
}

} // namespace Skink::Ui::Layers
