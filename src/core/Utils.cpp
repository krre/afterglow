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

void Utils::runRustupCommand(const QStringList& arguments) {
    QProcess process;
    process.start("rustup", arguments);
    process.waitForFinished();
}
