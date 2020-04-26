#pragma once
#include <QObject>
#include <QMap>

class QAction;

class ActionManager : public QObject {
    Q_OBJECT
public:
    explicit ActionManager(QObject* parent = nullptr);
    ~ActionManager();

    static ActionManager* instance();

    static void addAction(const QString& id, QAction* action);
    static QAction* action(const QString& id);
    static void removeAction(const QString& id);

private:
    QMap<QString, QAction*> actions;
};
