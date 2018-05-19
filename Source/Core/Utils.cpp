#include "Utils.h"
#include <QtCore>


QStringList Utils::listFromConsole(const QString& command) {
    QProcess process;
    QStringList list;
    QObject::connect(&process, QProcess::readyReadStandardOutput, [&] () {
       list << process.readAllStandardOutput();
    });

    QStringList arguments = command.split(" ");
    QString program = arguments.at(0);
    arguments.removeFirst();
    process.start(program, arguments);
    process.waitForFinished();

    return list;
}
