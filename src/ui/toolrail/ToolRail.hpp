#pragma once

#include <QWidget>

namespace Skink::Ui::ToolRail {

class ToolRail final : public QWidget {
    Q_OBJECT

public:
    explicit ToolRail(QWidget* parent = nullptr);

signals:
    void brushRequested();
    void selectionRequested();
    void panRequested();
    void transformRequested();
};

} // namespace Skink::Ui::ToolRail
