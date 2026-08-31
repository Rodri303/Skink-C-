#pragma once

#include <QObject>

namespace Skink::Ui::Input {

class ShortcutRouter final : public QObject {
    Q_OBJECT

public:
    explicit ShortcutRouter(QObject* parent = nullptr);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

signals:
    void brushRequested();
    void eraserRequested();
    void resetViewRequested();
    void brushSizeStepRequested(int delta);
    void temporaryPanChanged(bool active);
    void navigationModifiersChanged(bool control, bool alt, bool shift);
    void navigationCancelled();
    void undoRequested();
    void redoRequested();

private:
    [[nodiscard]] bool focusAcceptsTextInput() const;
    void setTemporaryPan(bool active);
    void setNavigationModifiers(bool control, bool alt, bool shift);

    bool m_temporaryPanActive{false};
    bool m_controlHeld{false};
    bool m_altHeld{false};
    bool m_shiftHeld{false};
};

} // namespace Skink::Ui::Input
