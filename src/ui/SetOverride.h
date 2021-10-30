#pragma once
#include "base/Dialog.h"

class QLineEdit;
class QComboBox;

class SetOverride : public Dialog {
    Q_OBJECT
public:
    explicit SetOverride(QWidget* parent = nullptr);

    QString directory() const;
    QString toolchain() const;

private slots:
    void onBrowseButtonClicked();
    void onTextChanged(const QString& text);

private:
    QLineEdit* lineEdit = nullptr;
    QComboBox* comboBox = nullptr;
};
