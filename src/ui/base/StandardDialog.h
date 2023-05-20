#pragma once
#include <QDialog>

class QLayout;
class QDialogButtonBox;

class StandardDialog : public QDialog {
public:
    explicit StandardDialog(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    QDialogButtonBox* buttonBox() const;

    void setContentWidget(QWidget* contentWidget);
    void setContentLayout(QLayout* contentLayout, bool stretchAfter = true);

    void setLayoutToFixedSize();
    void resizeToWidth(int width);

private:
    QDialogButtonBox* m_buttonBox = nullptr;
};
