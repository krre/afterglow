#pragma once
#include <QtGlobal>
#include <QObject>

template <typename T> class Singleton : public QObject {

public:
    Singleton(QObject* parent = nullptr) : QObject(parent) {
        Q_ASSERT(instance == nullptr && "Singleton object should be initialized only once");
        instance = static_cast<T*>(this);
    }

    virtual ~Singleton() {
        instance = nullptr;
    }

    static T* getInstance() {
        return instance;
    }

    static void release() {
        if (instance != nullptr) {
            delete instance;
            instance = nullptr;
        }
    }

private:
    static T* instance;
};

template <typename T> T* Singleton<T>::instance = nullptr;
