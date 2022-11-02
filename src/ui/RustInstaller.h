#pragma once
#include "base/Dialog.h"
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

class RustInstaller : public Dialog {
    Q_OBJECT
public:
    explicit RustInstaller(QWidget* parent = nullptr);
    ~RustInstaller();

private slots:
    void onRustupHomeLineEditTextChanged(const QString& text);
    void onCargoHomeLineEditTextChanged(const QString& text);

    void onRustupDownloadPushButtonClicked();
    void onRustupUpdatePushButtonClicked();
    void onRustupUpdateAllPushButtonClicked();
    void onRustupUninstallPushButtonClicked();

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
    void onCustomContextMenu(const QPoint& point);
    void onCopyAction();
    void onProcessStateChainged(QProcess::ProcessState newState);

private:
    void createRustupTab();
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

    void loadVersion();
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
    QPlainTextEdit* consolePlainTextEdit = nullptr;
    BrowseLayout* rustupHomeBrowseLayout = nullptr;
    BrowseLayout* cargoHomeBrowseLayout = nullptr;
    QLineEdit* versionLineEdit = nullptr;

    QListView* toolchainsListView = nullptr;
    QListView* targetsListView = nullptr;
    QListView* componentsListView = nullptr;
    QListView* overridesListView = nullptr;

    QPushButton* rustupDownloadPushButton = nullptr;
    QPushButton* rustupUpdatePushButton = nullptr;
    QPushButton* rustupUpdateAllPushButton = nullptr;
    QPushButton* rustupUninstallPushButton = nullptr;

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
    QMenu* contextMenu = nullptr;
    QString defaultToolchain;
    QString defaultTarget;
    bool settingsLoaded = false;
};
