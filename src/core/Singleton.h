#pragma once
#include <QtGlobal>

template <typename T> class Singleton {

public:
    Singleton() {
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
        }
    }

private:
    static T* instance;
};

template <typename T> T* Singleton<T>::instance = nullptr;
