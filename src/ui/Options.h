#pragma once
#include "base/Dialog.h"

class QLineEdit;
class QCheckBox;

class Options : public Dialog {
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
