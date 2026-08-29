#pragma once

#include "core/brush/IBrushEngine.hpp"

#include <optional>

namespace Skink::Core::Brush {

class BasicBrushEngine final : public IBrushEngine {
public:
    void setSettings(const BrushSettings& settings) override;
    [[nodiscard]] BrushSettings settings() const override;

    void beginStroke(const BrushSample& sample) override;
    void continueStroke(QPainter& painter, const BrushSample& sample) override;
    void endStroke() override;

private:
    BrushSettings m_settings;
    std::optional<BrushSample> m_previous;
};

} // namespace Skink::Core::Brush
