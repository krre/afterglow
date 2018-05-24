#include "Utils.h"
#include <QtWidgets>


QStringList Utils::getListFromConsole(const QString& command) {
    QProcess process;
    QStringList list;
    QObject::connect(&process, &QProcess::readyReadStandardOutput, [&] () {
        QString output = process.readAllStandardOutput();
        list << output.split("\n");
        // Cleanup last empty string
        if (list.count() && list.last().isEmpty()) {
            list.removeLast();
        }
    });

    QStringList arguments = command.split(" ");
    QString program = arguments.at(0);
    arguments.removeFirst();
    process.start(program, arguments);
    process.waitForFinished();

    return list;
}

QStringList Utils::getSelectedRowsFromListView(QListView* listView, bool removeDefaultSuffix) {
    QStringList list;

    QModelIndexList indices = listView->selectionModel()->selectedIndexes();
    for (int i = 0; i < indices.count(); i++) {
        list.append(listView->model()->data(indices.at(i), Qt::DisplayRole).toString());
    }

    if (removeDefaultSuffix) {
        list.replaceInStrings(" (default)", "");
    }

    return list;
}
