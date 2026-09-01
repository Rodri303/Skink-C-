#pragma once

#include "core/brush/BrushState.hpp"

#include <QColor>
#include <QPointF>
#include <QtGlobal>

class QPainter;

namespace Skink::Core::Brush {

struct BrushSettings {
    QColor color{"#151515"};
    qreal size{kDefaultBrushSize};
    qreal opacity{1.0};
    qreal pressureSensitivity{kDefaultPressureSensitivity / 100.0};
};

struct BrushSample {
    QPointF position;
    qreal pressure{1.0};
    qreal xTilt{0.0};
    qreal yTilt{0.0};
    qreal rotation{0.0};
    qreal tangentialPressure{0.0};
    quint64 timestamp{0};
};

class IBrushEngine {
public:
    virtual ~IBrushEngine() = default;

    virtual void setSettings(const BrushSettings& settings) = 0;
    [[nodiscard]] virtual BrushSettings settings() const = 0;

    virtual void beginStroke(const BrushSample& sample) = 0;
    virtual void continueStroke(QPainter& painter, const BrushSample& sample) = 0;
    virtual void endStroke() = 0;
};

} // namespace Skink::Core::Brush
