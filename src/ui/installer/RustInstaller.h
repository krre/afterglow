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
class OverrideTab;
class SelectableListView;

class RustInstaller : public StandardDialog {
    Q_OBJECT
public:
    explicit RustInstaller(QWidget* parent = nullptr);
    ~RustInstaller();

    void runCommand(const QString& program, const QStringList& arguments, const std::function<void()>& postWork = nullptr, const QString& directory = QString());
    void loadComponents();
    void cleanupTarget(QStringList& components) const;
    void downloadInstaller();

private slots:
    void onBreakPushButtonClicked();

    void onDownloaded();
    void onProcessStateChanged(QProcess::ProcessState newState);

private:
    void showAndScrollMessage(const QString message);
    void runFromQueue();
    void installDefaultComponents();
    void updateAllButtonsState();

    void readSettings();
    void writeSettings();

    struct Command {
        QString program;
        QStringList arguments;
        std::function<void()> postWork;
        QString directory;
    };

    QTabWidget* m_tabWidget = nullptr;

    RustupTab* m_rustupTab = nullptr;
    ToolchainTab* m_toolchainTab = nullptr;
    TargetTab* m_targetTab = nullptr;
    ComponentTab* m_componentTab = nullptr;
    OverrideTab* m_overrideTab = nullptr;

    QPlainTextEdit* m_consolePlainTextEdit = nullptr;

    QPushButton* m_breakPushButton = nullptr;

    QProcess* m_process;
    FileDownloader* m_fileDownloader = nullptr;
    QTemporaryDir m_tmpDir;
    QQueue<Command> m_commandQueue;
};
