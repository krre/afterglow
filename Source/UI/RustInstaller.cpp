#include "RustInstaller.h"
#include "ui_RustInstaller.h"
#include "Core/Settings.h"
#include <QtWidgets>

RustInstaller::RustInstaller(QWidget* parent) :
        QDialog(parent),
        ui(new Ui::RustInstaller) {
    ui->setupUi(this);
    ui->lineEditRustup->setText(Settings::getValue("rustup.path").toString());

    process = new QProcess(this);
    QTextCodec* outputCodec = QTextCodec::codecForLocale();

    connect(process, &QProcess::readyReadStandardOutput, [=] {
        const QByteArray& data = process->readAllStandardOutput();
        QTextCodec::ConverterState outputCodecState;
        const QString& output = outputCodec->toUnicode(data.constData(), data.length(), &outputCodecState);
        ui->plainTextEditConsole->insertPlainText(output);
        ui->plainTextEditConsole->verticalScrollBar()->setValue(ui->plainTextEditConsole->verticalScrollBar()->maximum());
    });

    connect(process, &QProcess::readyReadStandardError, [=] {
        const QByteArray& data = process->readAllStandardError();
        QTextCodec::ConverterState errorCodecState;
        const QString& output = outputCodec->toUnicode(data.constData(), data.length(), &errorCodecState);
        ui->plainTextEditConsole->insertPlainText(output);
        ui->plainTextEditConsole->verticalScrollBar()->setValue(ui->plainTextEditConsole->verticalScrollBar()->maximum());
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
    process->start("sh", QStringList() << "-c" << "curl https://sh.rustup.rs -sSf | sh -s -- -y");
#elif Q_OS_WIN

#endif
}
