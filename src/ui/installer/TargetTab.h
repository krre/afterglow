#pragma once
#include "InstallerTab.h"

class SelectableListView;

class QPushButton;

class TargetTab : public InstallerTab {
    Q_OBJECT
public:
    explicit TargetTab(RustInstaller* rustupInstaller, QWidget* parent = nullptr);

    void setWidgetsEnabled(bool enabled) override;
    void load() override;
    QString defaultTarget() const;

private slots:
    void onAddClicked();
    void onRemoveClicked();

private:
    SelectableListView* m_listView = nullptr;
    QString m_defaultTarget;

    QPushButton* m_addButton = nullptr;
    QPushButton* m_removeButton = nullptr;
};
