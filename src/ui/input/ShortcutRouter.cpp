#include "ui/input/ShortcutRouter.hpp"

#include <QAbstractSpinBox>
#include <QApplication>
#include <QComboBox>
#include <QEvent>
#include <QKeyEvent>
#include <QKeySequence>
#include <QKeySequenceEdit>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QTextEdit>
#include <QWidget>

namespace Skink::Ui::Input {

ShortcutRouter::ShortcutRouter(QObject* parent)
    : QObject(parent)
{
    qApp->installEventFilter(this);
}

bool ShortcutRouter::eventFilter(QObject* watched, QEvent* event)
{
    Q_UNUSED(watched);

    if (event->type() == QEvent::ApplicationDeactivate) {
        setTemporaryPan(false);
        setNavigationModifiers(false, false, false);
        emit navigationCancelled();
        return false;
    }

    if (event->type() != QEvent::KeyPress && event->type() != QEvent::KeyRelease) {
        return false;
    }

    auto* keyEvent = static_cast<QKeyEvent*>(event);
    const bool pressed = event->type() == QEvent::KeyPress;
    const bool editableFocus = focusAcceptsTextInput();

    if (!keyEvent->isAutoRepeat()
        && (keyEvent->key() == Qt::Key_Control
            || keyEvent->key() == Qt::Key_Alt
            || keyEvent->key() == Qt::Key_Shift)) {
        bool control = m_controlHeld;
        bool alt = m_altHeld;
        bool shift = m_shiftHeld;

        if (keyEvent->key() == Qt::Key_Control && (!pressed || !editableFocus)) control = pressed;
        if (keyEvent->key() == Qt::Key_Alt && (!pressed || !editableFocus)) alt = pressed;
        if (keyEvent->key() == Qt::Key_Shift && (!pressed || !editableFocus)) shift = pressed;
        setNavigationModifiers(control, alt, shift);
        return false;
    }

    if (keyEvent->key() == Qt::Key_Space && (!editableFocus || m_temporaryPanActive)) {
        if (!keyEvent->isAutoRepeat()) setTemporaryPan(pressed);
        keyEvent->accept();
        return true;
    }

    if (!pressed || editableFocus) return false;

    if (keyEvent->matches(QKeySequence::Undo)) {
        emit undoRequested();
    } else if (keyEvent->matches(QKeySequence::Redo)) {
        emit redoRequested();
    } else if (keyEvent->modifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier)) {
        return false;
    } else if (keyEvent->key() == Qt::Key_B) {
        emit brushRequested();
    } else if (keyEvent->key() == Qt::Key_E) {
        emit eraserRequested();
    } else if (keyEvent->key() == Qt::Key_C || keyEvent->key() == Qt::Key_0) {
        emit resetViewRequested();
    } else if (keyEvent->key() == Qt::Key_BracketLeft) {
        emit brushSizeStepRequested(-2);
    } else if (keyEvent->key() == Qt::Key_BracketRight) {
        emit brushSizeStepRequested(2);
    } else {
        return false;
    }

    keyEvent->accept();
    return true;
}

bool ShortcutRouter::focusAcceptsTextInput() const
{
    for (QWidget* widget = QApplication::focusWidget(); widget; widget = widget->parentWidget()) {
        if (qobject_cast<QLineEdit*>(widget)
            || qobject_cast<QTextEdit*>(widget)
            || qobject_cast<QPlainTextEdit*>(widget)
            || qobject_cast<QAbstractSpinBox*>(widget)
            || qobject_cast<QKeySequenceEdit*>(widget)) {
            return true;
        }

        if (const auto* comboBox = qobject_cast<QComboBox*>(widget); comboBox && comboBox->isEditable()) {
            return true;
        }
    }
    return false;
}

void ShortcutRouter::setTemporaryPan(bool active)
{
    if (m_temporaryPanActive == active) return;

    m_temporaryPanActive = active;
    emit temporaryPanChanged(m_temporaryPanActive);
}

void ShortcutRouter::setNavigationModifiers(bool control, bool alt, bool shift)
{
    if (m_controlHeld == control && m_altHeld == alt && m_shiftHeld == shift) return;

    m_controlHeld = control;
    m_altHeld = alt;
    m_shiftHeld = shift;
    emit navigationModifiersChanged(m_controlHeld, m_altHeld, m_shiftHeld);
}

} // namespace Skink::Ui::Input
