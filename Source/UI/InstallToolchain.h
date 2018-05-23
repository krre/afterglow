#pragma once
#include <QDialog>

namespace Ui {
    class InstallToolchain;
}

class InstallToolchain : public QDialog {
    Q_OBJECT

public:
    explicit InstallToolchain(QWidget* parent = 0);
    ~InstallToolchain();

private slots:
    void on_comboBoxChannel_currentIndexChanged(int index);
    void on_comboBoxHost_currentIndexChanged(int index);

private:
    void loadHosts();

    Ui::InstallToolchain* ui;
};
