#pragma once
#include <QDialog>

class QDialogButtonBox;
class QLineEdit;

class Rename : public QDialog {
    Q_OBJECT
public:
    explicit Rename(const QString& name, QWidget* parent = nullptr);
    QString name() const;

private slots:
    void onTextChanged(const QString& text);

private:
    QString m_name;
    QLineEdit* lineEdit = nullptr;
    QDialogButtonBox* buttonBox = nullptr;
};
