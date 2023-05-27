#pragma once
#include "InstallerTab.h"

class SelectableListView;
class QPushButton;

class ToolchainTab : public InstallerTab {
    Q_OBJECT
public:
    explicit ToolchainTab(RustInstaller* rustupInstaller, QWidget* parent = nullptr);

    void setWidgetsEnabled(bool enabled) override;
    void load() override;

signals:
    void defaultSetted();

private slots:
    void onInstallClicked();
    void onUninstallClicked();
    void onUpdateClicked();
    void onSetDefaultClicked();

private:
    SelectableListView* listView = nullptr;

    QPushButton* m_installButton = nullptr;
    QPushButton* m_uninstallButton = nullptr;
    QPushButton* m_updateButton = nullptr;
    QPushButton* m_setDefaultButton = nullptr;
};
