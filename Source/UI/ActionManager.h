#pragma once
#include <QObject>

class ActionManager : public QObject {
    Q_OBJECT
public:
    explicit ActionManager(QObject* parent = nullptr);
    ~ActionManager();

    static ActionManager* getInstance();
};
