#include "Utils.h"
#include <QtWidgets>

QStringList Utils::runConsoleCommand(const QString& command) {
    QProcess process;
    QByteArray output;

    QObject::connect(&process, &QProcess::readyReadStandardOutput, [&] () {
        output += process.readAllStandardOutput();
    });

    QStringList arguments = command.split(" ");
    QString program = arguments.first();
    arguments.removeFirst();
    process.start(program, arguments);
    process.waitForFinished();

    QStringList result;

    for (auto& part : output.split('\n')) {
        if (part.isEmpty()) continue;
        result.append(part);
    }

    return result;
}

QStringList Utils::selectedRowsFromListView(QListView* listView, bool removeDefaultSuffix) {
    QStringList list;
    QModelIndexList indices = listView->selectionModel()->selectedIndexes();

    for (int i = 0; i < indices.count(); i++) {
        list.append(listView->model()->data(indices.at(i), Qt::DisplayRole).toString());
    }

    if (removeDefaultSuffix) {
        list.replaceInStrings(" (default)", "");
    }

    list.replaceInStrings(" ", "");

    return list;
}

void Utils::copySelectedRowsFromListViewToClipboard(QListView* listView) {
    QStringList list = Utils::selectedRowsFromListView(listView);
    list.replaceInStrings(" ", "");
    QClipboard* clipboard = QGuiApplication::clipboard();
    clipboard->setText(list.join('\n'));
}

void Utils::runRustupCommand(const QStringList& arguments) {
    QProcess process;
    process.start("rustup", arguments);
    process.waitForFinished();
}
