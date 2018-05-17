#include "RustInstaller.h"
#include "ui_RustInstaller.h"
#include "Core/Settings.h"
#include <QtWidgets>

RustInstaller::RustInstaller(QWidget* parent) :
        QDialog(parent),
        ui(new Ui::RustInstaller) {
    ui->setupUi(this);
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
        showAndScrollMessage(QString(tr("Process finished with exit code %1\n\n")).arg(exitCode));
    });
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
#ifdef Q_OS_LINUX
    runCommand("sh", QStringList() << "-c" << "curl https://sh.rustup.rs -sSf | sh -s -- -y");
#elif Q_OS_WIN

#endif
}

void RustInstaller::on_pushButtonUpdate_clicked() {
    runCommand("rustup", QStringList() << "self" << "update");
}

void RustInstaller::on_pushButtonUninstall_clicked() {
    int button = QMessageBox::question(this, tr("Uninstall Rust"), tr("Rust will be uninstall. Are you sure?"),
                          QMessageBox::Ok,
                          QMessageBox::Cancel);
    if (button == QMessageBox::Ok) {
        runCommand("rustup", QStringList() << "self" << "uninstall" << "-y");
    }
}

void RustInstaller::on_pushButtonRun_clicked() {
    QStringList command = ui->lineEditCommand->text().split(' ');
    QString program = command.first();
    command.removeFirst();
    runCommand(program, command);
}

void RustInstaller::on_lineEditCommand_textChanged(const QString &text) {
    ui->pushButtonRun->setEnabled(!text.isEmpty());
}

void RustInstaller::runCommand(const QString &program, const QStringList &arguments) {
    QString command = program + " " + arguments.join(" ");
    showAndScrollMessage(command);
    showAndScrollMessage("\n");

    process->start(program, arguments);
}

void RustInstaller::showAndScrollMessage(const QString message) {
    ui->plainTextEditConsole->insertPlainText(message);
    ui->plainTextEditConsole->verticalScrollBar()->setValue(ui->plainTextEditConsole->verticalScrollBar()->maximum());
}
