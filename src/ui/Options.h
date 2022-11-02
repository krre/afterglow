#pragma once
#include "base/Dialog.h"

class QCheckBox;
class BrowseLayout;

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

    BrowseLayout* workspaceBrowseLayout = nullptr;
    QCheckBox* sessionCheckBox = nullptr;
};
