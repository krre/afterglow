#include "ActionManager.h"
#include <QtWidgets>

static ActionManager* instance = nullptr;

ActionManager::ActionManager(QObject* parent) : QObject(parent) {
    instance = this;
}

ActionManager::~ActionManager() {

}

ActionManager* ActionManager::getInstance() {
    return instance;
}

void ActionManager::addAction(const QString& id, QAction* action) {
    instance->actions[id] = action;
}

QAction* ActionManager::action(const QString& id) {
    return instance->actions.value(id);
}

void ActionManager::removeAction(const QString& id) {
    instance->actions.remove(id);
}
