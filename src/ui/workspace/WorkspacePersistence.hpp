#pragma once

class QMainWindow;

namespace Skink::Ui::Workspace {

class WorkspacePersistence final {
public:
    [[nodiscard]] static bool restore(QMainWindow& window);
    static void save(const QMainWindow& window);
    static void ensureVisible(QMainWindow& window);
};

} // namespace Skink::Ui::Workspace
