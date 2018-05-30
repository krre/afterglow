#include "RustInstaller.h"
#include "ui_RustInstaller.h"
#include "Core/Settings.h"
#include "Core/FileDownloader.h"
#include "AddComponentOrTarget.h"
#include "SetOverride.h"
#include "StringListModel.h"
#include "Core/Utils.h"
#include "InstallToolchain.h"
#include "Core/Settings.h"
#include <QtWidgets>

using namespace std::placeholders;

RustInstaller::RustInstaller(QWidget* parent) :
        QDialog(parent),
        ui(new Ui::RustInstaller) {
    ui->setupUi(this);

    ui->listViewToolchains->setModel(new StringListModel(this));
    ui->listViewTargets->setModel(new StringListModel(this));
    ui->listViewComponents->setModel(new StringListModel(this));
    ui->listViewOverrides->setModel(new StringListModel(this));

    connect(ui->listViewToolchains, &QListView::customContextMenuRequested, this, &RustInstaller::onCustomContextMenu);
    connect(ui->listViewTargets, &QListView::customContextMenuRequested, this, &RustInstaller::onCustomContextMenu);
    connect(ui->listViewComponents, &QListView::customContextMenuRequested, this, &RustInstaller::onCustomContextMenu);
    connect(ui->listViewOverrides, &QListView::customContextMenuRequested, this, &RustInstaller::onCustomContextMenu);

    contextMenu = new QMenu(this);
    QAction* copyAction = contextMenu->addAction(tr("Copy"));
    connect(copyAction, &QAction::triggered, this, &RustInstaller::onCopyAction);

    process = new QProcess(this);
    QTextCodec* outputCodec = QTextCodec::codecForLocale();

    connect(process, &QProcess::readyReadStandardOutput, [=] {
        const QByteArray& data = process->readAllStandardOutput();
        QTextCodec::ConverterState outputCodecState;
        const QString& output = outputCodec->toUnicode(data.constData(), data.length(), &outputCodecState);
        showAndScrollMessage(output);
    });

    connect(process, &QProcess::readyReadStandardError, [=] {
        const QByteArray& data = process->readAllStandardError();
        QTextCodec::ConverterState errorCodecState;
        const QString& output = outputCodec->toUnicode(data.constData(), data.length(), &errorCodecState);
        showAndScrollMessage(output);
    });

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [=] (int exitCode, QProcess::ExitStatus exitStatus) {
        Q_UNUSED(exitStatus)
        Q_UNUSED(exitCode)
        QString message = QString("<font color=%1>%2</font>").arg("#0000FF").arg(tr("Command finished successfully"));
        showAndScrollMessage(message);
        Command command = commandQueue.dequeue();
        if (command.postWork) {
            command.postWork();
        }

        runFromQueue();
    });

    connect(process, &QProcess::errorOccurred, [=] (QProcess::ProcessError error){
        Q_UNUSED(error)
        QString message = QString("<font color=%1>%2</font>").arg("#0000FF").arg(tr("Command finished with error"));
        showAndScrollMessage(message);
        commandQueue.clear();
    });

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
    delete ui;
}

void RustInstaller::on_pushButtonBrowseRustupHome_clicked() {
    QString dirPath = QFileDialog::getExistingDirectory(this);
    if (!dirPath.isEmpty()) {
        ui->lineEditRustupHome->setText(dirPath);
        writeSettings();
    }
}

void RustInstaller::on_pushButtonBrowseCargoHome_clicked() {
    QString dirPath = QFileDialog::getExistingDirectory(this);
    if (!dirPath.isEmpty()) {
        ui->lineEditCargoHome->setText(dirPath);
        writeSettings();
    }
}

void RustInstaller::on_pushButtonDownloadRustup_clicked() {
#if defined(Q_OS_LINUX)
    runCommand("sh", QStringList() << "-c" << "curl https://sh.rustup.rs -sSf | sh -s -- -y");
    installDefaultComponents();
#elif defined(Q_OS_WIN)
    QUrl url("https://static.rust-lang.org/rustup/dist/x86_64-pc-windows-gnu/rustup-init.exe");
    showAndScrollMessage("Download " + url.toString());
    fileDownloader->load(url);
#endif
}

void RustInstaller::on_pushButtonUpdate_clicked() {
    runCommand("rustup", QStringList() << "self" << "update", [this] {
        loadVersion();
    });
}

void RustInstaller::on_pushButtonUninstall_clicked() {
    int button = QMessageBox::question(this, tr("Uninstall Rust"), tr("Rust will be uninstalled. Are you sure?"),
                          QMessageBox::Ok,
                          QMessageBox::Cancel);
    if (button == QMessageBox::Ok) {
        runCommand("rustup", QStringList() << "self" << "uninstall" << "-y");
    }
}

void RustInstaller::on_pushButtonInstallToolchain_clicked() {
    InstallToolchain installToolchain(this);
    installToolchain.exec();

    QString toolchain = installToolchain.getToolchain();
    if (!toolchain.isEmpty()) {
        runCommand("rustup", QStringList() << "toolchain" << "install" << toolchain, [this] {
            loadToolchainList();
        });
    }
}

void RustInstaller::on_pushButtonUninstallToolchain_clicked() {
    int button = QMessageBox::question(this, tr("Uninstall Toolchain"), tr("Toolchains will be uninstalled. Are you sure?"),
                          QMessageBox::Ok,
                          QMessageBox::Cancel);
    if (button == QMessageBox::Ok) {
        runCommand("rustup", QStringList() << "toolchain" << "uninstall"
                   << Utils::getSelectedRowsFromListView(ui->listViewToolchains), [this] {
            loadToolchainList();
        });
    }
}

void RustInstaller::on_pushButtonSetDefaultToolchain_clicked() {
    runCommand("rustup", QStringList() << "default"
               << Utils::getSelectedRowsFromListView(ui->listViewToolchains).at(0), [this] {
        loadToolchainList();
        loadTargetList();
        loadComponentList();
    });
}

void RustInstaller::on_pushButtonAddTarget_clicked() {
    AddComponentOrTarget addTarget(tr("Add Target"), "rustup target list", this);
    addTarget.exec();

    QStringList targets = addTarget.getList();
    if (targets.count()) {
        runCommand("rustup", QStringList() << "target" << "add" << targets, [this] {
            loadTargetList();
            loadComponentList();
        });
    }
}

void RustInstaller::on_pushButtonRemoveTarget_clicked() {
    int button = QMessageBox::question(this, tr("Remove Targets"), tr("Targets will be removed. Are you sure?"),
                          QMessageBox::Ok,
                          QMessageBox::Cancel);
    if (button == QMessageBox::Ok) {
        runCommand("rustup", QStringList() << "target" << "remove"
                   << Utils::getSelectedRowsFromListView(ui->listViewTargets), [this] {
            loadTargetList();
            loadComponentList();
        });
    }
}

void RustInstaller::on_pushButtonAddComponent_clicked() {
    AddComponentOrTarget addComponent(tr("Add Component"), "rustup component list", this);
    addComponent.exec();

    QStringList components = addComponent.getList();

    if (components.count()) {
        cleanupTarget(components);
        runCommand("rustup", QStringList() << "component" << "add" << components, [this] {
            loadComponentList();
        });
    }
}

void RustInstaller::on_pushButtonRemoveComponent_clicked() {
    int button = QMessageBox::question(this, tr("Remove Components"), tr("Components will be removed. Are you sure?"),
                          QMessageBox::Ok,
                          QMessageBox::Cancel);
    if (button == QMessageBox::Ok) {
        QStringList components = Utils::getSelectedRowsFromListView(ui->listViewComponents);
        cleanupTarget(components);
        runCommand("rustup", QStringList() << "component" << "remove" << components, [this] {
            loadComponentList();
        });
    }
}

void RustInstaller::on_pushButtonSetOverride_clicked() {
    SetOverride setOverride(this);
    setOverride.exec();

    QString directory = setOverride.getDirectory();
    QString toolchain = setOverride.getToolchain();

    if (!directory.isEmpty()) {
        process->setWorkingDirectory(directory);
        runCommand("rustup", QStringList() << "override" << "set" << toolchain, [this] {
            loadOverrideList();
        });
    }
}

void RustInstaller::on_pushButtonUnsetOverride_clicked() {
    int button = QMessageBox::question(this, tr("Unset Override"), tr("Override will be unsetted. Are you sure?"),
                          QMessageBox::Ok,
                          QMessageBox::Cancel);
    if (button == QMessageBox::Ok) {
        QStringList overrides = Utils::getSelectedRowsFromListView(ui->listViewOverrides);
        for (const QString& override : overrides) {
            runCommand("rustup", QStringList() << "override" << "unset" << "--path" << override.split('\t').at(0), [this] {
                loadOverrideList();
            });
        }
    }
}

void RustInstaller::on_pushButtonCleanupOverride_clicked() {
    runCommand("rustup", QStringList() << "override" << "unset" << "--nonexistent", [this] {
        loadOverrideList();
    });
}

void RustInstaller::onDownloaded() {
    showAndScrollMessage(QString("Downloaded %1 bytes").arg(fileDownloader->getDownloadedData().size()));

    QString filePath = tmpDir.path() + "/" + "rustup-init.exe";
    QFile file(filePath);
    file.open(QIODevice::WriteOnly);
    file.write(fileDownloader->getDownloadedData());
    file.close();

    runCommand(filePath, QStringList() << "-y");
    installDefaultComponents();
}

void RustInstaller::onCustomContextMenu(const QPoint& point) {
    QListView* listView = getCurrentListView();

    if (listView->indexAt(point).isValid()) {
        contextMenu->exec(listView->mapToGlobal(point));
    }
}

void RustInstaller::onCopyAction() {
    Utils::copySelectedRowsFromListViewToClipboard(getCurrentListView());
}

void RustInstaller::runCommand(const QString& program, const QStringList& arguments, const std::function<void()>& postWork) {
    commandQueue.enqueue({ program, arguments, postWork });
    runFromQueue();
}

void RustInstaller::showAndScrollMessage(const QString message) {
    ui->plainTextEditConsole->appendHtml(message);
    ui->plainTextEditConsole->verticalScrollBar()->setValue(ui->plainTextEditConsole->verticalScrollBar()->maximum());
}

void RustInstaller::runFromQueue() {
    if (!commandQueue.isEmpty() && process->state() == QProcess::NotRunning) {
        if (ui->plainTextEditConsole->document()->blockCount() > 1) {
            showAndScrollMessage("");
        }

        Command command = commandQueue.head();
        QString message = QString("<font color=%1>%2</font>: <font color=%3>%4 %5</font>")
                    .arg("#0000FF")
                    .arg(tr("Run command"))
                    .arg("#FF0000")
                    .arg(command.program)
                    .arg(command.arguments.join(" "));
        showAndScrollMessage(message);
        process->start(command.program, command.arguments);
    }
}

void RustInstaller::installDefaultComponents() {
    runCommand("rustup", QStringList() << "component" << "add" << "rls-preview" << "rust-analysis" << "rust-src");
    runCommand("cargo", QStringList() << "install" << "racer");
}

void RustInstaller::loadToolchainList() {
    loadAndFilterList("rustup toolchain list", ui->listViewToolchains);
    updateToolchainButtonsState();
    defaultToolchain = findDefault(ui->listViewToolchains);
}

void RustInstaller::updateToolchainButtonsState() {
    int selectedCount = ui->listViewToolchains->selectionModel()->selectedIndexes().count();
    ui->pushButtonUninstallToolchain->setEnabled(selectedCount);
    ui->pushButtonSetDefaultToolchain->setEnabled(selectedCount);
}

void RustInstaller::loadVersion() {
    QStringList list = Utils::getListFromConsole("rustup show");
    for (const QString& row : list) {
        if (row.left(5) == "rustc") {
            ui->lineEditVersion->setText(row);
            break;
        }
    }
}

void RustInstaller::loadTargetList() {
    loadAndFilterList("rustup target list", ui->listViewTargets, std::bind(&RustInstaller::defaultInstalledFilter, this, _1));
    defaultTarget = findDefault(ui->listViewTargets);
}

void RustInstaller::loadComponentList() {
    loadAndFilterList("rustup component list", ui->listViewComponents, std::bind(&RustInstaller::rustStdFilter, this, _1));
}

void RustInstaller::loadOverrideList() {
    loadAndFilterList("rustup override list", ui->listViewOverrides);
}

void RustInstaller::loadAndFilterList(const QString& command, QListView* listView, const std::function<void(QStringList&)>& filter) {
    StringListModel* model = static_cast<StringListModel*>(listView->model());
    QStringList list = Utils::getListFromConsole(command);

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

QListView* RustInstaller::getCurrentListView() const {
    QListView* listView = nullptr;
    Tab currentTab = static_cast<Tab>(ui->tabWidget->currentIndex());
    if (currentTab == Tab::Toolchain) {
        listView = ui->listViewToolchains;
    } else if (currentTab == Tab::Targets) {
        listView = ui->listViewTargets;
    } else if (currentTab == Tab::Components) {
        listView = ui->listViewComponents;
    } else if (currentTab == Tab::Overrides) {
        listView = ui->listViewOverrides;
    }

    Q_ASSERT(listView != nullptr);

    return listView;
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
    ui->lineEditRustupHome->setText(Settings::getValue("environment.rustupHome").toString());
    ui->lineEditCargoHome->setText(Settings::getValue("environment.cargoHome").toString());
    ui->tabWidget->setCurrentIndex(Settings::getValue("gui.rustInstaller.currentTab").toInt());
}

void RustInstaller::writeSettings() {
    Settings::setValue("environment.rustupHome", ui->lineEditRustupHome->text());
    Settings::setValue("environment.cargoHome", ui->lineEditCargoHome->text());
    Settings::setValue("gui.rustInstaller.currentTab", ui->tabWidget->currentIndex());
    Settings::updateRustEnvironmentVariables();
}
