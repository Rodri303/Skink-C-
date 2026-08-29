#include "ui/panels/DraggablePanel.hpp"

#include <QLabel>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <algorithm>

namespace Skink::Ui::Panels {

DraggablePanel::DraggablePanel(const QString& title, QWidget* parent)
    : QFrame(parent)
{
    setObjectName("draggablePanel");
    setFrameShape(QFrame::NoFrame);
    setMinimumWidth(190);

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 8, 10, 10);
    root->setSpacing(8);

    m_titleLabel = new QLabel(title, this);
    m_titleLabel->setObjectName("panelTitle");
    m_titleLabel->setCursor(Qt::SizeAllCursor);
    root->addWidget(m_titleLabel);

    m_contentLayout = new QVBoxLayout();
    m_contentLayout->setContentsMargins(0, 0, 0, 0);
    m_contentLayout->setSpacing(7);
    root->addLayout(m_contentLayout);
}

QVBoxLayout* DraggablePanel::contentLayout() const
{
    return m_contentLayout;
}

void DraggablePanel::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && canStartDrag(event->position().toPoint())) {
        m_dragging = true;
        m_dragOffset = event->position().toPoint();
        raise();
        grabMouse();
        event->accept();
        return;
    }

    QFrame::mousePressEvent(event);
}

void DraggablePanel::mouseMoveEvent(QMouseEvent* event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        const QPoint target = mapToParent(event->position().toPoint() - m_dragOffset);
        move(clampedPosition(target));
        event->accept();
        return;
    }

    QFrame::mouseMoveEvent(event);
}

void DraggablePanel::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_dragging) {
        m_dragging = false;
        releaseMouse();
        event->accept();
        return;
    }

    QFrame::mouseReleaseEvent(event);
}

bool DraggablePanel::canStartDrag(const QPoint& localPosition) const
{
    const int titleBottom = m_titleLabel->geometry().bottom() + 8;
    return localPosition.y() <= titleBottom;
}

QPoint DraggablePanel::clampedPosition(const QPoint& requested) const
{
    if (!parentWidget()) {
        return requested;
    }

    const QRect bounds = parentWidget()->rect();
    const int maxX = std::max(0, bounds.width() - width());
    const int maxY = std::max(0, bounds.height() - height());

    return {
        std::clamp(requested.x(), 0, maxX),
        std::clamp(requested.y(), 0, maxY)
    };
}

} // namespace Skink::Ui::Panels
