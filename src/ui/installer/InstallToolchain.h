#pragma once
#include "ui/base/StandardDialog.h"

class QLineEdit;
class QComboBox;

class InstallToolchain : public StandardDialog {
    Q_OBJECT
public:
    explicit InstallToolchain(QWidget* parent = nullptr);
    QString toolchain() const;

private slots:
    void onChannelCurrentIndexChanged(int index);
    void onHostCurrentIndexChanged(int index);

private:
    void loadHosts();

    QLineEdit* m_dateLineEdit = nullptr;

    QComboBox* m_channelComboBox = nullptr;
    QLineEdit* m_channelLineEdit = nullptr;

    QComboBox* m_hostComboBox = nullptr;
    QLineEdit* m_hostLineEdit = nullptr;
};
