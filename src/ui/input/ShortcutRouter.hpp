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
    void undoRequested();
    void redoRequested();

private:
    [[nodiscard]] bool focusAcceptsTextInput() const;
    void setTemporaryPan(bool active);

    bool m_temporaryPanActive{false};
};

} // namespace Skink::Ui::Input
