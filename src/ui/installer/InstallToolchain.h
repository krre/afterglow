#pragma once
#include "ui/base/Dialog.h"

class QLineEdit;
class QComboBox;

class InstallToolchain : public Dialog {
    Q_OBJECT
public:
    explicit InstallToolchain(QWidget* parent = nullptr);
    QString toolchain() const;

private slots:
    void onChannelCurrentIndexChanged(int index);
    void onHostCurrentIndexChanged(int index);

private:
    void loadHosts();

    QLineEdit* dateLineEdit = nullptr;

    QComboBox* channelComboBox = nullptr;
    QLineEdit *channelLineEdit = nullptr;

    QComboBox *hostComboBox = nullptr;
    QLineEdit *hostLineEdit = nullptr;
};
