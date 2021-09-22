#pragma once
#include <QDialog>

class QLineEdit;
class QDialogButtonBox;

class NewName : public QDialog {
    Q_OBJECT
public:
    explicit NewName(const QString& title, QWidget* parent = nullptr);
    QString name() const;

private slots:
    void onTextChanged(const QString& text);

private:
    QLineEdit* lineEdit = nullptr;
    QDialogButtonBox* buttonBox = nullptr;
};
