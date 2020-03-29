#pragma once
#include <QtGlobal>

template <typename T> class Singleton {

public:
    Singleton() {
        Q_ASSERT(s_instance == nullptr && "Singleton object should be initialized only once");
        s_instance = static_cast<T*>(this);
    }

    virtual ~Singleton() {
        s_instance = nullptr;
    }

    static T* instance() {
        return s_instance;
    }

    static void release() {
        if (s_instance != nullptr) {
            delete s_instance;
        }
    }

private:
    static T* s_instance;
};

template <typename T> T* Singleton<T>::s_instance = nullptr;
