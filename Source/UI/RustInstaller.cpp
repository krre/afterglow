#include "RustInstaller.h"
#include "ui_RustInstaller.h"
#include "Core/Settings.h"
#include "Core/FileDownloader.h"
#include "CommandLine.h"
#include "AddComponent.h"
#include "StringListModel.h"
#include "Core/Utils.h"
#include "InstallToolchain.h"
#include <QtWidgets>

RustInstaller::RustInstaller(QWidget* parent) :
        QDialog(parent),
        ui(new Ui::RustInstaller) {
    ui->setupUi(this);

    commandLine = new CommandLine(this);
    ui->horizontalLayoutCommandLine->addWidget(commandLine);
    connect(commandLine, &QLineEdit::textChanged, this, &RustInstaller::onCommandLineTextChanged);
    connect(commandLine, &CommandLine::focusReceived, [this] () {
        ui->pushButtonRun->setDefault(true);
    });

    ui->lineEditRustup->setText(Settings::getValue("rustup.path").toString());
    ui->pushButtonRun->setEnabled(false);

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
        QString message = QString("<font color=%1>%2</font>").arg("#0000FF").arg(tr("Command finished"));
        showAndScrollMessage(message, true);
        runFromQueue();
    });

    fileDownloader = new FileDownloader(this);
    connect(fileDownloader, &FileDownloader::downloaded, this, &RustInstaller::onDownloaded);

    loadToolchainList();
    loadTargetList();
    loadComponentList();
    loadOverrideList();
}

RustInstaller::~RustInstaller() {
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
        runCommand("rustup", QStringList() << "toolchain" << "install" << toolchain);
    }
}

void RustInstaller::on_pushButtonUninstallToolchain_clicked() {
    int button = QMessageBox::question(this, tr("Uninstall Toolchain"), tr("Toolchains will be uninstalled. Are you sure?"),
                          QMessageBox::Ok,
                          QMessageBox::Cancel);
    if (button == QMessageBox::Ok) {
        runCommand("rustup", QStringList() << "toolchain" << "uninstall"
                   << Utils::getSelectedRowsFromListView(ui->listViewToolchains));
    }
}

void RustInstaller::on_pushButtonSetDefaultToolchain_clicked() {
    QStringList toolchains;

    QModelIndexList indices = ui->listViewToolchains->selectionModel()->selectedIndexes();
    StringListModel* model = static_cast<StringListModel*>(ui->listViewToolchains->model());

    for (int i = 0; i < indices.count(); i++) {
        toolchains.append(model->data(indices.at(i), Qt::DisplayRole).toString());
    }

    runCommand("rustup", QStringList() << "default"
               << Utils::getSelectedRowsFromListView(ui->listViewToolchains).at(0));
}

void RustInstaller::on_pushButtonAddComponent_clicked() {
    AddComponent addComponent(this);
    addComponent.exec();

    QStringList components = addComponent.getComponents();
    if (components.count()) {
        runCommand("rustup", QStringList() << "component" << "add" << components);
    }
}

void RustInstaller::on_pushButtonRemoveComponent_clicked() {
    int button = QMessageBox::question(this, tr("Remove Components"), tr("Components will be removed. Are you sure?"),
                          QMessageBox::Ok,
                          QMessageBox::Cancel);
    if (button == QMessageBox::Ok) {
        runCommand("rustup", QStringList() << "component" << "remove"
                   << Utils::getSelectedRowsFromListView(ui->listViewComponents));
    }
}

void RustInstaller::on_pushButtonRun_clicked() {
    QStringList command = commandLine->text().split(' ');
    QString program = command.first();
    command.removeFirst();
    runCommand(program, command);
    commandLine->run();
}

void RustInstaller::onCommandLineTextChanged(const QString &text) {
    ui->pushButtonRun->setEnabled(!text.isEmpty());
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

void RustInstaller::runCommand(const QString &program, const QStringList &arguments) {
    commandQueue.enqueue({ program, arguments });
    runFromQueue();
}

void RustInstaller::showAndScrollMessage(const QString message, bool html) {
    if (html) {
        ui->plainTextEditConsole->appendHtml(message);
    } else {
        ui->plainTextEditConsole->appendPlainText(message);
    }

    ui->plainTextEditConsole->verticalScrollBar()->setValue(ui->plainTextEditConsole->verticalScrollBar()->maximum());
}

void RustInstaller::runFromQueue() {
    if (!commandQueue.isEmpty() && process->state() == QProcess::NotRunning) {
        if (ui->plainTextEditConsole->document()->blockCount() > 1) {
            showAndScrollMessage("");
        }

        Command command = commandQueue.dequeue();
        QString message = QString("<font color=%1>%2</font>: <font color=%3>%4 %5</font>")
                    .arg("#0000FF")
                    .arg(tr("Run command"))
                    .arg("#FF0000")
                    .arg(command.program)
                    .arg(command.arguments.join(" "));
        showAndScrollMessage(message, true);
        process->start(command.program, command.arguments);
    }
}

void RustInstaller::loadToolchainList() {
    QStringList toolchainList = Utils::listFromConsole("rustup toolchain list");

    QItemSelectionModel* oldModel = ui->listViewToolchains->selectionModel();
    QAbstractItemModel* model = new StringListModel(toolchainList, this);
    ui->listViewToolchains->setModel(model);
    if (oldModel) {
        delete oldModel;
    }
}

void RustInstaller::loadTargetList() {
    QStringList targetList = Utils::listFromConsole("rustup target list");

    QItemSelectionModel* oldModel = ui->listViewTargets->selectionModel();
    QAbstractItemModel* model = new StringListModel(targetList, this);
    ui->listViewTargets->setModel(model);
    if (oldModel) {
        delete oldModel;
    }
}

void RustInstaller::loadComponentList() {
    QStringList componentList = Utils::listFromConsole("rustup component list");
    for (int i = componentList.count() - 1; i >= 0; i--) {
        if (componentList.at(i).contains("(default)")) {
            continue;
        } else if (componentList.at(i).contains("(installed)")) {
            QString component = componentList.at(i);
            componentList[i] = component.replace("(installed)", "");
        } else {
            componentList.removeAt(i);
        }
    }

    QItemSelectionModel* oldModel = ui->listViewComponents->selectionModel();
    QAbstractItemModel* model = new StringListModel(componentList, this);
    ui->listViewComponents->setModel(model);
    if (oldModel) {
        delete oldModel;
    }
}

void RustInstaller::loadOverrideList() {
    QStringList overrideList = Utils::listFromConsole("rustup override list");

    QItemSelectionModel* oldModel = ui->listViewOverrides->selectionModel();
    QAbstractItemModel* model = new StringListModel(overrideList, this);
    ui->listViewOverrides->setModel(model);
    if (oldModel) {
        delete oldModel;
    }
}
