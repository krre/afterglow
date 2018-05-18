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

void RlsManager::initialize(const QString& projectPath) {
    QJsonObject capabilities = {
        {}
    };

    QJsonObject params = {
        { "rootPath", projectPath },
        { "capabilities", capabilities }
    };

    instance->send("initialize", params);
}

void RlsManager::shutdown() {
    send("shutdown");
}

void RlsManager::setShowDebug(bool showDebug) {
    instance->showDebug = showDebug;
}

void RlsManager::send(const QString& method, const QJsonObject& params) {
    QJsonObject obj = {
        {"jsonrpc", "2.0"},
        { "id", instance->counter++ },
        { "method", method },
    };


    if (!params.isEmpty()) {
        obj["params"] = params;
    }

    QJsonDocument doc;
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

void RlsManager::onReadyReadStandardOutput(const QString& data) {
    if (showDebug) {
        qDebug() << "RLS Result:" << data;
    }

    QJsonDocument doc = QJsonDocument::fromJson(data.split("\r\n").at(2).toUtf8());
    emit answer(doc.object());
}

void RlsManager::onReadyReadStandardError(const QString& data) {
    if (showDebug) {
        qDebug() << "RLS Error:" << data;
    }
}