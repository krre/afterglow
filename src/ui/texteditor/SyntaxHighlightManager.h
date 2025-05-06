#pragma once
#include <QObject>
#include <QMap>
#include <QJsonObject>

class Highlighter;

class SyntaxHighlightManager : public QObject {
public:
    explicit SyntaxHighlightManager(QObject* parent = nullptr);

    void addSyntaxFile(const QString& path);
    QJsonObject syntaxJson(const QString& ext) const;
    bool hasExtension(const QString& ext) const { return m_syntaxJsonObjects.contains(ext); }

private:
    QMap<QString, QJsonObject> m_syntaxJsonObjects;
};
