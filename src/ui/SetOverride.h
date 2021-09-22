#pragma once
#include <QDialog>

namespace Ui {
    class SetOverride;
}

class SetOverride : public QDialog {
    Q_OBJECT

public:
    explicit SetOverride(QWidget* parent = nullptr);
    ~SetOverride();

    const QString& getDirectory() const { return directory; }
    const QString& getToolchain() const { return toolchain; }

private slots:
    void on_pushButtonBrowse_clicked();
    void on_buttonBox_accepted();
    void on_lineEdit_textChanged(const QString& text);

private:
    Ui::SetOverride* ui = nullptr;
    QString directory;
    QString toolchain;
};
