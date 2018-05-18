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

    QJsonDocument doc;

    QJsonObject capabilities = {
        {}
    };

    QJsonObject params = {
        { "capabilities", capabilities }
    };

    QJsonObject obj = {
        {"jsonrpc", "2.0"},
        { "id", instance->counter++ },
        { "method", "initialize" },
        { "params", params }
    };

    doc.setObject(obj);
    QString jsonrpc = doc.toJson(QJsonDocument::Compact);

    QString message = QString("Content-Length: %1\r\n").arg(jsonrpc.size());
    message += "\r\n";
    message += jsonrpc;

    if (instance->showDebug) {
        qDebug() << "RLS Message:" << message;
    }

    instance->getProcess()->write(message.toUtf8());
}

void RlsManager::setShowDebug(bool showDebug) {
    instance->showDebug = showDebug;
}

void RlsManager::onReadyReadStandardOutput(const QString& data) {
    if (showDebug) {
        qDebug() << "RLS Result:" << data;
    }
}

void RlsManager::onReadyReadStandardError(const QString& data) {
    if (showDebug) {
        qDebug() << "RLS Error:" << data;
    }
}
