#pragma once
#include <QDialog>

class QDialogButtonBox;

class Rename : public QDialog {
    Q_OBJECT
public:
    explicit Rename(const QString& path, QWidget* parent = nullptr);
    const QString& name() const;

private slots:
    void onRejected();

    void onTextChanged(const QString& text);

private:
    QString m_name;
    QDialogButtonBox* buttonBox = nullptr;
};
