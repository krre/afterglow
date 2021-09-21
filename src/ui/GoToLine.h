#pragma once
#include <QDialog>

class QDialogButtonBox;

class GoToLine : public QDialog {
    Q_OBJECT
public:
    explicit GoToLine(QWidget* parent = nullptr);

    int line() const { return m_line; }

private slots:
    void onTextChanged(const QString& text);

private:
    QDialogButtonBox* buttonBox = nullptr;
    int m_line = -1;
    bool m_isValid = false;
};
