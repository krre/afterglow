#include "SyntaxHighlightManager.h"
#include <QtCore>

SyntaxHighlightManager::SyntaxHighlightManager(QObject* parent) : QObject(parent) {
    addSyntaxFile(":/resources/highlighting/rust.json");
}

SyntaxHighlightManager::~SyntaxHighlightManager() {

}

void SyntaxHighlightManager::addSyntaxFile(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file" << file.fileName();
        return;
    }

    QJsonParseError err;
    QJsonObject syntax = QJsonDocument::fromJson(file.readAll(), &err).object();

    if (err.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse JSON file" << file.fileName();
        qWarning() << "Error:" << err.errorString() << "offset:" << err.offset;
        return;
    }

    m_syntaxJsonObjects[syntax["lang"].toObject()["extension"].toString()] = syntax;
}

QJsonObject SyntaxHighlightManager::getSyntaxJson(const QString& ext) const {
    return m_syntaxJsonObjects.contains(ext) ? m_syntaxJsonObjects[ext] : QJsonObject();
}
