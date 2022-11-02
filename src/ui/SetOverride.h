#pragma once
#include "base/Dialog.h"

class QLineEdit;
class QComboBox;
class BrowseLineEdit;

class SetOverride : public Dialog {
    Q_OBJECT
public:
    explicit SetOverride(QWidget* parent = nullptr);

    QString directory() const;
    QString toolchain() const;

private slots:
    void onTextChanged(const QString& text);

private:
    BrowseLineEdit* directoryBrowseLineEdit = nullptr;
    QComboBox* toolchainComboBox = nullptr;
};
