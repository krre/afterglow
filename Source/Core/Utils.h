#pragma once
#include <QStringList>

class QListView;

class Utils {

public:
    static QStringList listFromConsole(const QString& command);
    static QStringList getSelectedRowsFromListView(QListView* listView);
};
