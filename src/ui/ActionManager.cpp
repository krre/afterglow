#include "ActionManager.h"

static ActionManager* s_instance = nullptr;

ActionManager::ActionManager(QObject* parent) : QObject(parent) {
    s_instance = this;
}

ActionManager::~ActionManager() {

}

ActionManager* ActionManager::instance() {
    return s_instance;
}

void ActionManager::addAction(const QString& id, QAction* action) {
    s_instance->m_actions[id] = action;
}

QAction* ActionManager::action(const QString& id) {
    return s_instance->m_actions.value(id);
}

void ActionManager::removeAction(const QString& id) {
    s_instance->m_actions.remove(id);
}
