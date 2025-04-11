#include "RustInstaller.h"
#include "RustupTab.h"
#include "ToolchainTab.h"
#include "TargetTab.h"
#include "ComponentTab.h"
#include "OverrideTab.h"
#include "core/Settings.h"
#include "core/FileDownloader.h"
#include "core/Settings.h"
#include <QTabWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QProcess>

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

    m_fileDownloader = new FileDownloader(this);
    connect(m_fileDownloader, &FileDownloader::downloaded, this, &RustInstaller::onDownloaded);

    for (int i = 0; i < m_tabWidget->count(); ++i) {
        InstallerTab* tab = static_cast<InstallerTab*>(m_tabWidget->widget(i));
        tab->load();
    }

    readSettings();
}

RustInstaller::~RustInstaller() {
    writeSettings();
}

void RustInstaller::onBreakPushButtonClicked() {
    m_fileDownloader->abort();
    emit breakPressed();
}

CoTask RustInstaller::onDownloaded() {
    updateAllButtonsState();
    showAndScrollMessage(QString("Downloaded %1 bytes").arg(m_fileDownloader->data().size()));

    QString filePath = m_tmpDir.path() + "/" + "rustup-init.exe";
    QFile file(filePath);
    file.open(QIODevice::WriteOnly);
    file.write(m_fileDownloader->data());
    file.close();

    co_await runCommand(filePath, QStringList( "-y"));
    m_rustupTab->load();

    installDefaultComponents();
}

CoTask RustInstaller::runCommand(const QString& program, const QStringList& arguments, const QString& directory) {
    QString message = QString("<font color=%1>%2</font>: <font color=%3>%4 %5</font>")
                          .arg("#0000FF", tr("Run command"), "#FF0000", program, arguments.join(" "));
    showAndScrollMessage(message);

    QProcess process;
    process.setWorkingDirectory(directory);

    connect(this, &RustInstaller::breakPressed, [&] { process.close(); });

    CoAwaiter awaiter{};

    connect(&process, &QProcess::readyReadStandardOutput, this, [&, this] {
        QByteArray data = process.readAllStandardOutput();
        auto toUtf16 = QStringDecoder(QStringDecoder::Utf8);
        QString output = toUtf16(data);
        showAndScrollMessage(output);
    });

    connect(&process, &QProcess::readyReadStandardError, this, [&, this] {
        QByteArray data = process.readAllStandardError();
        auto toUtf16 = QStringDecoder(QStringDecoder::Utf8);
        QString output = toUtf16(data);
        showAndScrollMessage(output);
    });

    connect(&process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this, [&, this] (int, QProcess::ExitStatus) {
        QString message = QString("<font color=%1>%2</font>").arg("#0000FF", tr("Command finished successfully"));
        showAndScrollMessage(message);
        awaiter.resume();
    });

    connect(&process, &QProcess::errorOccurred, this, [&, this] (QProcess::ProcessError error [[maybe_unused]]) {
        QString message = QString("<font color=%1>%2</font>").arg("#0000FF", tr("Command finished with error"));
        showAndScrollMessage(message);
        awaiter.resume();
    });

    connect(&process, &QProcess::stateChanged, this, [this] (QProcess::ProcessState newState) {
        updateAllButtonsState(newState == QProcess::Running);
    });

    process.start(program, arguments);
    co_await awaiter;
    co_return;
}

void RustInstaller::loadComponents() {
    m_componentTab->load();
}

void RustInstaller::showAndScrollMessage(const QString message) {
    m_consolePlainTextEdit->appendHtml(message);
    m_consolePlainTextEdit->verticalScrollBar()->setValue(m_consolePlainTextEdit->verticalScrollBar()->maximum());
}

CoTask RustInstaller::installDefaultComponents() {
    co_await runCommand("rustup", QStringList("component") << "add" << "rls-preview" << "rust-analysis" << "rust-src");
    co_await runCommand("cargo", QStringList("install") << "racer");
}

void RustInstaller::updateAllButtonsState(bool isProcessRunning) {
    bool processesFree = !isProcessRunning && !m_fileDownloader->isBusy();
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

CoTask RustInstaller::downloadInstaller() {
#if defined(Q_OS_LINUX)
    co_await runCommand("sh", QStringList("-c") << "curl https://sh.rustup.rs -sSf | sh -s -- -y");
    m_rustupTab->load();

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
