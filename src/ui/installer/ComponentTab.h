#pragma once
#include <QWidget>

class RustInstaller;
class SelectableListView;
class QPushButton;

class ComponentTab : public QWidget {
    Q_OBJECT
public:
    explicit ComponentTab(RustInstaller* rustupInstaller, QWidget* parent = nullptr);

    void setWidgetsEnabled(bool enabled);
    void loadList();

private slots:
    void onAddClicked();
    void onRemoveClicked();

private:
    RustInstaller* rustupInstaller = nullptr;
    SelectableListView* listView = nullptr;

    QPushButton* addButton = nullptr;
    QPushButton* removeButton = nullptr;
};
