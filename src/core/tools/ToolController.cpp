#include "core/tools/ToolController.hpp"

namespace Skink::Core::Tools {

ToolController::ToolController(QObject* parent)
    : QObject(parent)
{
}

Tool ToolController::activeTool() const noexcept
{
    return m_activeTool;
}

void ToolController::setActiveTool(Tool tool)
{
    if (m_activeTool == tool) return;

    m_activeTool = tool;
    emit activeToolChanged(m_activeTool);
}

} // namespace Skink::Core::Tools
