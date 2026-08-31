#include "ui/hud/CanvasHud.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>

namespace Skink::Ui::Hud {

namespace {

// Provisional until the exact SWINK HUD timeout is recovered.
constexpr int kProvisionalHudDurationMs = 1200;

} // namespace

CanvasHud::CanvasHud(QWidget* parent)
    : QWidget(parent)
    , m_label(new QLabel(this))
    , m_hideTimer(new QTimer(this))
{
    setObjectName("floatingHud");
    setAttribute(Qt::WA_StyledBackground, true);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);

    m_label->setObjectName("canvasHudText");
    m_label->setAlignment(Qt::AlignCenter);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 8, 12, 8);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    layout->addWidget(m_label);

    setStyleSheet(R"(
        #floatingHud {
            background: rgba(14, 15, 17, 235);
            border: 1px solid rgba(255, 255, 255, 18);
            border-radius: 9px;
        }

        #canvasHudText {
            color: #f1f2f3;
            font-size: 11px;
            font-weight: 600;
        }
    )");

    m_hideTimer->setSingleShot(true);
    connect(m_hideTimer, &QTimer::timeout, this, &CanvasHud::hide);
    hide();
}

void CanvasHud::showMessage(const QString& message, int durationMs)
{
    m_label->setText(message);
    adjustSize();
    show();
    raise();
    m_hideTimer->start(durationMs > 0 ? durationMs : kProvisionalHudDurationMs);
}

} // namespace Skink::Ui::Hud
