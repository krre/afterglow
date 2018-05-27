#include "RustInstaller.h"
#include "ui_RustInstaller.h"
#include "Core/Settings.h"
#include "Core/FileDownloader.h"
#include "AddComponentOrTarget.h"
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

    ui->lineEditRustup->setText(Settings::getValue("rustup.path").toString());

    ui->listViewToolchains->setModel(new StringListModel(this));
    ui->listViewTargets->setModel(new StringListModel(this));
    ui->listViewComponents->setModel(new StringListModel(this));
    ui->listViewOverrides->setModel(new StringListModel(this));

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

void RustInstaller::on_pushButtonBrowseRustup_clicked() {
    QString path = QFileDialog::getOpenFileName(this);
    if (!path.isEmpty()) {

        ui->lineEditRustup->setText(path);
        Settings::setValue("rustup.path", path);
    }
}

void RustInstaller::on_pushButtonDownloadRustup_clicked() {
#if defined(Q_OS_LINUX)
    runCommand("sh", QStringList() << "-c" << "curl https://sh.rustup.rs -sSf | sh -s -- -y");
#elif defined(Q_OS_WIN)
    QUrl url("https://static.rust-lang.org/rustup/dist/x86_64-pc-windows-gnu/rustup-init.exe");
    showAndScrollMessage("Download " + url.toString());
    fileDownloader->load(url);
#endif

    runCommand("rustup", QStringList() << "component" << "add" << "rls-preview" << "rust-analysis" << "rust-src");
    runCommand("cargo", QStringList() << "install" << "racer");
}

void RustInstaller::on_pushButtonUpdate_clicked() {
    runCommand("rustup", QStringList() << "self" << "update");
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
    });
}

void RustInstaller::on_pushButtonAddTarget_clicked() {
    AddComponentOrTarget addTarget(tr("Add Target"), "rustup target list", this);
    addTarget.exec();

    QStringList targets = addTarget.getList();
    if (targets.count()) {
        runCommand("rustup", QStringList() << "target" << "add" << targets, [this] {
            loadTargetList();
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
        });
    }
}

void RustInstaller::on_pushButtonAddComponent_clicked() {
    AddComponentOrTarget addComponent(tr("Add Component"), "rustup component list", this);
    addComponent.exec();

    QStringList components = addComponent.getList();
    if (components.count()) {
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
        runCommand("rustup", QStringList() << "component" << "remove"
                   << Utils::getSelectedRowsFromListView(ui->listViewComponents), [this] {
            loadComponentList();
        });
    }
}

void RustInstaller::onDownloaded() {
    showAndScrollMessage(QString("Downloaded %1 bytes").arg(fileDownloader->getDownloadedData().size()));

    QString filePath = tmpDir.path() + "/" + "rustup-init.exe";
    QFile file(filePath);
    file.open(QIODevice::WriteOnly);
    file.write(fileDownloader->getDownloadedData());
    file.close();

    QStringList arguments;
    arguments << "-y";
    showAndScrollMessage(filePath + " " + arguments.join(" "));
    process->start(filePath, arguments);
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

void RustInstaller::loadToolchainList() {
    loadAndFilterList("rustup toolchain list", ui->listViewToolchains);
    updateToolchainButtonsState();
}

void RustInstaller::updateToolchainButtonsState() {
    int selectedCount = ui->listViewToolchains->selectionModel()->selectedIndexes().count();
    ui->pushButtonUninstallToolchain->setEnabled(selectedCount);
    ui->pushButtonSetDefaultToolchain->setEnabled(selectedCount);
}

void RustInstaller::loadTargetList() {
    loadAndFilterList("rustup target list", ui->listViewTargets, std::bind(&RustInstaller::defaultInstalledFilter, this, _1));
}

void RustInstaller::loadComponentList() {
    loadAndFilterList("rustup component list", ui->listViewComponents, std::bind(&RustInstaller::defaultInstalledFilter, this, _1));
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
    if (model->rowCount()) {
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

void RustInstaller::readSettings() {
    ui->tabWidget->setCurrentIndex(Settings::getValue("gui.rustInstaller.currentTab").toInt());
}

void RustInstaller::writeSettings() {
    Settings::setValue("gui.rustInstaller.currentTab", ui->tabWidget->currentIndex());
}
