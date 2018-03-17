#pragma once
#include <QDialog>

namespace Ui {
    class NewName;
}

class NewName : public QDialog {
    Q_OBJECT

public:
    explicit NewName(const QString& title, QWidget* parent = 0);
    ~NewName();
    QString getName() const;

private slots:
    void on_buttonBox_accepted();
    void on_lineEditName_textChanged(const QString& text);

private:
    Ui::NewName* ui;
    QString name;
};
