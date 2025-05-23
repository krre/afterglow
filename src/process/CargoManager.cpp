#include "CargoManager.h"
#include "ui/ProjectProperties.h"
#include "core/Settings.h"
#include "core/Constants.h"

CargoManager::CargoManager(ProjectProperties* projectProperties, QObject* parent)
    : ProcessManager(parent), m_projectProperties(projectProperties) {
}

CargoManager::~CargoManager() {

}

void CargoManager::createProject(Target target, const QString& path) {
    QStringList arguments("new");

    if (target == Target::Binary) {
        arguments << "--bin";
    } else if (target == Target::Library) {
        arguments << "--lib";
    }

    arguments << path;
    m_commandStatus = CommandStatus::New;
    prepareAndStart(arguments);
    setProjectPath(path);
}

void CargoManager::build() {
    QStringList arguments("build");
    addBuildRunArguments(arguments);
    m_commandStatus = CommandStatus::Build;
    prepareAndStart(arguments);
}

void CargoManager::run() {
    QStringList arguments("run");
    addBuildRunArguments(arguments);
    m_commandStatus = CommandStatus::Run;
    prepareAndStart(arguments);
}

void CargoManager::check() {
    prepareAndStart({ "check" });
}

void CargoManager::clean() {
    prepareAndStart({ "clean" });
}

void CargoManager::doc() {
    prepareAndStart({ "doc", "--open" });
}

void CargoManager::setProjectPath(const QString& path) {
    m_projectPath = path;
    process()->setWorkingDirectory(path);
}

QString CargoManager::projectPath() const {
    return m_projectPath;
}

void CargoManager::onReadyReadStandardOutput(const QString& data) {
    emit consoleMessage(data);
}

void CargoManager::onReadyReadStandardError(const QString& data) {
    emit consoleMessage(data);
}

void CargoManager::onFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    switch (m_commandStatus) {
        case CommandStatus::New:
            emit projectCreated(process()->arguments().last());
            break;
        default:
            break;
    }

    QString message = QString("%1 %2 with code %3")
            .arg(process()->program(), exitStatus == QProcess::NormalExit ? "finished" : "crashed")
            .arg(exitCode);
    coloredOutputMessage(message);
}

void CargoManager::prepareAndStart(const QStringList& arguments) {
    QString cargoPath = Settings::value("cargo.path").toString();
    process()->setProgram(cargoPath.isEmpty() ? "cargo" : cargoPath);
    process()->setArguments(arguments);

    QString message = process()->program();

    for (const auto& argument : arguments) {
        message += " " + argument;
    }

    message += "<br>";

    coloredOutputMessage(message, true);
    process()->start();
}

void CargoManager::addBuildRunArguments(QStringList& arguments) {
    if (Settings::value("debug.compilerMessageFormat").toString() == "json") {
        arguments << "--message-format";
        arguments << "json";
    }

    if (m_projectProperties->buildTarget() == BuildTarget::Release) {
        arguments << "--release";
    }
}

void CargoManager::coloredOutputMessage(const QString& message, bool start) {
    QString coloredMessage = QString("<font color=%1>%2</font>")
            .arg(Const::Color::Message, message);
    emit consoleMessage(coloredMessage, true, start);
}
