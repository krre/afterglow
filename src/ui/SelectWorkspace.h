#pragma once
#include "base/Dialog.h"

class QLineEdit;

class SelectWorkspace : public Dialog {
    Q_OBJECT
public:
    explicit SelectWorkspace(QWidget* parent = nullptr);

protected slots:
    void accept() override;

private slots:
    void onBrowseButtonClicked();
    void adjustAcceptedButton(const QString& text);

private:
    QLineEdit* lineEdit = nullptr;
};
