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
    QJsonDocument doc(QJsonDocument::fromJson(file.readAll(), &err));
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse JSON file" << file.fileName();
        qWarning() << "Error:" << err.errorString() << "offset:" << err.offset;
        return;
    }

    syntaxJsonObjects[doc.object()["lang"].toObject()["extension"].toString()] = doc.object();
}

QJsonObject SyntaxHighlightManager::getSyntaxJson(const QString& ext) const {
    return syntaxJsonObjects.contains(ext) ? syntaxJsonObjects[ext] : QJsonObject();
}
