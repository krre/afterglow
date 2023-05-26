#pragma once
#include <QWidget>

class RustInstaller;
class SelectableListView;
class QPushButton;

class ToolchainTab : public QWidget {
    Q_OBJECT
public:
    explicit ToolchainTab(RustInstaller* rustupInstaller, QWidget* parent = nullptr);

    void setWidgetsEnabled(bool enabled);
    void loadList();

signals:
    void defaultSetted();

private slots:
    void onInstallClicked();
    void onUninstallClicked();
    void onUpdateClicked();
    void onSetDefaultClicked();

private:
    RustInstaller* rustupInstaller = nullptr;
    SelectableListView* listView = nullptr;

    QPushButton* installButton = nullptr;
    QPushButton* uninstallButton = nullptr;
    QPushButton* updateButton = nullptr;
    QPushButton* setDefaultButton = nullptr;
};
