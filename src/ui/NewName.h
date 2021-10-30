#pragma once
#include "base/Dialog.h"

class QLineEdit;

class NewName : public Dialog {
    Q_OBJECT
public:
    explicit NewName(const QString& title, QWidget* parent = nullptr);
    QString name() const;

private slots:
    void onTextChanged(const QString& text);

private:
    QLineEdit* lineEdit = nullptr;
};
