#pragma once
#include <QWidget>

class RustInstaller;
class SelectableListView;

class QPushButton;

class TargetTab : public QWidget {
    Q_OBJECT
public:
    explicit TargetTab(RustInstaller* rustupInstaller, QWidget* parent = nullptr);

    void setWidgetsEnabled(bool enabled);
    void loadList();
    QString defaultTarget() const;

private slots:
    void onAddClicked();
    void onRemoveClicked();

private:
    RustInstaller* rustupInstaller = nullptr;
    SelectableListView* listView = nullptr;
    QString m_defaultTarget;

    QPushButton* addButton = nullptr;
    QPushButton* removeButton = nullptr;
};
