#include "RustInstaller.h"
#include "AddComponentOrTarget.h"
#include "SetOverride.h"
#include "InstallToolchain.h"
#include "ui/base/BrowseLayout.h"
#include "ui/StringListModel.h"
#include "core/Utils.h"
#include "core/Constants.h"
#include "core/Settings.h"
#include "core/FileDownloader.h"
#include "core/Settings.h"
#include <QtWidgets>

using namespace std::placeholders;

RustInstaller::RustInstaller(QWidget* parent) : StandardDialog(parent) {
    setWindowTitle(tr("Rust Installer"));

    tabWidget = new QTabWidget;

    createRustupTab();
    createToolchainsTab();
    createTargetsTab();
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

    toolchainsListView->setModel(new StringListModel(this));
    targetsListView->setModel(new StringListModel(this));
    componentsListView->setModel(new StringListModel(this));
    overridesListView->setModel(new StringListModel(this));

    connect(rustupDownloadPushButton, &QPushButton::clicked, this, &RustInstaller::onRustupDownloadPushButtonClicked);
    connect(rustupUpdatePushButton, &QPushButton::clicked, this, &RustInstaller::onRustupUpdatePushButtonClicked);
    connect(rustupUpdateAllPushButton, &QPushButton::clicked, this, &RustInstaller::onRustupUpdateAllPushButtonClicked);
    connect(rustupUninstallPushButton, &QPushButton::clicked, this, &RustInstaller::onRustupUninstallPushButtonClicked);

    connect(toolchainInstallPushButton, &QPushButton::clicked, this, &RustInstaller::onToolchainInstallPushButtonClicked);
    connect(toolchainUninstallPushButton, &QPushButton::clicked, this, &RustInstaller::onToolchainUninstallPushButtonClicked);
    connect(toolchainUpdatePushButton, &QPushButton::clicked, this, &RustInstaller::onToolchainUpdatePushButtonClicked);
    connect(toolchainSetDefaultPushButton, &QPushButton::clicked, this, &RustInstaller::onToolchainSetDefaultPushButtonClicked);

    connect(targetAddPushButton, &QPushButton::clicked, this, &RustInstaller::onTargetAddPushButtonAddClicked);
    connect(targetRemovePushButton, &QPushButton::clicked, this, &RustInstaller::onTargetRemovePushButtonAddClicked);

    connect(componentAddPushButton, &QPushButton::clicked, this, &RustInstaller::onComponentAddPushButtonAddClicked);
    connect(componentRemovePushButton, &QPushButton::clicked, this, &RustInstaller::onComponentRemovePushButtonAddClicked);

    connect(overrideSetPushButton, &QPushButton::clicked, this, &RustInstaller::onOverrideSetPushButtonSetClicked);
    connect(overrideUnsetPushButton, &QPushButton::clicked, this, &RustInstaller::onOverrideUnsetPushButtonSetClicked);
    connect(overrideCleanupPushButton, &QPushButton::clicked, this, &RustInstaller::onOverrideCleanupPushButtonSetClicked);

    connect(breakPushButton, &QPushButton::clicked, this, &RustInstaller::onBreakPushButtonClicked);

    connect(rustupHomeBrowseLayout->lineEdit(), &QLineEdit::textChanged, this, &RustInstaller::onRustupHomeLineEditTextChanged);
    connect(cargoHomeBrowseLayout->lineEdit(), &QLineEdit::textChanged, this, &RustInstaller::onCargoHomeLineEditTextChanged);

    connect(toolchainsListView, &QListView::customContextMenuRequested, this, &RustInstaller::onCustomContextMenu);
    connect(targetsListView, &QListView::customContextMenuRequested, this, &RustInstaller::onCustomContextMenu);
    connect(componentsListView, &QListView::customContextMenuRequested, this, &RustInstaller::onCustomContextMenu);
    connect(overridesListView, &QListView::customContextMenuRequested, this, &RustInstaller::onCustomContextMenu);

    contextMenu = new QMenu(this);
    QAction* copyAction = contextMenu->addAction(tr("Copy"));
    connect(copyAction, &QAction::triggered, this, &RustInstaller::onCopyAction);

    process = new QProcess(this);

    connect(process, &QProcess::readyReadStandardOutput, [=, this] {
        QByteArray data = process->readAllStandardOutput();
        auto toUtf16 = QStringDecoder(QStringDecoder::Utf8);
        QString output = toUtf16(data);
        showAndScrollMessage(output);
    });

    connect(process, &QProcess::readyReadStandardError, [=, this] {
        QByteArray data = process->readAllStandardError();
        auto toUtf16 = QStringDecoder(QStringDecoder::Utf8);
        QString output = toUtf16(data);
        showAndScrollMessage(output);
    });

    connect(process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), [=, this] (int, QProcess::ExitStatus) {
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

    connect(process, &QProcess::errorOccurred, [=, this] (QProcess::ProcessError error) {
        Q_UNUSED(error)
        QString message = QString("<font color=%1>%2</font>").arg("#0000FF", tr("Command finished with error"));
        showAndScrollMessage(message);
        commandQueue.clear();
    });

    connect(process, &QProcess::stateChanged, this, &RustInstaller::onProcessStateChainged);

    fileDownloader = new FileDownloader(this);
    connect(fileDownloader, &FileDownloader::downloaded, this, &RustInstaller::onDownloaded);

    loadVersion();
    loadToolchainList();
    loadTargetList();
    loadComponentList();
    loadOverrideList();

    readSettings();
}

RustInstaller::~RustInstaller() {
    writeSettings();
}

void RustInstaller::onRustupHomeLineEditTextChanged(const QString& text) {
    Q_UNUSED(text)
    if (settingsLoaded) {
        writeSettings();
    }
}

void RustInstaller::onCargoHomeLineEditTextChanged(const QString& text) {
    Q_UNUSED(text)
    if (settingsLoaded) {
        writeSettings();
    }
}

void RustInstaller::onRustupDownloadPushButtonClicked() {
#if defined(Q_OS_LINUX)
    runCommand("sh", { "-c", "curl https://sh.rustup.rs -sSf | sh -s -- -y" }, [this] {
        loadVersion();
    });
    installDefaultComponents();
#elif defined(Q_OS_WIN)
    QUrl url("https://static.rust-lang.org/rustup/dist/x86_64-pc-windows-gnu/rustup-init.exe");
    showAndScrollMessage("Download " + url.toString());
    fileDownloader->load(url);
    updateAllButtonsState();
#endif
}

void RustInstaller::onRustupUpdatePushButtonClicked() {
    runCommand("rustup", { "self", "update" }, [this] {
        loadVersion();
    });
}

void RustInstaller::onRustupUpdateAllPushButtonClicked() {
    runCommand("rustup", { "update" }, [this] {
        loadVersion();
    });
}

void RustInstaller::onRustupUninstallPushButtonClicked() {
    int button = QMessageBox::question(this, tr("Uninstall Rust"), tr("Rust will be uninstalled. Are you sure?"),
                          QMessageBox::Ok,
                          QMessageBox::Cancel);
    if (button == QMessageBox::Ok) {
        runCommand("rustup", { "self", "uninstall", "-y" });
    }
}

void RustInstaller::onToolchainInstallPushButtonClicked() {
    InstallToolchain installToolchain(this);
    if (installToolchain.exec() == QDialog::Rejected) return;;

    QString toolchain = installToolchain.toolchain();

    if (!toolchain.isEmpty()) {
        runCommand("rustup", { "toolchain", "install", toolchain }, [this] {
            loadToolchainList();
        });
    }
}

void RustInstaller::onToolchainUninstallPushButtonClicked() {
    int button = QMessageBox::question(this, tr("Uninstall Toolchain"), tr("Toolchains will be uninstalled. Are you sure?"),
                          QMessageBox::Ok,
                          QMessageBox::Cancel);
    if (button == QMessageBox::Ok) {
        runCommand("rustup", QStringList("toolchain") << "uninstall"
                   << Utils::selectedRowsFromListView(toolchainsListView), [this] {
            loadToolchainList();
        });
    }
}

void RustInstaller::onToolchainUpdatePushButtonClicked() {
    runCommand("rustup", QStringList("update") << Utils::selectedRowsFromListView(toolchainsListView));
}

void RustInstaller::onToolchainSetDefaultPushButtonClicked() {
    runCommand("rustup", QStringList("default")
               << Utils::selectedRowsFromListView(toolchainsListView).at(0), [this] {
        loadToolchainList();
        loadTargetList();
        loadComponentList();
        loadVersion();
    });
}

void RustInstaller::onTargetAddPushButtonAddClicked() {
    AddComponentOrTarget addTarget(tr("Add Target"), "rustup target list", this);
    addTarget.exec();

    QStringList targets = addTarget.list();
    if (targets.count()) {
        runCommand("rustup", QStringList("target") << "add" << targets, [this] {
            loadTargetList();
            loadComponentList();
        });
    }
}

void RustInstaller::onTargetRemovePushButtonAddClicked() {
    int button = QMessageBox::question(this, tr("Remove Targets"), tr("Targets will be removed. Are you sure?"),
                          QMessageBox::Ok,
                          QMessageBox::Cancel);
    if (button == QMessageBox::Ok) {
        runCommand("rustup", QStringList("target") << "remove"
                   << Utils::selectedRowsFromListView(targetsListView), [this] {
            loadTargetList();
            loadComponentList();
        });
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
        QStringList components = Utils::selectedRowsFromListView(componentsListView);
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
        QStringList overrides = Utils::selectedRowsFromListView(overridesListView);
        for (const QString& override : overrides) {
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
        loadVersion();
    });
    installDefaultComponents();
}

void RustInstaller::onCustomContextMenu(const QPoint& point) {
    QListView* listView = currentListView();

    if (listView->indexAt(point).isValid()) {
        contextMenu->exec(listView->mapToGlobal(point));
    }
}

void RustInstaller::onCopyAction() {
    Utils::copySelectedRowsFromListViewToClipboard(currentListView());
}

void RustInstaller::onProcessStateChainged(QProcess::ProcessState newState) {
    if (newState == QProcess::Running || newState == QProcess::NotRunning) {
        updateAllButtonsState();
    }
}

void RustInstaller::createRustupTab() {
    rustupHomeBrowseLayout = new BrowseLayout;
    cargoHomeBrowseLayout = new BrowseLayout;

    auto envLayout = new QFormLayout;
    envLayout->addRow(new QLabel("RUSTUP_HOME:"), rustupHomeBrowseLayout);
    envLayout->addRow(new QLabel("RUSTUP_HOME:"), cargoHomeBrowseLayout);

    auto groupBox = new QGroupBox(tr("Environment Variables"));
    groupBox->setLayout(envLayout);

    versionLineEdit = new QLineEdit;
    versionLineEdit->setReadOnly(true);

    auto versionLayout = new QFormLayout;
    versionLayout->addRow(tr("Version:"), versionLineEdit);

    rustupDownloadPushButton = new QPushButton(tr("Download"));
    rustupUpdatePushButton = new QPushButton(tr("Update"));
    rustupUpdateAllPushButton = new QPushButton(tr("Update All"));
    rustupUninstallPushButton = new QPushButton(tr("Uninstall..."));

    auto horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(rustupDownloadPushButton);
    horizontalLayout->addWidget(rustupUpdatePushButton);
    horizontalLayout->addWidget(rustupUpdateAllPushButton);
    horizontalLayout->addWidget(rustupUninstallPushButton);
    horizontalLayout->addStretch();

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(groupBox);
    verticalLayout->addLayout(versionLayout);
    verticalLayout->addLayout(horizontalLayout);
    verticalLayout->addStretch();

    auto rustup = new QWidget();
    rustup->setLayout(verticalLayout);
    tabWidget->addTab(rustup, "Rustup");
}

void RustInstaller::createToolchainsTab() {
    auto horizontalLayout = new QHBoxLayout;
    toolchainsListView = new QListView;
    toolchainsListView->setContextMenuPolicy(Qt::CustomContextMenu);
    toolchainsListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    toolchainsListView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    horizontalLayout->addWidget(toolchainsListView);

    auto verticalLayout = new QVBoxLayout();

    toolchainInstallPushButton = new QPushButton(tr("Install..."));
    verticalLayout->addWidget(toolchainInstallPushButton);

    toolchainUninstallPushButton = new QPushButton(tr("Uninstall..."));
    toolchainUninstallPushButton->setEnabled(false);
    verticalLayout->addWidget(toolchainUninstallPushButton);

    toolchainUpdatePushButton = new QPushButton(tr("Update"));
    toolchainUpdatePushButton->setEnabled(false);
    verticalLayout->addWidget(toolchainUpdatePushButton);

    toolchainSetDefaultPushButton = new QPushButton(tr("Set Default"));
    toolchainSetDefaultPushButton->setEnabled(false);
    verticalLayout->addWidget(toolchainSetDefaultPushButton);

    verticalLayout->addStretch();
    horizontalLayout->addLayout(verticalLayout);

    auto toolchains = new QWidget();
    toolchains->setLayout(horizontalLayout);
    tabWidget->addTab(toolchains, tr("Toolchains"));
}

void RustInstaller::createTargetsTab() {
    auto horizontalLayout = new QHBoxLayout;
    targetsListView = new QListView;
    targetsListView->setContextMenuPolicy(Qt::CustomContextMenu);
    targetsListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    targetsListView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    horizontalLayout->addWidget(targetsListView);

    auto verticalLayout = new QVBoxLayout;
    targetAddPushButton = new QPushButton(tr("Add..."));
    verticalLayout->addWidget(targetAddPushButton);

    targetRemovePushButton = new QPushButton(tr("Remove..."));
    verticalLayout->addWidget(targetRemovePushButton);

    verticalLayout->addStretch();
    horizontalLayout->addLayout(verticalLayout);

    auto targets = new QWidget;
    targets->setLayout(horizontalLayout);
    tabWidget->addTab(targets, tr("Targets"));
}

void RustInstaller::createComponentsTab() {
    auto horizontalLayout = new QHBoxLayout;
    componentsListView = new QListView;
    componentsListView->setContextMenuPolicy(Qt::CustomContextMenu);
    componentsListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    componentsListView->setSelectionMode(QAbstractItemView::ExtendedSelection);

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
    overridesListView = new QListView;
    overridesListView->setContextMenuPolicy(Qt::CustomContextMenu);
    overridesListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    overridesListView->setSelectionMode(QAbstractItemView::ExtendedSelection);

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

void RustInstaller::updateRustupButtonsState() {
    bool processesFree = process->state() == QProcess::NotRunning && !fileDownloader->isBusy();
    rustupDownloadPushButton->setEnabled(processesFree);
    rustupUpdatePushButton->setEnabled(processesFree);
    rustupUpdateAllPushButton->setEnabled(processesFree);
    rustupUninstallPushButton->setEnabled(processesFree);
}

void RustInstaller::updateToolchainButtonsState() {
    bool processesFree = process->state() == QProcess::NotRunning && !fileDownloader->isBusy();
    bool enabled = toolchainsListView->selectionModel()->selectedIndexes().count() && processesFree;
    toolchainInstallPushButton->setEnabled(processesFree);
    toolchainUninstallPushButton->setEnabled(enabled);
    toolchainUpdatePushButton->setEnabled(enabled);
    toolchainSetDefaultPushButton->setEnabled(enabled);
}

void RustInstaller::updateTargetButtonsState() {
    bool processesFree = process->state() == QProcess::NotRunning && !fileDownloader->isBusy();
    bool enabled = targetsListView->selectionModel()->selectedIndexes().count() && processesFree;
    targetAddPushButton->setEnabled(processesFree);
    targetRemovePushButton->setEnabled(enabled);
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
    updateRustupButtonsState();
    updateToolchainButtonsState();
    updateTargetButtonsState();
    updateComponentButtonsState();
    updateOverrideButtonsState();
}

void RustInstaller::loadVersion() {
    QStringList list = Utils::runConsoleCommand("rustup show");

    for (const QString& row : list) {
        if (row.left(5) == "rustc") {
            versionLineEdit->setText(row);
            break;
        }
    }
}

void RustInstaller::loadToolchainList() {
    loadAndFilterList("rustup toolchain list", toolchainsListView);
    updateToolchainButtonsState();
    defaultToolchain = findDefault(toolchainsListView);
}

void RustInstaller::loadTargetList() {
    loadAndFilterList("rustup target list", targetsListView, [=, this] (QStringList& list) { defaultInstalledFilter(list); });
    updateTargetButtonsState();
    defaultTarget = findDefault(targetsListView);
}

void RustInstaller::loadComponentList() {
    loadAndFilterList("rustup component list", componentsListView, [=, this] (QStringList& list) { rustStdFilter(list); });
    updateComponentButtonsState();
}

void RustInstaller::loadOverrideList() {
    loadAndFilterList("rustup override list", overridesListView);
    updateOverrideButtonsState();
}

void RustInstaller::loadAndFilterList(const QString& command, QListView* listView, const std::function<void(QStringList&)>& filter) {
    StringListModel* model = static_cast<StringListModel*>(listView->model());
    QStringList list = Utils::runConsoleCommand(command);

    if (list.count() == 1 && list.at(0).left(2) == "no") {
        list.removeFirst();
    }

    if (filter) {
        filter(list);
    }

    model->setStrings(list);
    if (model->getCount()) {
        listView->setCurrentIndex(model->index(0, 0));
    }
}

void RustInstaller::defaultInstalledFilter(QStringList& list) {
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

void RustInstaller::rustStdFilter(QStringList& list) {
    defaultInstalledFilter(list);

    for (int i = list.count() - 1; i >= 0; i--) {
        if (list.at(i).left(8) == "rust-std") {
            list.removeAt(i);
        }
    }
}

QListView* RustInstaller::currentListView() const {
    QListView* result = nullptr;
    Tab currentTab = static_cast<Tab>(tabWidget->currentIndex());

    if (currentTab == Tab::Toolchain) {
        result = toolchainsListView;
    } else if (currentTab == Tab::Targets) {
        result = targetsListView;
    } else if (currentTab == Tab::Components) {
        result = componentsListView;
    } else if (currentTab == Tab::Overrides) {
        result = overridesListView;
    }

    Q_ASSERT(result != nullptr);

    return result;
}

QString RustInstaller::findDefault(QListView* listView) const {
    StringListModel* model = static_cast<StringListModel*>(listView->model());

    for (int i = 0; i < model->getCount(); i++) {
        if (model->getData(i).contains("default")) {
            return model->getData(i).replace(" (default)", "");
        }
    }

    return QString();
}

void RustInstaller::cleanupTarget(QStringList& components) const {
    QString search = "-" + defaultTarget;
    components.replaceInStrings(search, "");
}

void RustInstaller::readSettings() {
    rustupHomeBrowseLayout->lineEdit()->setText(qEnvironmentVariable(Const::Environment::RustupHome));
    cargoHomeBrowseLayout->lineEdit()->setText(qEnvironmentVariable(Const::Environment::CargoHome));
    tabWidget->setCurrentIndex(Settings::value("gui.rustInstaller.currentTab").toInt());
    settingsLoaded = true;
}

void RustInstaller::writeSettings() {
    Settings::setValue("environment.rustupHome", rustupHomeBrowseLayout->lineEdit()->text());
    Settings::setValue("environment.cargoHome", cargoHomeBrowseLayout->lineEdit()->text());
    Settings::setValue("gui.rustInstaller.currentTab", tabWidget->currentIndex());
    Settings::updateRustEnvironmentVariables();
}
