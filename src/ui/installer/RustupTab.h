#pragma once
#include <QWidget>

class BrowseLayout;
class QLineEdit;
class QPushButton;

class RustupTab : public QWidget {
    Q_OBJECT
public:
    explicit RustupTab(QWidget* parent = nullptr);

    void setRustupButtonsEnabled(bool enabled);
    void loadVersion();

signals:
    void downloadClicked();
    void updateClicked();
    void updateAllClicked();
    void uninstallClicked();

private slots:
    void onRustupHomeChanged(const QString& text);
    void onCargoHomeChanged(const QString& text);

private:
    QLineEdit* rustupHomeLineEdit = nullptr;
    QLineEdit* cargoHomeLineEdit = nullptr;
    QLineEdit* versionLineEdit = nullptr;

    QPushButton* downloadButton = nullptr;
    QPushButton* updateButton = nullptr;
    QPushButton* updateAllButton = nullptr;
    QPushButton* uninstallButton = nullptr;
};
