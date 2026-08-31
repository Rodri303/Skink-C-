#include "ui/layers/LayersPanel.hpp"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace Skink::Ui::Layers {
namespace {

QPushButton* makeButton(QWidget* parent, const QString& text, const char* objectName)
{
    auto* button = new QPushButton(text, parent);
    button->setObjectName(objectName);
    return button;
}

QWidget* makeRow(QWidget* parent, const QString& name, const QString& meta, bool selected, int index, LayersPanel* panel)
{
    auto* row = new QPushButton(parent);
    row->setObjectName(selected ? "layerSelected" : "layerRow");
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
    QObject::connect(row, &QPushButton::clicked, panel, [panel, index, row] { emit panel->layerSelected(index); row->setObjectName("layerSelected"); });
    return row;
}

} // namespace

LayersPanel::LayersPanel(QWidget* parent) : QFrame(parent)
{
    setObjectName("floatingPanel");
    setFixedSize(330, 376);
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
    layout->addWidget(makeRow(this, "Capa 4", "100% N", true, 0, this));
    layout->addWidget(makeRow(this, "Capa 3", "55% N", false, 1, this));
    layout->addWidget(makeRow(this, "Capa 2", "100% N", false, 2, this));
    layout->addWidget(makeRow(this, "Capa 1", "100% N", false, 3, this));
    layout->addWidget(makeRow(this, "Fondo", "LOCK", false, 4, this));
    connect(add, &QPushButton::clicked, this, &LayersPanel::addLayerRequested);
}

} // namespace Skink::Ui::Layers
