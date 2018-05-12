#include "ActionManager.h"
#include <QtCore>

static ActionManager* instance = nullptr;

ActionManager::ActionManager(QObject* parent) : QObject(parent) {
    instance = this;
}

ActionManager::~ActionManager() {

}

ActionManager* ActionManager::getInstance() {
    return instance;
}
