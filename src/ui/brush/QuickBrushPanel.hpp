#pragma once

#include "core/brush/BrushState.hpp"

#include <QFrame>

class QButtonGroup;

namespace Skink::Ui::Brush {

class QuickBrushPanel final : public QFrame {
    Q_OBJECT

public:
    explicit QuickBrushPanel(QWidget* parent = nullptr);

public slots:
    void setActivePreset(Core::Brush::BrushPreset preset);

signals:
    void presetSelected(Core::Brush::BrushPreset preset);

private:
    QButtonGroup* m_presetGroup{nullptr};
};

} // namespace Skink::Ui::Brush
