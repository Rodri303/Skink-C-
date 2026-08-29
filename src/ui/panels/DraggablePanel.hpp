#pragma once

#include <QFrame>
#include <QPoint>

class QLabel;
class QVBoxLayout;

namespace Skink::Ui::Panels {

class DraggablePanel final : public QFrame {
    Q_OBJECT

public:
    explicit DraggablePanel(const QString& title, QWidget* parent = nullptr);

    QVBoxLayout* contentLayout() const;

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    [[nodiscard]] bool canStartDrag(const QPoint& localPosition) const;
    [[nodiscard]] QPoint clampedPosition(const QPoint& requested) const;

    QLabel* m_titleLabel{nullptr};
    QVBoxLayout* m_contentLayout{nullptr};
    QPoint m_dragOffset;
    bool m_dragging{false};
};

} // namespace Skink::Ui::Panels
