#pragma once
#include <QDialog>

class QLineEdit;
class QCheckBox;

class Options : public QDialog {
    Q_OBJECT
public:
    explicit Options(QWidget* parent = nullptr);

signals:
    void openPrefs();

private slots:
    void onWorkspaceClicked();
    void onResetSettingsClicked();
    void onAccepted();

private:
    void readSettings();
    void writeSettings();

    QLineEdit* workspaceLineEdit = nullptr;
    QCheckBox* sessionCheckBox = nullptr;
};
