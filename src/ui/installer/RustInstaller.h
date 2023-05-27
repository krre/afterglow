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
class ToolchainTab;
class TargetTab;
class SelectableListView;

class RustInstaller : public StandardDialog {
    Q_OBJECT
public:
    explicit RustInstaller(QWidget* parent = nullptr);
    ~RustInstaller();

    void runCommand(const QString& program, const QStringList& arguments, const std::function<void()>& postWork = nullptr);
    void loadComponentList();

private slots:
    void rustupDownload();
    void rustupUpdate();
    void rustupUpdateAll();
    void rustupUninstall();

    void onComponentAddPushButtonAddClicked();
    void onComponentRemovePushButtonAddClicked();

    void onOverrideSetPushButtonSetClicked();
    void onOverrideUnsetPushButtonSetClicked();
    void onOverrideCleanupPushButtonSetClicked();

    void onBreakPushButtonClicked();

    void onDownloaded();
    void onProcessStateChainged(QProcess::ProcessState newState);

private:
    void createComponentsTab();
    void createOverridesTab();

    void showAndScrollMessage(const QString message);
    void runFromQueue();
    void installDefaultComponents();

    void updateComponentButtonsState();
    void updateOverrideButtonsState();
    void updateAllButtonsState();

    void loadOverrideList();

    void rustStdFilter(QStringList& list);
    void cleanupTarget(QStringList& components) const;

    void readSettings();
    void writeSettings();

    struct Command {
        QString program;
        QStringList arguments;
        std::function<void()> postWork;
    };

    QTabWidget* tabWidget = nullptr;

    RustupTab* rustupTab = nullptr;
    ToolchainTab* toolchainTab = nullptr;
    TargetTab* targetTab = nullptr;

    QPlainTextEdit* consolePlainTextEdit = nullptr;

    SelectableListView* componentsListView = nullptr;
    SelectableListView* overridesListView = nullptr;

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
    bool settingsLoaded = false;
};
