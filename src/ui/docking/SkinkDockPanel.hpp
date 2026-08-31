#pragma once

#include <QDockWidget>

namespace Skink::Ui::Docking {

class SkinkDockPanel final : public QDockWidget {
    Q_OBJECT

public:
    explicit SkinkDockPanel(const QString& title, QWidget* content, QWidget* parent = nullptr);
};

} // namespace Skink::Ui::Docking
