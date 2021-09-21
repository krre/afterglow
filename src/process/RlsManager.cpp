#include "RlsManager.h"
#include <QtCore>

static RlsManager* s_instance = nullptr;

RlsManager::RlsManager(QObject* parent) : ProcessManager(parent) {
    s_instance = this;
}

RlsManager::~RlsManager() {
    stop();
}

RlsManager* RlsManager::instance() {
    return s_instance;
}

void RlsManager::start() {
    s_instance->process()->start("rls", QStringList());
}

void RlsManager::initialize(const QString& projectPath) {
    start(); // TODO: Do not restart process on changing project

    QJsonObject capabilities = {
        {}
    };

    QJsonObject params = {
        { "rootUri", "file://" + projectPath },
        { "capabilities", capabilities }
    };

    s_instance->send("initialize", params);
}

void RlsManager::shutdown() {
    send("shutdown");
    s_instance->stop(); // TODO: Do not restart process on changing project
}

void RlsManager::setShowDebug(bool showDebug) {
    s_instance->showDebug = showDebug;
}

// Available methods:
// https://github.com/rust-lang-nursery/rls/blob/master/clients.md
void RlsManager::send(const QString& method, const QJsonObject& params) {
    int id = s_instance->counter++;
    s_instance->identifiers[id] = method;

    QJsonObject obj = {
        {"jsonrpc", "2.0"},
        { "id", id },
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

    if (s_instance->showDebug) {
        qDebug() << "RLS Message:" << message;
    }

    s_instance->process()->write(message.toUtf8());
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

    s_instance->send("textDocument/completion", params);
}

void RlsManager::onReadyReadStandardOutput(const QString& data) {
    if (showDebug) {
        qDebug() << "RLS Result:" << data;
    }

    QStringList rows = data.split("\r\n");
    if (rows.count() >= 3) {
        QJsonDocument doc = QJsonDocument::fromJson(rows.at(2).toUtf8());
        QJsonObject obj = doc.object();
        if (obj.contains("id")) {
            int id = obj["id"].toInt();
            if (identifiers.contains(id)) {
                QString method = identifiers.take(id);
                if (method == "textDocument/completion") {
                    emit completionResult(obj["result"].toArray());
                }
            }
        }
    }
}

void RlsManager::onReadyReadStandardError(const QString& data) {
    if (showDebug) {
        qDebug() << "RLS Error:" << data;
    }
}
