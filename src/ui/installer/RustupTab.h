#pragma once
#include <QWidget>

class RustInstaller;
class BrowseLayout;
class QLineEdit;
class QPushButton;

class RustupTab : public QWidget {
    Q_OBJECT
public:
    explicit RustupTab(RustInstaller* rustupInstaller, QWidget* parent = nullptr);

    void setWidgetsEnabled(bool enabled);
    void loadVersion();

private slots:
    void onRustupHomeChanged(const QString& text);
    void onCargoHomeChanged(const QString& text);

    void onDownloadClicked();
    void onUpdateClicked();
    void onUpdateAllClicked();
    void onUninstallClicked();

private:
    RustInstaller* rustupInstaller = nullptr;

    QLineEdit* rustupHomeLineEdit = nullptr;
    QLineEdit* cargoHomeLineEdit = nullptr;
    QLineEdit* versionLineEdit = nullptr;

    QPushButton* downloadButton = nullptr;
    QPushButton* updateButton = nullptr;
    QPushButton* updateAllButton = nullptr;
    QPushButton* uninstallButton = nullptr;
};
