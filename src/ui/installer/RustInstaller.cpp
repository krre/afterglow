#include "RustInstaller.h"
#include "RustupTab.h"
#include "ToolchainTab.h"
#include "TargetTab.h"
#include "ComponentTab.h"
#include "OverrideTab.h"
#include "core/Settings.h"
#include "core/FileDownloader.h"
#include "core/Settings.h"
#include <QtWidgets>

using namespace std::placeholders;

RustInstaller::RustInstaller(QWidget* parent) : StandardDialog(parent) {
    setWindowTitle(tr("Rust Installer"));

    rustupTab = new RustupTab(this);
    toolchainTab = new ToolchainTab(this);

    connect(toolchainTab, &ToolchainTab::defaultSetted, [this] {
        targetTab->load();
        componentTab->load();
        rustupTab->load();
    });

    targetTab = new TargetTab(this);
    componentTab = new ComponentTab(this);
    overrideTab = new OverrideTab(this);

    tabWidget = new QTabWidget;
    tabWidget->addTab(rustupTab, "Rustup");
    tabWidget->addTab(toolchainTab, tr("Toolchains"));
    tabWidget->addTab(targetTab, tr("Targets"));
    tabWidget->addTab(componentTab, tr("Components"));
    tabWidget->addTab(overrideTab, tr("Overrides"));

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(tabWidget);

    consolePlainTextEdit = new QPlainTextEdit;
    consolePlainTextEdit->setReadOnly(true);
    verticalLayout->addWidget(consolePlainTextEdit, 1);

    breakPushButton = new QPushButton(tr("Break"));
    breakPushButton->setEnabled(false);
    connect(breakPushButton, &QPushButton::clicked, this, &RustInstaller::onBreakPushButtonClicked);
    verticalLayout->addWidget(breakPushButton, 0, Qt::AlignLeft);

    setContentLayout(verticalLayout, false);
    resizeToWidth(810);
    buttonBox()->setStandardButtons(QDialogButtonBox::Close);

    process = new QProcess(this);

    connect(process, &QProcess::readyReadStandardOutput, this, [this] {
        QByteArray data = process->readAllStandardOutput();
        auto toUtf16 = QStringDecoder(QStringDecoder::Utf8);
        QString output = toUtf16(data);
        showAndScrollMessage(output);
    });

    connect(process, &QProcess::readyReadStandardError, this, [this] {
        QByteArray data = process->readAllStandardError();
        auto toUtf16 = QStringDecoder(QStringDecoder::Utf8);
        QString output = toUtf16(data);
        showAndScrollMessage(output);
    });

    connect(process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this, [=, this] (int, QProcess::ExitStatus) {
        QString message = QString("<font color=%1>%2</font>").arg("#0000FF", tr("Command finished successfully"));
        showAndScrollMessage(message);

        if (commandQueue.count()) {
            Command command = commandQueue.dequeue();
            if (command.postWork) {
                command.postWork();
            }

            runFromQueue();
        }
    });

    connect(process, &QProcess::errorOccurred, this, [this] (QProcess::ProcessError error) {
        Q_UNUSED(error)
        QString message = QString("<font color=%1>%2</font>").arg("#0000FF", tr("Command finished with error"));
        showAndScrollMessage(message);
        commandQueue.clear();
    });

    connect(process, &QProcess::stateChanged, this, &RustInstaller::onProcessStateChainged);

    fileDownloader = new FileDownloader(this);
    connect(fileDownloader, &FileDownloader::downloaded, this, &RustInstaller::onDownloaded);

    for (int i = 0; i < tabWidget->count(); i++) {
        InstallerTab* tab = static_cast<InstallerTab*>(tabWidget->widget(i));
        tab->load();
    }

    readSettings();
}

RustInstaller::~RustInstaller() {
    writeSettings();
}

void RustInstaller::onBreakPushButtonClicked() {
    commandQueue.clear();
    fileDownloader->abort();
    process->close();
}

void RustInstaller::onDownloaded() {
    updateAllButtonsState();
    showAndScrollMessage(QString("Downloaded %1 bytes").arg(fileDownloader->data().size()));

    QString filePath = tmpDir.path() + "/" + "rustup-init.exe";
    QFile file(filePath);
    file.open(QIODevice::WriteOnly);
    file.write(fileDownloader->data());
    file.close();

    runCommand(filePath, { "-y" }, [this] {
        rustupTab->load();
    });

    installDefaultComponents();
}

void RustInstaller::onProcessStateChainged(QProcess::ProcessState newState) {
    if (newState == QProcess::Running || newState == QProcess::NotRunning) {
        updateAllButtonsState();
    }
}

void RustInstaller::runCommand(const QString& program, const QStringList& arguments, const std::function<void()>& postWork, const QString& directory) {
    commandQueue.enqueue({ program, arguments, postWork, directory });
    runFromQueue();
}

void RustInstaller::loadComponents() {
    componentTab->load();
}

void RustInstaller::showAndScrollMessage(const QString message) {
    consolePlainTextEdit->appendHtml(message);
    consolePlainTextEdit->verticalScrollBar()->setValue(consolePlainTextEdit->verticalScrollBar()->maximum());
}

void RustInstaller::runFromQueue() {
    if (!commandQueue.isEmpty() && process->state() == QProcess::NotRunning) {
        if (consolePlainTextEdit->document()->blockCount() > 1) {
            showAndScrollMessage("");
        }

        Command command = commandQueue.head();
        QString message = QString("<font color=%1>%2</font>: <font color=%3>%4 %5</font>")
                    .arg("#0000FF", tr("Run command"), "#FF0000", command.program, command.arguments.join(" "));
        showAndScrollMessage(message);
        process->setWorkingDirectory(command.directory);
        process->start(command.program, command.arguments);
    }
}

void RustInstaller::installDefaultComponents() {
    runCommand("rustup", { "component", "add", "rls-preview", "rust-analysis", "rust-src" });
    runCommand("cargo", { "install", "racer" });
}

void RustInstaller::updateAllButtonsState() {
    bool processesFree = process->state() == QProcess::NotRunning && !fileDownloader->isBusy();
    breakPushButton->setEnabled(!processesFree);
    rustupTab->setWidgetsEnabled(processesFree);
    toolchainTab->setWidgetsEnabled(processesFree);
    targetTab->setWidgetsEnabled(processesFree);
    componentTab->setWidgetsEnabled(processesFree);
    overrideTab->setWidgetsEnabled(processesFree);
}

void RustInstaller::cleanupTarget(QStringList& components) const {
    if (!targetTab->defaultTarget().isEmpty()) {
        QString search = "-" + targetTab->defaultTarget();
        components.replaceInStrings(search, "");
    }
}

void RustInstaller::downloadInstaller() {
#if defined(Q_OS_LINUX)
    runCommand("sh", { "-c", "curl https://sh.rustup.rs -sSf | sh -s -- -y" }, [this] {
        rustupTab->load();
    });

    installDefaultComponents();
#elif defined(Q_OS_WIN)
    QUrl url("https://static.rust-lang.org/rustup/dist/x86_64-pc-windows-gnu/rustup-init.exe");
    showAndScrollMessage("Download " + url.toString());
    fileDownloader->load(url);
    updateAllButtonsState();
#endif
}

void RustInstaller::readSettings() {
    // TODO: Leads to disappearance tabs in Qt 6.5.1
//    tabWidget->setCurrentIndex(Settings::value("gui.rustInstaller.currentTab").toInt());
}

void RustInstaller::writeSettings() {
    Settings::setValue("gui.rustInstaller.currentTab", tabWidget->currentIndex());
    Settings::updateRustEnvironmentVariables();
}
