#pragma once
#include <QDialog>

namespace Ui {
    class InstallToolchain;
}

class InstallToolchain : public QDialog {
    Q_OBJECT

public:
    explicit InstallToolchain(QWidget* parent = nullptr);
    ~InstallToolchain();
    const QString& getToolchain() const { return toolchain; }

private slots:
    void on_comboBoxChannel_currentIndexChanged(int index);
    void on_comboBoxHost_currentIndexChanged(int index);

    void on_buttonBox_accepted();

private:
    void loadHosts();

    Ui::InstallToolchain* ui;
    QString toolchain;
};
