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

// Available methods:
// https://github.com/rust-lang-nursery/rls/blob/master/clients.md
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

void RlsManager::completion(const QString& filename, int row, int column) {
    QJsonObject textDocument = {
        { "uri", "file://" + filename }
    };

    QJsonObject position = {
        { "line", row },
        { "character", column }
    };

    QJsonObject params = {
        { "textDocument", textDocument },
        { "position", position }
    };

    instance->send("textDocument/completion", params);
}

void RlsManager::onReadyReadStandardOutput(const QString& data) {
    if (showDebug) {
        qDebug() << "RLS Result:" << data;
    }

    QStringList rows = data.split("\r\n");
    if (rows.count() >= 3) {
        QJsonDocument doc = QJsonDocument::fromJson(rows.at(2).toUtf8());
        emit answer(doc.object());
    }
}

void RlsManager::onReadyReadStandardError(const QString& data) {
    if (showDebug) {
        qDebug() << "RLS Error:" << data;
    }
}
