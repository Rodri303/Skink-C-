#include "ui/workspace/WorkspacePersistence.hpp"

#include <QByteArray>
#include <QDockWidget>
#include <QGuiApplication>
#include <QMainWindow>
#include <QScreen>
#include <QSettings>

#include <algorithm>

namespace Skink::Ui::Workspace {

namespace {

constexpr int kStateVersion = 1;
constexpr int kFallbackMargin = 16;
constexpr int kMinimumFallbackWidth = 320;
constexpr int kMinimumFallbackHeight = 240;
constexpr auto kSettingsGroup = "Workspace";
constexpr auto kGeometryKey = "MainWindowGeometry";
constexpr auto kStateKey = "MainWindowState";

bool intersectsAvailableScreen(const QRect& geometry)
{
    if (!geometry.isValid()) return false;

    for (const QScreen* screen : QGuiApplication::screens()) {
        if (screen && screen->availableGeometry().intersects(geometry)) return true;
    }
    return false;
}

QScreen* fallbackScreen()
{
    if (QScreen* primary = QGuiApplication::primaryScreen()) return primary;

    const auto screens = QGuiApplication::screens();
    return screens.isEmpty() ? nullptr : screens.constFirst();
}

QRect recoveryGeometry(const QWidget& widget, const QRect& availableGeometry)
{
    QRect usableGeometry = availableGeometry.adjusted(
        kFallbackMargin,
        kFallbackMargin,
        -kFallbackMargin,
        -kFallbackMargin);
    if (!usableGeometry.isValid()) usableGeometry = availableGeometry;

    QSize restoredSize = widget.frameGeometry().size();
    if (!restoredSize.isValid() || restoredSize.isEmpty()) restoredSize = widget.size();
    if (!restoredSize.isValid() || restoredSize.isEmpty()) restoredSize = widget.sizeHint();

    const int minimumWidth = std::min(
        usableGeometry.width(),
        std::max(kMinimumFallbackWidth, widget.minimumWidth()));
    const int minimumHeight = std::min(
        usableGeometry.height(),
        std::max(kMinimumFallbackHeight, widget.minimumHeight()));
    const int width = std::clamp(restoredSize.width(), minimumWidth, usableGeometry.width());
    const int height = std::clamp(restoredSize.height(), minimumHeight, usableGeometry.height());

    const QPoint centeredPosition(
        usableGeometry.left() + (usableGeometry.width() - width) / 2,
        usableGeometry.top() + (usableGeometry.height() - height) / 2);
    return {centeredPosition, QSize(width, height)};
}

void recoverWidgetIfOffscreen(QWidget& widget)
{
    QRect geometry = widget.frameGeometry();
    if (!geometry.isValid()) geometry = widget.geometry();
    if (intersectsAvailableScreen(geometry)) return;

    QScreen* screen = fallbackScreen();
    if (!screen) return;

    const bool wasMaximized = widget.isMaximized();
    if (wasMaximized) widget.setWindowState(widget.windowState() & ~Qt::WindowMaximized);
    widget.setGeometry(recoveryGeometry(widget, screen->availableGeometry()));
    if (wasMaximized) widget.setWindowState(widget.windowState() | Qt::WindowMaximized);
}

} // namespace

bool WorkspacePersistence::restore(QMainWindow& window)
{
    QSettings settings;
    settings.beginGroup(kSettingsGroup);
    const QByteArray geometry = settings.value(kGeometryKey).toByteArray();
    const QByteArray state = settings.value(kStateKey).toByteArray();
    settings.endGroup();

    if (geometry.isEmpty() || state.isEmpty()) return false;

    const bool geometryRestored = window.restoreGeometry(geometry);
    const bool stateRestored = window.restoreState(state, kStateVersion);
    return geometryRestored && stateRestored;
}

void WorkspacePersistence::save(const QMainWindow& window)
{
    QSettings settings;
    settings.beginGroup(kSettingsGroup);
    settings.setValue(kGeometryKey, window.saveGeometry());
    settings.setValue(kStateKey, window.saveState(kStateVersion));
    settings.endGroup();
    settings.sync();
}

void WorkspacePersistence::ensureVisible(QMainWindow& window)
{
    recoverWidgetIfOffscreen(window);

    const auto docks = window.findChildren<QDockWidget*>();
    for (QDockWidget* dock : docks) {
        if (dock && dock->isFloating()) recoverWidgetIfOffscreen(*dock);
    }
}

} // namespace Skink::Ui::Workspace
