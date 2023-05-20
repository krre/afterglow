#pragma once
#include "ui/base/StandardDialog.h"

class QLineEdit;

class GoToLine : public StandardDialog {
public:
    explicit GoToLine(QWidget* parent = nullptr);
    int line() const;

private slots:
    void onTextChanged(const QString& text);

private:
    QLineEdit* lineEdit = nullptr;
};
