#pragma once
#include <QDialog>
#include <QTemporaryDir>
#include <QQueue>
#include <functional>

namespace Ui {
    class RustInstaller;
}

class QProcess;
class QListView;
class FileDownloader;
class CommandLine;

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

    void on_pushButtonInstallToolchain_clicked();
    void on_pushButtonUninstallToolchain_clicked();
    void on_pushButtonSetDefaultToolchain_clicked();

    void on_pushButtonAddComponent_clicked();
    void on_pushButtonRemoveComponent_clicked();

    void on_pushButtonRun_clicked();
    void onCommandLineTextChanged(const QString& text);

    void onDownloaded();

private:
    void runCommand(const QString& program, const QStringList& arguments, const std::function<void()>& postWork = nullptr);
    void showAndScrollMessage(const QString message, bool html = false);
    void runFromQueue();

    void loadToolchainList();
    void updateToolchainButtonsState();

    void loadTargetList();
    void loadComponentList();
    void loadOverrideList();

    void loadAndFilterList(const QString& command, QListView* listView, const std::function<void(QStringList&)>& filter = nullptr);

    void readSettings();
    void writeSettings();

    struct Command {
        QString program;
        QStringList arguments;
        std::function<void()> postWork;
    };

    Ui::RustInstaller* ui;
    QProcess* process;
    FileDownloader* fileDownloader;
    CommandLine* commandLine;
    QTemporaryDir tmpDir;
    QQueue<Command> commandQueue;
};
