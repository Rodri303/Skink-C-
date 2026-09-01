#pragma once

#include <QColor>
#include <QMetaType>

namespace Skink::Core::Brush {

inline constexpr int kMinimumBrushSize = 1;
inline constexpr int kMaximumBrushSize = 160;
inline constexpr int kDefaultBrushSize = 14;
inline constexpr int kMinimumBrushOpacity = 5;
inline constexpr int kMaximumBrushOpacity = 100;
inline constexpr int kDefaultBrushOpacity = 100;
inline constexpr int kMinimumPressureSensitivity = 0;
inline constexpr int kMaximumPressureSensitivity = 100;
inline constexpr int kDefaultPressureSensitivity = 85;

enum class BrushPreset {
    TransparentInk,
    Marker,
    Pencil,
    Airbrush
};

struct BrushState {
    int size{kDefaultBrushSize};
    int opacity{kDefaultBrushOpacity};
    int pressureSensitivity{kDefaultPressureSensitivity};
    QColor color{"#151515"};
    BrushPreset preset{BrushPreset::Marker};
};

} // namespace Skink::Core::Brush

Q_DECLARE_METATYPE(Skink::Core::Brush::BrushPreset)
Q_DECLARE_METATYPE(Skink::Core::Brush::BrushState)
