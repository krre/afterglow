#include "Utils.h"
#include "ui/StringListModel.h"
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

void Utils::runRustupCommand(const QStringList& arguments) {
    QProcess process;
    process.start("rustup", arguments);
    process.waitForFinished();
}

void Utils::loadAndFilterList(const QString& command, QListView* listView, const std::function<void (QStringList&)>& filter) {
    QStringList list = runConsoleCommand(command);

    if (list.count() == 1 && list.first().left(2) == "no") {
        list.removeFirst();
    }

    if (filter) {
        filter(list);
    }

    if (list.count()) {
        StringListModel* model = static_cast<StringListModel*>(listView->model());
        model->setStrings(list);
        listView->setCurrentIndex(model->index(0, 0));
    }
}

QString Utils::findDefault(QListView* listView) {
    StringListModel* model = static_cast<StringListModel*>(listView->model());
    auto str = model->find("(default)");
    return str == std::nullopt ? QString() : str->replace(" (default)", "");
}

void Utils::defaultInstalledFilter(QStringList& list) {
    for (int i = list.count() - 1; i >= 0; i--) {
        if (list.at(i).contains("(default)")) {
            continue;
        } else if (list.at(i).contains("(installed)")) {
            QString value = list.at(i);
            list[i] = value.replace("(installed)", "");
        } else {
            list.removeAt(i);
        }
    }
}
