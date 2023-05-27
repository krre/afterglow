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
class ComponentTab;
class SelectableListView;

class RustInstaller : public StandardDialog {
    Q_OBJECT
public:
    explicit RustInstaller(QWidget* parent = nullptr);
    ~RustInstaller();

    void runCommand(const QString& program, const QStringList& arguments, const std::function<void()>& postWork = nullptr);
    void loadComponents();
    void cleanupTarget(QStringList& components) const;

private slots:
    void rustupDownload();
    void rustupUpdate();
    void rustupUpdateAll();
    void rustupUninstall();

    void onOverrideSetPushButtonSetClicked();
    void onOverrideUnsetPushButtonSetClicked();
    void onOverrideCleanupPushButtonSetClicked();

    void onBreakPushButtonClicked();

    void onDownloaded();
    void onProcessStateChainged(QProcess::ProcessState newState);

private:
    void createOverridesTab();

    void showAndScrollMessage(const QString message);
    void runFromQueue();
    void installDefaultComponents();

    void updateOverrideButtonsState();
    void updateAllButtonsState();

    void loadOverrideList();

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
    ComponentTab* componentTab = nullptr;

    QPlainTextEdit* consolePlainTextEdit = nullptr;

    SelectableListView* overridesListView = nullptr;

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
