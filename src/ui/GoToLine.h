#pragma once
#include <QDialog>

class QDialogButtonBox;
class QLineEdit;

class GoToLine : public QDialog {
    Q_OBJECT
public:
    explicit GoToLine(QWidget* parent = nullptr);
    int line() const;

private slots:
    void onTextChanged(const QString& text);

private:
    QLineEdit* lineEdit = nullptr;
    QDialogButtonBox* buttonBox = nullptr;
};
