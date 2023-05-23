#pragma once
#include "ui/base/StandardDialog.h"
#include <QTemporaryDir>
#include <QQueue>
#include <QProcess>
#include <functional>

class QTabWidget;
class QPlainTextEdit;
class QListView;
class QLineEdit;
class QPushButton;
class QMenu;

class FileDownloader;
class CommandLine;
class BrowseLayout;
class RustupTab;
class InstallerListView;

class RustInstaller : public StandardDialog {
    Q_OBJECT
public:
    explicit RustInstaller(QWidget* parent = nullptr);
    ~RustInstaller();

private slots:
    void rustupDownload();
    void rustupUpdate();
    void rustupUpdateAll();
    void rustupUninstall();

    void onToolchainInstallPushButtonClicked();
    void onToolchainUninstallPushButtonClicked();
    void onToolchainUpdatePushButtonClicked();
    void onToolchainSetDefaultPushButtonClicked();

    void onTargetAddPushButtonAddClicked();
    void onTargetRemovePushButtonAddClicked();

    void onComponentAddPushButtonAddClicked();
    void onComponentRemovePushButtonAddClicked();

    void onOverrideSetPushButtonSetClicked();
    void onOverrideUnsetPushButtonSetClicked();
    void onOverrideCleanupPushButtonSetClicked();

    void onBreakPushButtonClicked();

    void onDownloaded();
    void onProcessStateChainged(QProcess::ProcessState newState);

private:
    void createToolchainsTab();
    void createTargetsTab();
    void createComponentsTab();
    void createOverridesTab();

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

    void loadToolchainList();
    void loadTargetList();
    void loadComponentList();
    void loadOverrideList();

    void loadAndFilterList(const QString& command, QListView* listView, const std::function<void(QStringList&)>& filter = nullptr);
    void defaultInstalledFilter(QStringList& list);
    void rustStdFilter(QStringList& list);
    QListView* currentListView() const;
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

    QTabWidget* tabWidget = nullptr;
    RustupTab* rustupTab = nullptr;

    QPlainTextEdit* consolePlainTextEdit = nullptr;

    InstallerListView* toolchainsListView = nullptr;
    InstallerListView* targetsListView = nullptr;
    InstallerListView* componentsListView = nullptr;
    InstallerListView* overridesListView = nullptr;

    QPushButton* toolchainInstallPushButton = nullptr;
    QPushButton* toolchainUninstallPushButton = nullptr;
    QPushButton* toolchainUpdatePushButton = nullptr;
    QPushButton* toolchainSetDefaultPushButton = nullptr;

    QPushButton* targetAddPushButton = nullptr;
    QPushButton* targetRemovePushButton = nullptr;

    QPushButton* componentAddPushButton = nullptr;
    QPushButton* componentRemovePushButton = nullptr;

    QPushButton* overrideSetPushButton = nullptr;
    QPushButton* overrideUnsetPushButton = nullptr;
    QPushButton* overrideCleanupPushButton = nullptr;

    QPushButton* breakPushButton = nullptr;

    QProcess* process;
    FileDownloader* fileDownloader = nullptr;
    QTemporaryDir tmpDir;
    QQueue<Command> commandQueue;
    QString defaultToolchain;
    QString defaultTarget;
    bool settingsLoaded = false;
};
