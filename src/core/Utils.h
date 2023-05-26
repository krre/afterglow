#pragma once
#include <QStringList>

class QListView;

class Utils {
public:
    static QStringList runConsoleCommand(const QString& command);
    static void runRustupCommand(const QStringList& arguments);
    static void loadAndFilterList(const QString& command, QListView* listView, const std::function<void(QStringList&)>& filter = nullptr);
    static void defaultInstalledFilter(QStringList& list);
};
