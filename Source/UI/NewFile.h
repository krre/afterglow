#pragma once
#include <QDialog>

namespace Ui {
    class NewFile;
}

class NewFile : public QDialog {
    Q_OBJECT

public:
    explicit NewFile(const QString& projectPath, QWidget* parent = 0);
    ~NewFile();
    QString getFilePath() const;

private slots:
    void on_pushButtonBrowse_clicked();
    void on_buttonBox_accepted();
    void on_lineEditName_textChanged(const QString& text);
    void on_lineEditDirectory_textChanged(const QString& text);

private:
    void changeOkButtonState();

private:
    Ui::NewFile* ui;
    QString filePath;
};
