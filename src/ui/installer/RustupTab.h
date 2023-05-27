#pragma once
#include "InstallerTab.h"

class BrowseLayout;
class QLineEdit;
class QPushButton;

class RustupTab : public InstallerTab {
    Q_OBJECT
public:
    explicit RustupTab(RustInstaller* rustupInstaller, QWidget* parent = nullptr);

    void setWidgetsEnabled(bool enabled) override;
    void load() override;

private slots:
    void onRustupHomeChanged(const QString& text);
    void onCargoHomeChanged(const QString& text);

    void onDownloadClicked();
    void onUpdateClicked();
    void onUpdateAllClicked();
    void onUninstallClicked();

private:
    QLineEdit* rustupHomeLineEdit = nullptr;
    QLineEdit* cargoHomeLineEdit = nullptr;
    QLineEdit* versionLineEdit = nullptr;

    QPushButton* downloadButton = nullptr;
    QPushButton* updateButton = nullptr;
    QPushButton* updateAllButton = nullptr;
    QPushButton* uninstallButton = nullptr;
};
