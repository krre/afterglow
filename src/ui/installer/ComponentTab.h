#pragma once
#include "InstallerTab.h"

class SelectableListView;
class QPushButton;

class ComponentTab : public InstallerTab {
    Q_OBJECT
public:
    explicit ComponentTab(RustInstaller* rustupInstaller, QWidget* parent = nullptr);

    void setWidgetsEnabled(bool enabled) override;
    void load() override;

private slots:
    void onAddClicked();
    void onRemoveClicked();

private:
    SelectableListView* listView = nullptr;

    QPushButton* addButton = nullptr;
    QPushButton* removeButton = nullptr;
};
