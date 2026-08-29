#pragma once

#include <QPushButton>

namespace Skink::Ui::Widgets {

class ToolButton final : public QPushButton {
    Q_OBJECT

public:
    explicit ToolButton(const QString& label, QWidget* parent = nullptr);
};

} // namespace Skink::Ui::Widgets
