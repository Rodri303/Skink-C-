#pragma once

#include <QImage>

#include <cstddef>
#include <vector>

namespace Skink::Core::History {

class HistoryManager final {
public:
    explicit HistoryManager(std::size_t maxStates = 24);

    void initialize(const QImage& image);
    void push(const QImage& image);

    [[nodiscard]] bool canUndo() const noexcept;
    [[nodiscard]] bool canRedo() const noexcept;
    [[nodiscard]] bool undo(QImage& target);
    [[nodiscard]] bool redo(QImage& target);

private:
    std::size_t m_maxStates;
    std::vector<QImage> m_undo;
    std::vector<QImage> m_redo;
};

} // namespace Skink::Core::History
