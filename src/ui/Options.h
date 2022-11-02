#pragma once
#include "base/Dialog.h"

class QCheckBox;
class BrowseLineEdit;

class Options : public Dialog {
    Q_OBJECT
public:
    explicit Options(QWidget* parent = nullptr);

signals:
    void openPrefs();

private slots:
    void onResetSettingsClicked();
    void onAccepted();

private:
    void readSettings();
    void writeSettings();

    BrowseLineEdit* workspaceBrowseLineEdit = nullptr;
    QCheckBox* sessionCheckBox = nullptr;
};
