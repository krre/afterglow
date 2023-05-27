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

RustInstaller::RustInstaller(QWidget* parent) : StandardDialog(parent) {
    setWindowTitle(tr("Rust Installer"));

    m_rustupTab = new RustupTab(this);
    m_toolchainTab = new ToolchainTab(this);

    connect(m_toolchainTab, &ToolchainTab::defaultSetted, [this] {
        m_targetTab->load();
        m_componentTab->load();
        m_rustupTab->load();
    });

    m_targetTab = new TargetTab(this);
    m_componentTab = new ComponentTab(this);
    m_overrideTab = new OverrideTab(this);

    m_tabWidget = new QTabWidget;
    m_tabWidget->addTab(m_rustupTab, "Rustup");
    m_tabWidget->addTab(m_toolchainTab, tr("Toolchains"));
    m_tabWidget->addTab(m_targetTab, tr("Targets"));
    m_tabWidget->addTab(m_componentTab, tr("Components"));
    m_tabWidget->addTab(m_overrideTab, tr("Overrides"));

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(m_tabWidget);

    m_consolePlainTextEdit = new QPlainTextEdit;
    m_consolePlainTextEdit->setReadOnly(true);
    verticalLayout->addWidget(m_consolePlainTextEdit, 1);

    m_breakPushButton = new QPushButton(tr("Break"));
    m_breakPushButton->setEnabled(false);
    connect(m_breakPushButton, &QPushButton::clicked, this, &RustInstaller::onBreakPushButtonClicked);
    verticalLayout->addWidget(m_breakPushButton, 0, Qt::AlignLeft);

    setContentLayout(verticalLayout, false);
    resizeToWidth(810);
    buttonBox()->setStandardButtons(QDialogButtonBox::Close);

    m_process = new QProcess(this);

    connect(m_process, &QProcess::readyReadStandardOutput, this, [this] {
        QByteArray data = m_process->readAllStandardOutput();
        auto toUtf16 = QStringDecoder(QStringDecoder::Utf8);
        QString output = toUtf16(data);
        showAndScrollMessage(output);
    });

    connect(m_process, &QProcess::readyReadStandardError, this, [this] {
        QByteArray data = m_process->readAllStandardError();
        auto toUtf16 = QStringDecoder(QStringDecoder::Utf8);
        QString output = toUtf16(data);
        showAndScrollMessage(output);
    });

    connect(m_process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this, [=, this] (int, QProcess::ExitStatus) {
        QString message = QString("<font color=%1>%2</font>").arg("#0000FF", tr("Command finished successfully"));
        showAndScrollMessage(message);

        if (m_commandQueue.count()) {
            Command command = m_commandQueue.dequeue();
            if (command.postWork) {
                command.postWork();
            }

            runFromQueue();
        }
    });

    connect(m_process, &QProcess::errorOccurred, this, [this] (QProcess::ProcessError error) {
        Q_UNUSED(error)
        QString message = QString("<font color=%1>%2</font>").arg("#0000FF", tr("Command finished with error"));
        showAndScrollMessage(message);
        m_commandQueue.clear();
    });

    connect(m_process, &QProcess::stateChanged, this, &RustInstaller::onProcessStateChanged);

    m_fileDownloader = new FileDownloader(this);
    connect(m_fileDownloader, &FileDownloader::downloaded, this, &RustInstaller::onDownloaded);

    for (int i = 0; i < m_tabWidget->count(); i++) {
        InstallerTab* tab = static_cast<InstallerTab*>(m_tabWidget->widget(i));
        tab->load();
    }

    readSettings();
}

RustInstaller::~RustInstaller() {
    writeSettings();
}

void RustInstaller::onBreakPushButtonClicked() {
    m_commandQueue.clear();
    m_fileDownloader->abort();
    m_process->close();
}

void RustInstaller::onDownloaded() {
    updateAllButtonsState();
    showAndScrollMessage(QString("Downloaded %1 bytes").arg(m_fileDownloader->data().size()));

    QString filePath = m_tmpDir.path() + "/" + "rustup-init.exe";
    QFile file(filePath);
    file.open(QIODevice::WriteOnly);
    file.write(m_fileDownloader->data());
    file.close();

    runCommand(filePath, { "-y" }, [this] {
        m_rustupTab->load();
    });

    installDefaultComponents();
}

void RustInstaller::onProcessStateChanged(QProcess::ProcessState newState) {
    if (newState == QProcess::Running || newState == QProcess::NotRunning) {
        updateAllButtonsState();
    }
}

void RustInstaller::runCommand(const QString& program, const QStringList& arguments, const std::function<void()>& postWork, const QString& directory) {
    m_commandQueue.enqueue({ program, arguments, postWork, directory });
    runFromQueue();
}

void RustInstaller::loadComponents() {
    m_componentTab->load();
}

void RustInstaller::showAndScrollMessage(const QString message) {
    m_consolePlainTextEdit->appendHtml(message);
    m_consolePlainTextEdit->verticalScrollBar()->setValue(m_consolePlainTextEdit->verticalScrollBar()->maximum());
}

void RustInstaller::runFromQueue() {
    if (!m_commandQueue.isEmpty() && m_process->state() == QProcess::NotRunning) {
        if (m_consolePlainTextEdit->document()->blockCount() > 1) {
            showAndScrollMessage("");
        }

        Command command = m_commandQueue.head();
        QString message = QString("<font color=%1>%2</font>: <font color=%3>%4 %5</font>")
                    .arg("#0000FF", tr("Run command"), "#FF0000", command.program, command.arguments.join(" "));
        showAndScrollMessage(message);
        m_process->setWorkingDirectory(command.directory);
        m_process->start(command.program, command.arguments);
    }
}

void RustInstaller::installDefaultComponents() {
    runCommand("rustup", { "component", "add", "rls-preview", "rust-analysis", "rust-src" });
    runCommand("cargo", { "install", "racer" });
}

void RustInstaller::updateAllButtonsState() {
    bool processesFree = m_process->state() == QProcess::NotRunning && !m_fileDownloader->isBusy();
    m_breakPushButton->setEnabled(!processesFree);
    m_rustupTab->setWidgetsEnabled(processesFree);
    m_toolchainTab->setWidgetsEnabled(processesFree);
    m_targetTab->setWidgetsEnabled(processesFree);
    m_componentTab->setWidgetsEnabled(processesFree);
    m_overrideTab->setWidgetsEnabled(processesFree);
}

void RustInstaller::cleanupTarget(QStringList& components) const {
    if (!m_targetTab->defaultTarget().isEmpty()) {
        QString search = "-" + m_targetTab->defaultTarget();
        components.replaceInStrings(search, "");
    }
}

void RustInstaller::downloadInstaller() {
#if defined(Q_OS_LINUX)
    runCommand("sh", { "-c", "curl https://sh.rustup.rs -sSf | sh -s -- -y" }, [this] {
        m_rustupTab->load();
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
    Settings::setValue("gui.rustInstaller.currentTab", m_tabWidget->currentIndex());
    Settings::updateRustEnvironmentVariables();
}
