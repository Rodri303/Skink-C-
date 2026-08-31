#pragma once

#include <QFrame>

namespace Skink::Ui::Layers {

class LayersPanel final : public QFrame {
    Q_OBJECT

public:
    explicit LayersPanel(QWidget* parent = nullptr);

signals:
    void layerSelected(int index);
    void addLayerRequested();
    void visibilityToggled(int index, bool visible);
    void lockToggled(int index, bool locked);
};

} // namespace Skink::Ui::Layers
