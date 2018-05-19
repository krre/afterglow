#pragma once
#include <QDialog>

namespace Ui {
    class Options;
}

class Options : public QDialog {
    Q_OBJECT

public:
    explicit Options(QWidget* parent = 0);
    ~Options();

private slots:
    void on_pushButtonWorkspace_clicked();
    void on_buttonBox_accepted();

private:
    void readSettings();
    void writeSettings();

    Ui::Options* ui;
};
