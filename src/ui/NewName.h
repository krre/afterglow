#pragma once
#include "base/StandardDialog.h"

class QLineEdit;

class NewName : public StandardDialog {
    Q_OBJECT
public:
    explicit NewName(const QString& title, QWidget* parent = nullptr);
    QString name() const;

private slots:
    void onTextChanged(const QString& text);

private:
    QLineEdit* m_lineEdit = nullptr;
};
