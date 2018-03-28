#pragma once
#include <QCompleter>

class AutoCompleter : public QCompleter {
    Q_OBJECT

public:
    explicit AutoCompleter(const QStringList& list, QObject* parent = nullptr);
};
