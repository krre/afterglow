#pragma once
#include <QDialog>

namespace Ui {
    class Rename;
}

class Rename : public QDialog {
    Q_OBJECT

public:
    explicit Rename(const QString& path, QWidget* parent = nullptr);
    ~Rename();
    QString getName() const;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_lineEdit_textChanged(const QString& text);

private:
    Ui::Rename* ui;
    QString name;
};
