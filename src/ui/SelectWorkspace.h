#pragma once
#include <QDialog>

class QLineEdit;
class QDialogButtonBox;

class SelectWorkspace : public QDialog {
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
    QDialogButtonBox* buttonBox = nullptr;
};
