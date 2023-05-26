#include "RustInstaller.h"
#include "SelectableListView.h"
#include "RustupTab.h"
#include "ToolchainTab.h"
#include "TargetTab.h"
#include "SetOverride.h"
#include "AddComponentOrTarget.h"
#include "ui/StringListModel.h"
#include "core/Utils.h"
#include "core/Settings.h"
#include "core/FileDownloader.h"
#include "core/Settings.h"
#include <QtWidgets>

using namespace std::placeholders;

RustInstaller::RustInstaller(QWidget* parent) : StandardDialog(parent) {
    setWindowTitle(tr("Rust Installer"));

    rustupTab = new RustupTab;
    connect(rustupTab, &RustupTab::downloadClicked, this, &RustInstaller::rustupDownload);
    connect(rustupTab, &RustupTab::updateClicked, this, &RustInstaller::rustupUpdate);
    connect(rustupTab, &RustupTab::updateAllClicked, this, &RustInstaller::rustupUpdateAll);
    connect(rustupTab, &RustupTab::uninstallClicked, this, &RustInstaller::rustupUninstall);

    toolchainTab = new ToolchainTab(this);

    connect(toolchainTab, &ToolchainTab::defaultSetted, [this] {
        targetTab->loadList();
        loadComponentList();
        rustupTab->loadVersion();
    });

    targetTab = new TargetTab(this);

    tabWidget = new QTabWidget;
    tabWidget->addTab(rustupTab, "Rustup");
    tabWidget->addTab(toolchainTab, tr("Toolchains"));
    tabWidget->addTab(targetTab, tr("Targets"));

    createComponentsTab();
    createOverridesTab();

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(tabWidget);

    consolePlainTextEdit = new QPlainTextEdit;
    consolePlainTextEdit->setReadOnly(true);
    verticalLayout->addWidget(consolePlainTextEdit, 1);

    breakPushButton = new QPushButton(tr("Break"));
    breakPushButton->setEnabled(false);
    verticalLayout->addWidget(breakPushButton, 0, Qt::AlignLeft);

    setContentLayout(verticalLayout, false);
    resizeToWidth(810);
    buttonBox()->setStandardButtons(QDialogButtonBox::Close);

    tabWidget->setCurrentIndex(0);

    componentsListView->setModel(new StringListModel(this));
    overridesListView->setModel(new StringListModel(this));

    connect(componentAddPushButton, &QPushButton::clicked, this, &RustInstaller::onComponentAddPushButtonAddClicked);
    connect(componentRemovePushButton, &QPushButton::clicked, this, &RustInstaller::onComponentRemovePushButtonAddClicked);

    connect(overrideSetPushButton, &QPushButton::clicked, this, &RustInstaller::onOverrideSetPushButtonSetClicked);
    connect(overrideUnsetPushButton, &QPushButton::clicked, this, &RustInstaller::onOverrideUnsetPushButtonSetClicked);
    connect(overrideCleanupPushButton, &QPushButton::clicked, this, &RustInstaller::onOverrideCleanupPushButtonSetClicked);

    connect(breakPushButton, &QPushButton::clicked, this, &RustInstaller::onBreakPushButtonClicked);

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

    loadComponentList();
    loadOverrideList();

    readSettings();
}

RustInstaller::~RustInstaller() {
    writeSettings();
}

void RustInstaller::rustupDownload() {
#if defined(Q_OS_LINUX)
    runCommand("sh", { "-c", "curl https://sh.rustup.rs -sSf | sh -s -- -y" }, [this] {
        rustupTab->loadVersion();
    });

    installDefaultComponents();
#elif defined(Q_OS_WIN)
    QUrl url("https://static.rust-lang.org/rustup/dist/x86_64-pc-windows-gnu/rustup-init.exe");
    showAndScrollMessage("Download " + url.toString());
    fileDownloader->load(url);
    updateAllButtonsState();
#endif
}

void RustInstaller::rustupUpdate() {
    runCommand("rustup", { "self", "update" }, [this] {
        rustupTab->loadVersion();
    });
}

void RustInstaller::rustupUpdateAll() {
    runCommand("rustup", { "update" }, [this] {
        rustupTab->loadVersion();
    });
}

void RustInstaller::rustupUninstall() {
    int button = QMessageBox::question(this, tr("Uninstall Rust"), tr("Rust will be uninstalled. Are you sure?"),
                                       QMessageBox::Ok,
                                       QMessageBox::Cancel);

    if (button == QMessageBox::Ok) {
        runCommand("rustup", { "self", "uninstall", "-y" });
    }
}

void RustInstaller::onComponentAddPushButtonAddClicked() {
    AddComponentOrTarget addComponent(tr("Add Component"), "rustup component list", this);
    addComponent.exec();

    QStringList components = addComponent.list();

    if (components.count()) {
        cleanupTarget(components);
        runCommand("rustup", QStringList("component") << "add" << components, [this] {
            loadComponentList();
        });
    }
}

void RustInstaller::onComponentRemovePushButtonAddClicked() {
    int button = QMessageBox::question(this, tr("Remove Components"), tr("Components will be removed. Are you sure?"),
                          QMessageBox::Ok,
                          QMessageBox::Cancel);
    if (button == QMessageBox::Ok) {
        QStringList components = componentsListView->selectedRows();
        cleanupTarget(components);
        runCommand("rustup", QStringList("component") << "remove" << components, [this] {
            loadComponentList();
        });
    }
}

void RustInstaller::onOverrideSetPushButtonSetClicked() {
    SetOverride setOverride(this);
    if (setOverride.exec() == QDialog::Rejected) return;

    QString directory = setOverride.directory();
    QString toolchain = setOverride.toolchain();

    if (!directory.isEmpty()) {
        process->setWorkingDirectory(directory);
        runCommand("rustup", QStringList("override") << "set" << toolchain, [this] {
            loadOverrideList();
        });
    }
}

void RustInstaller::onOverrideUnsetPushButtonSetClicked() {
    int button = QMessageBox::question(this, tr("Unset Override"), tr("Override will be unsetted. Are you sure?"),
                          QMessageBox::Ok,
                          QMessageBox::Cancel);
    if (button == QMessageBox::Ok) {
        for (const QString& override : overridesListView->selectedRows()) {
            runCommand("rustup", QStringList("override") << "unset" << "--path" << override.split('\t').at(0), [this] {
                loadOverrideList();
            });
        }
    }
}

void RustInstaller::onOverrideCleanupPushButtonSetClicked() {
    runCommand("rustup", { "override", "unset", "--nonexistent" }, [this] {
        loadOverrideList();
    });
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
        rustupTab->loadVersion();
    });
    installDefaultComponents();
}

void RustInstaller::onProcessStateChainged(QProcess::ProcessState newState) {
    if (newState == QProcess::Running || newState == QProcess::NotRunning) {
        updateAllButtonsState();
    }
}

void RustInstaller::createComponentsTab() {
    auto horizontalLayout = new QHBoxLayout;
    componentsListView = new SelectableListView;

    horizontalLayout->addWidget(componentsListView);

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->setContentsMargins(0, -1, -1, -1);

    componentAddPushButton = new QPushButton(tr("Add..."));
    verticalLayout->addWidget(componentAddPushButton);

    componentRemovePushButton = new QPushButton(tr("Remove..."));
    verticalLayout->addWidget(componentRemovePushButton);

    verticalLayout->addStretch();
    horizontalLayout->addLayout(verticalLayout);

    auto components = new QWidget;
    components->setLayout(horizontalLayout);
    tabWidget->addTab(components, tr("Components"));
}

void RustInstaller::createOverridesTab() {
    auto horizontalLayout = new QHBoxLayout;
    overridesListView = new SelectableListView;

    horizontalLayout->addWidget(overridesListView);

    auto verticalLayout = new QVBoxLayout;

    overrideSetPushButton = new QPushButton(tr("Set..."));
    verticalLayout->addWidget(overrideSetPushButton);

    overrideUnsetPushButton = new QPushButton(tr("Unset..."));
    verticalLayout->addWidget(overrideUnsetPushButton);

    overrideCleanupPushButton = new QPushButton(tr("Cleanup"));
    verticalLayout->addWidget(overrideCleanupPushButton);

    verticalLayout->addStretch();
    horizontalLayout->addLayout(verticalLayout);

    auto overrides = new QWidget;
    overrides->setLayout(horizontalLayout);
    tabWidget->addTab(overrides, tr("Overrides"));
}

void RustInstaller::runCommand(const QString& program, const QStringList& arguments, const std::function<void()>& postWork) {
    commandQueue.enqueue({ program, arguments, postWork });
    runFromQueue();
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
        process->start(command.program, command.arguments);
    }
}

void RustInstaller::installDefaultComponents() {
    runCommand("rustup", { "component", "add", "rls-preview", "rust-analysis", "rust-src" });
    runCommand("cargo", { "install", "racer" });
}

void RustInstaller::updateComponentButtonsState() {
    bool processesFree = process->state() == QProcess::NotRunning && !fileDownloader->isBusy();
    bool enabled = componentsListView->selectionModel()->selectedIndexes().count() && processesFree;
    componentAddPushButton->setEnabled(processesFree);
    componentRemovePushButton->setEnabled(enabled);
}

void RustInstaller::updateOverrideButtonsState() {
    bool processesFree = process->state() == QProcess::NotRunning && !fileDownloader->isBusy();
    bool enabled = overridesListView->selectionModel()->selectedIndexes().count() && processesFree;
    overrideSetPushButton->setEnabled(processesFree);
    overrideUnsetPushButton->setEnabled(enabled);
    overrideCleanupPushButton->setEnabled(enabled);
}

void RustInstaller::updateAllButtonsState() {
    bool processesFree = process->state() == QProcess::NotRunning && !fileDownloader->isBusy();
    breakPushButton->setEnabled(!processesFree);
    rustupTab->setWidgetsEnabled(processesFree);
    toolchainTab->setWidgetsEnabled(processesFree);
    targetTab->setWidgetsEnabled(processesFree);
    updateComponentButtonsState();
    updateOverrideButtonsState();
}

void RustInstaller::loadComponentList() {
    Utils::loadAndFilterList("rustup component list", componentsListView, [=, this] (QStringList& list) { rustStdFilter(list); });
    updateComponentButtonsState();
}

void RustInstaller::loadOverrideList() {
    Utils::loadAndFilterList("rustup override list", overridesListView);
    updateOverrideButtonsState();
}

void RustInstaller::rustStdFilter(QStringList& list) {
    Utils::defaultInstalledFilter(list);

    for (int i = list.count() - 1; i >= 0; i--) {
        if (list.at(i).left(8) == "rust-std") {
            list.removeAt(i);
        }
    }
}

void RustInstaller::cleanupTarget(QStringList& components) const {
    QString search = "-" + targetTab->defaultTarget();
    components.replaceInStrings(search, "");
}

void RustInstaller::readSettings() {
    tabWidget->setCurrentIndex(Settings::value("gui.rustInstaller.currentTab").toInt());
    settingsLoaded = true;
}

void RustInstaller::writeSettings() {
    Settings::setValue("gui.rustInstaller.currentTab", tabWidget->currentIndex());
    Settings::updateRustEnvironmentVariables();
}
