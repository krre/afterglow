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
class QMenu;
class FileDownloader;
class CommandLine;

class RustInstaller : public QDialog {
    Q_OBJECT

public:
    explicit RustInstaller(QWidget* parent = 0);
    ~RustInstaller();

private slots:
    void on_pushButtonBrowseRustupHome_clicked();
    void on_pushButtonDownloadRustup_clicked();
    void on_pushButtonUpdate_clicked();
    void on_pushButtonUninstall_clicked();

    void on_pushButtonInstallToolchain_clicked();
    void on_pushButtonUninstallToolchain_clicked();
    void on_pushButtonSetDefaultToolchain_clicked();

    void on_pushButtonAddTarget_clicked();
    void on_pushButtonRemoveTarget_clicked();

    void on_pushButtonAddComponent_clicked();
    void on_pushButtonRemoveComponent_clicked();

    void on_pushButtonSetOverride_clicked();
    void on_pushButtonUnsetOverride_clicked();
    void on_pushButtonCleanupOverride_clicked();

    void onDownloaded();
    void onCustomContextMenu(const QPoint& point);
    void onCopyAction();

private:
    void runCommand(const QString& program, const QStringList& arguments, const std::function<void()>& postWork = nullptr);
    void showAndScrollMessage(const QString message);
    void runFromQueue();
    void installDefaultComponents();

    void updateToolchainButtonsState();

    void loadVersion();
    void loadToolchainList();
    void loadTargetList();
    void loadComponentList();
    void loadOverrideList();

    void loadAndFilterList(const QString& command, QListView* listView, const std::function<void(QStringList&)>& filter = nullptr);
    void defaultInstalledFilter(QStringList& list);
    void rustStdFilter(QStringList& list);
    QListView* getCurrentListView() const;
    QString findDefault(QListView* listView) const;
    void cleanupTarget(QStringList& components) const;

    void readSettings();
    void writeSettings();

    struct Command {
        QString program;
        QStringList arguments;
        std::function<void()> postWork;
    };

    enum class Tab {
        Rustup,
        Toolchain,
        Targets,
        Components,
        Overrides
    };

    Ui::RustInstaller* ui;
    QProcess* process;
    FileDownloader* fileDownloader;
    QTemporaryDir tmpDir;
    QQueue<Command> commandQueue;
    QMenu* contextMenu;
    QString defaultToolchain;
    QString defaultTarget;
};
