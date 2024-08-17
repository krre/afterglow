#pragma once
#include "base/StandardDialog.h"

class QCheckBox;
class BrowseLayout;

class Preferences : public StandardDialog {
    Q_OBJECT
public:
    explicit Preferences(QWidget* parent = nullptr);

signals:
    void openPrefs();

private slots:
    void onResetSettingsClicked();
    void onAccepted();

private:
    void readSettings();
    void writeSettings();

    BrowseLayout* m_workspaceBrowseLayout = nullptr;
    QCheckBox* m_sessionCheckBox = nullptr;
};
