#pragma once
#include <QDialog>

namespace Ui {
    class RustInstaller;
}

class QProcess;

class RustInstaller : public QDialog {
    Q_OBJECT

public:
    explicit RustInstaller(QWidget* parent = 0);
    ~RustInstaller();

private slots:
    void on_pushButtonBrowseRustup_clicked();
    void on_pushButtonDownloadRustup_clicked();

private:
    Ui::RustInstaller* ui;
    QProcess* process;
};
