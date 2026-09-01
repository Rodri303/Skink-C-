#pragma once

#include "core/brush/IBrushEngine.hpp"

#include <optional>
#include <vector>

namespace Skink::Core::Stroke {

struct StrokeSettings {
    qreal smoothing{0.35};
    qreal spacingFactor{0.14};
    qreal minimumSpacing{0.55};
};

class StrokeEngine final {
public:
    void setSettings(const StrokeSettings& settings);
    [[nodiscard]] StrokeSettings settings() const noexcept;

    void beginStroke(const Brush::BrushSample& sample);
    [[nodiscard]] std::vector<Brush::BrushSample> processSample(
        const Brush::BrushSample& rawSample,
        qreal brushSize);
    void endStroke();

private:
    [[nodiscard]] Brush::BrushSample smooth(const Brush::BrushSample& raw) const;

    StrokeSettings m_settings;
    std::optional<Brush::BrushSample> m_previousSmooth;
};

} // namespace Skink::Core::Stroke
