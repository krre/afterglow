#pragma once
#include <QDialog>
#include <QTemporaryDir>
#include <QQueue>
#include <QProcess>
#include <functional>

namespace Ui {
    class RustInstaller;
}

class QListView;
class QMenu;
class FileDownloader;
class CommandLine;

class RustInstaller : public QDialog {
    Q_OBJECT

public:
    explicit RustInstaller(QWidget* parent = nullptr);
    ~RustInstaller();

private slots:
    void on_pushButtonBrowseRustupHome_clicked();
    void on_lineEditRustupHome_textChanged(const QString& text);
    void on_pushButtonBrowseCargoHome_clicked();
    void on_lineEditCargoHome_textChanged(const QString& text);

    void on_pushButtonDownloadRustup_clicked();
    void on_pushButtonUpdateRustup_clicked();
    void on_pushButtonUpdateAll_clicked();
    void on_pushButtonUninstallRustup_clicked();

    void on_pushButtonInstallToolchain_clicked();
    void on_pushButtonUninstallToolchain_clicked();
    void on_pushButtonUpdateToolchain_clicked();
    void on_pushButtonSetDefaultToolchain_clicked();

    void on_pushButtonAddTarget_clicked();
    void on_pushButtonRemoveTarget_clicked();

    void on_pushButtonAddComponent_clicked();
    void on_pushButtonRemoveComponent_clicked();

    void on_pushButtonSetOverride_clicked();
    void on_pushButtonUnsetOverride_clicked();
    void on_pushButtonCleanupOverride_clicked();

    void on_pushButtonBreak_clicked();

    void onDownloaded();
    void onCustomContextMenu(const QPoint& point);
    void onCopyAction();
    void onProcessStateChainged(QProcess::ProcessState newState);

private:
    void runCommand(const QString& program, const QStringList& arguments, const std::function<void()>& postWork = nullptr);
    void showAndScrollMessage(const QString message);
    void runFromQueue();
    void installDefaultComponents();

    void updateRustupButtonsState();
    void updateToolchainButtonsState();
    void updateTargetButtonsState();
    void updateComponentButtonsState();
    void updateOverrideButtonsState();
    void updateAllButtonsState();

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

    Ui::RustInstaller* ui = nullptr;
    QProcess* process;
    FileDownloader* fileDownloader = nullptr;
    QTemporaryDir tmpDir;
    QQueue<Command> commandQueue;
    QMenu* contextMenu = nullptr;
    QString defaultToolchain;
    QString defaultTarget;
    bool settingsLoaded = false;
};
