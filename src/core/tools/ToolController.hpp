#pragma once

#include <QObject>

namespace Skink::Core::Tools {

enum class Tool {
    Brush,
    Eraser,
    Pan,
    Selection,
    Transform
};

class ToolController final : public QObject {
    Q_OBJECT

public:
    explicit ToolController(QObject* parent = nullptr);

    [[nodiscard]] Tool activeTool() const noexcept;

public slots:
    void setActiveTool(Tool tool);

signals:
    void activeToolChanged(Tool tool);

private:
    Tool m_activeTool{Tool::Brush};
};

} // namespace Skink::Core::Tools
