#pragma once
#include <QStringList>

class QListView;

class Utils {
public:
    static QStringList listFromConsole(const QString& command);
    static QStringList selectedRowsFromListView(QListView* listView, bool removeDefaultSuffix = true);
    static void copySelectedRowsFromListViewToClipboard(QListView* listView);
    static void runRustupCommand(const QStringList& arguments);
};
