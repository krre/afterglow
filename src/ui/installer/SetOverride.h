#pragma once
#include "ui/widgets/StandardDialog.h"

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
    BrowseLayout* m_directoryBrowseLayout = nullptr;
    QComboBox* m_toolchainComboBox = nullptr;
};
