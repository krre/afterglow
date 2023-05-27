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
    SelectableListView* m_listView = nullptr;

    QPushButton* m_setButton = nullptr;
    QPushButton* m_unsetButton = nullptr;
    QPushButton* m_cleanupButton = nullptr;
};
