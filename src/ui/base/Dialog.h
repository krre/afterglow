#pragma once
#include <QDialog>

class QLayout;
class QDialogButtonBox;

class Dialog : public QDialog {
public:
    explicit Dialog(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    QDialogButtonBox* buttonBox() const;
    void setContentWidget(QWidget* contentWidget);
    void setContentLayout(QLayout* contentLayout, bool stretchAfter = true);
    void setLayoutToFixedSize();
    void resizeToWidth(int width);

private:
    QDialogButtonBox* m_buttonBox = nullptr;
};

