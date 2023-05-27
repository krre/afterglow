#pragma once
#include "InstallerTab.h"

class SelectableListView;

class QPushButton;

class OverrideTab : public InstallerTab {
    Q_OBJECT
public:
    explicit OverrideTab(RustInstaller* rustupInstaller, QWidget* parent = nullptr);

    void setWidgetsEnabled(bool enabled) override;
    void load() override;

private slots:
    void onSetClicked();
    void onUnsetClicked();
    void onCleanupClicked();

private:
    SelectableListView* listView = nullptr;

    QPushButton* setButton = nullptr;
    QPushButton* unsetButton = nullptr;
    QPushButton* cleanupButton = nullptr;
};
