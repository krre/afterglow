#pragma once
#include "ui/widgets/StandardDialog.h"

class QLineEdit;

class GoToLine : public StandardDialog {
public:
    explicit GoToLine(QWidget* parent = nullptr);
    int line() const;

private slots:
    void onTextChanged(const QString& text);

private:
    QLineEdit* m_lineEdit = nullptr;
};
