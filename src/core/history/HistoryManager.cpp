#include "core/history/HistoryManager.hpp"

#include <algorithm>

namespace Skink::Core::History {

HistoryManager::HistoryManager(std::size_t maxStates)
    : m_maxStates(std::max<std::size_t>(2, maxStates))
{
}

void HistoryManager::initialize(const QImage& image)
{
    m_undo = {image};
    m_redo.clear();
}

void HistoryManager::push(const QImage& image)
{
    if (!m_undo.empty() && m_undo.back() == image) {
        return;
    }

    m_undo.push_back(image);
    if (m_undo.size() > m_maxStates) {
        m_undo.erase(m_undo.begin());
    }
    m_redo.clear();
}

bool HistoryManager::canUndo() const noexcept
{
    return m_undo.size() > 1;
}

bool HistoryManager::canRedo() const noexcept
{
    return !m_redo.empty();
}

bool HistoryManager::undo(QImage& target)
{
    if (!canUndo()) {
        return false;
    }

    m_redo.push_back(m_undo.back());
    m_undo.pop_back();
    target = m_undo.back();
    return true;
}

bool HistoryManager::redo(QImage& target)
{
    if (!canRedo()) {
        return false;
    }

    target = m_redo.back();
    m_redo.pop_back();
    m_undo.push_back(target);
    return true;
}

} // namespace Skink::Core::History
