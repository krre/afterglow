#include "RlsManager.h"
#include <QtCore>

static RlsManager* instance = nullptr;

RlsManager::RlsManager(QObject* parent) : ProcessManager(parent) {
    instance = this;
}

RlsManager::~RlsManager() {
    stop();
}

RlsManager* RlsManager::getInstance() {
    return instance;
}

void RlsManager::start() {
    instance->getProcess()->start("rls");
}

void RlsManager::onReadyReadStandardOutput(const QString& data) {
    qDebug() << "output" << data;
}

void RlsManager::onReadyReadStandardError(const QString& data) {
    qDebug() << "error" << data;
}
