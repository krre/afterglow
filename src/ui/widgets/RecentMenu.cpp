#include "RecentMenu.h"
#include "core/Constants.h"

constexpr int SeparatorAndMenuClearCount = 2;

RecentMenu::RecentMenu(const QString& title, QWidget* parent) : QMenu(title, parent) {
    addSeparator();
    addAction(tr("Clear Menu"), this, &RecentMenu::clear);
}

void RecentMenu::addRecent(const QString& recent) {
    const auto acts = actions();

        for (auto action : acts) {
        if (action->text() == recent) {
            removeAction(action);
        }
    }

    auto action = new QAction(recent);
    connect(action, &QAction::triggered, this, [recent, this] { emit activated(recent); });
    insertAction(actions().constFirst(), action);

    if (actions().size() > Const::Window::MaxRecentFiles + SeparatorAndMenuClearCount) {
        removeAction(actions().at(actions().size() - SeparatorAndMenuClearCount - 1));
    }

    updateState();
}

QStringList RecentMenu::recents() const {
    QStringList result;

    for (int i = 0; i < actions().size() - SeparatorAndMenuClearCount; ++i) {
        result.append(actions().at(i)->text());
    }

    return result;
}

void RecentMenu::updateState() {
    menuAction()->setEnabled(actions().size() > SeparatorAndMenuClearCount);
}

void RecentMenu::clear() {
    for (int i = actions().size() - SeparatorAndMenuClearCount - 1; i >= 0; --i) {
        removeAction(actions().at(i));
    }

    updateState();
}
