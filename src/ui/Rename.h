#pragma once
#include "base/StandardDialog.h"

class QLineEdit;

class Rename : public StandardDialog {
    Q_OBJECT
public:
    explicit Rename(const QString& name, QWidget* parent = nullptr);
    QString name() const;

private slots:
    void onTextChanged(const QString& text);

private:
    QString m_name;
    QLineEdit* lineEdit = nullptr;
};
