#pragma once
#include <QDialog>

class QLineEdit;
class QComboBox;
class QDialogButtonBox;

class SetOverride : public QDialog {
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
    QDialogButtonBox* buttonBox = nullptr;
};
