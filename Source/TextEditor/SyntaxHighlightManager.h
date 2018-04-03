#pragma once
#include "Core/Singleton.h"
#include <QObject>
#include <QVector>

class Highlighter;

class SyntaxHighlightManager : public QObject, public Singleton<SyntaxHighlightManager> {
    Q_OBJECT

public:
    explicit SyntaxHighlightManager(QObject* parent = nullptr);
    ~SyntaxHighlightManager();

private:
    QVector<Highlighter*> highlighters;
};
