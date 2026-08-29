#pragma once

#include <QColor>
#include <QPointF>

class QPainter;

namespace Skink::Core::Brush {

struct BrushSettings {
    QColor color{Qt::black};
    qreal size{18.0};
    qreal opacity{1.0};
};

struct BrushSample {
    QPointF position;
    qreal pressure{1.0};
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
