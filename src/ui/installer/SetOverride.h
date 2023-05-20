#pragma once
#include "ui/base/StandardDialog.h"

class QComboBox;
class BrowseLayout;

class SetOverride : public StandardDialog {
    Q_OBJECT
public:
    explicit SetOverride(QWidget* parent = nullptr);

    QString directory() const;
    QString toolchain() const;

private slots:
    void onTextChanged(const QString& text);

private:
    BrowseLayout* directoryBrowseLayout = nullptr;
    QComboBox* toolchainComboBox = nullptr;
};
