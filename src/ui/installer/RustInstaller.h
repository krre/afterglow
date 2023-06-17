#pragma once
#include "core/async/CoTask.h"
#include "ui/base/StandardDialog.h"
#include <QTemporaryDir>
#include <QQueue>
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

    CoTask runCommand(const QString& program, const QStringList& arguments, const QString& directory = QString());

    void loadComponents();
    void cleanupTarget(QStringList& components) const;
    CoTask downloadInstaller();

signals:
    void breakPressed();

private slots:
    void onBreakPushButtonClicked();
    CoTask onDownloaded();

private:
    void showAndScrollMessage(const QString message);
    CoTask installDefaultComponents();
    void updateAllButtonsState(bool isProcessRunning = true);

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

    FileDownloader* m_fileDownloader = nullptr;
    QTemporaryDir m_tmpDir;
};
