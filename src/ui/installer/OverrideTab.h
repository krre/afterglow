#pragma once
#include <QWidget>

class RustInstaller;
class SelectableListView;

class QPushButton;

class OverrideTab : public QWidget {
    Q_OBJECT
public:
    explicit OverrideTab(RustInstaller* rustupInstaller, QWidget* parent = nullptr);

    void setWidgetsEnabled(bool enabled);
    void loadList();

private slots:
    void onSetClicked();
    void onUnsetClicked();
    void onCleanupClicked();

private:
    RustInstaller* rustupInstaller = nullptr;
    SelectableListView* listView = nullptr;

    QPushButton* setButton = nullptr;
    QPushButton* unsetButton = nullptr;
    QPushButton* cleanupButton = nullptr;
};
