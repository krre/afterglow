#pragma once
#include "core/Singleton.h"
#include <QObject>
#include <QMap>
#include <QJsonObject>

class Highlighter;

class SyntaxHighlightManager : public QObject, public Singleton<SyntaxHighlightManager> {
public:
    explicit SyntaxHighlightManager(QObject* parent = nullptr);
    ~SyntaxHighlightManager();

    void addSyntaxFile(const QString& path);
    QJsonObject getSyntaxJson(const QString& ext) const;
    bool hasExtension(const QString& ext) const { return syntaxJsonObjects.contains(ext); }

private:
    QMap<QString, QJsonObject> syntaxJsonObjects;
};
