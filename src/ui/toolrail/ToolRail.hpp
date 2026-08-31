#pragma once

#include "core/tools/ToolController.hpp"

#include <QWidget>

class QPushButton;

namespace Skink::Ui::ToolRail {

class ToolRail final : public QWidget {
    Q_OBJECT

public:
    explicit ToolRail(QWidget* parent = nullptr);
    void setActiveTool(Core::Tools::Tool tool);

signals:
    void brushRequested();
    void selectionRequested();
    void panRequested();
    void transformRequested();

private:
    QPushButton* m_brushButton{nullptr};
    QPushButton* m_selectionButton{nullptr};
    QPushButton* m_panButton{nullptr};
    QPushButton* m_transformButton{nullptr};
};

} // namespace Skink::Ui::ToolRail
