#pragma once
#include <QStringList>

class QListView;

class Utils {

public:
    static QStringList getListFromConsole(const QString& command);
    static QStringList getSelectedRowsFromListView(QListView* listView, bool removeDefaultSuffix = true);
    static void copySelectedRowsFromListViewToClipboard(QListView* listView);
    static void runRustupCommand(const QStringList& arguments);
};
