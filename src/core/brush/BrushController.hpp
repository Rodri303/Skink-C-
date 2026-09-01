#pragma once

#include "core/brush/BrushState.hpp"

#include <QObject>

namespace Skink::Core::Brush {

class BrushController final : public QObject {
    Q_OBJECT

public:
    explicit BrushController(QObject* parent = nullptr);

    [[nodiscard]] const BrushState& state() const noexcept;

public slots:
    void setSize(int size);
    void adjustSize(int delta);
    void setOpacity(int opacity);
    void setPressureSensitivity(int sensitivity);
    void setColor(const QColor& color);
    void setPreset(BrushPreset preset);

signals:
    void stateChanged(const Skink::Core::Brush::BrushState& state);
    void sizeChanged(int size);
    void opacityChanged(int opacity);
    void pressureSensitivityChanged(int sensitivity);
    void colorChanged(const QColor& color);
    void presetChanged(Skink::Core::Brush::BrushPreset preset);

private:
    void notifyStateChanged();

    BrushState m_state;
};

} // namespace Skink::Core::Brush
