#pragma once
#include <QHBoxLayout>

class QLineEdit;

class BrowseLineEdit : public QHBoxLayout {
public:
    BrowseLineEdit();

    QLineEdit* lineEdit() const;

private slots:
    void onClicked();

private:
    QLineEdit* m_lineEdit = nullptr;
};
