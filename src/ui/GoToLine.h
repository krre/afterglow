#pragma once
#include "ui/base/Dialog.h"

class QLineEdit;

class GoToLine : public Dialog {
    Q_OBJECT
public:
    explicit GoToLine(QWidget* parent = nullptr);
    int line() const;

private slots:
    void onTextChanged(const QString& text);

private:
    QLineEdit* lineEdit = nullptr;
};
