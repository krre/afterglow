#pragma once
#include <QDialog>
#include <QTemporaryDir>
#include <QQueue>

namespace Ui {
    class RustInstaller;
}

class QProcess;
class FileDownloader;

class RustInstaller : public QDialog {
    Q_OBJECT

public:
    explicit RustInstaller(QWidget* parent = 0);
    ~RustInstaller();

private slots:
    void on_pushButtonBrowseRustup_clicked();
    void on_pushButtonDownloadRustup_clicked();
    void on_pushButtonUpdate_clicked();
    void on_pushButtonUninstall_clicked();

    void on_pushButtonRun_clicked();
    void on_lineEditCommand_textChanged(const QString& text);

    void onDownloaded();

private:
    void runCommand(const QString& program, const QStringList& arguments);
    void showAndScrollMessage(const QString message, bool newLine = true);
    void runFromQueue();

    struct Command {
        QString program;
        QStringList arguments;
    };

    Ui::RustInstaller* ui;
    QProcess* process;
    FileDownloader* fileDownloader;
    QTemporaryDir tmpDir;
    QQueue<Command> commandQueue;
};
