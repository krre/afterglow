#pragma once
#include "InstallerTab.h"
#include "core/async/CoTask.h"

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
    CoTask onUpdateClicked();
    CoTask onUpdateAllClicked();
    void onUninstallClicked();

private:
    QLineEdit* m_rustupHomeLineEdit = nullptr;
    QLineEdit* m_cargoHomeLineEdit = nullptr;
    QLineEdit* m_versionLineEdit = nullptr;

    QPushButton* m_downloadButton = nullptr;
    QPushButton* m_updateButton = nullptr;
    QPushButton* m_updateAllButton = nullptr;
    QPushButton* m_uninstallButton = nullptr;
};
