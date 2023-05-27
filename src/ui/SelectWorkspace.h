#pragma once
#include "base/StandardDialog.h"

class BrowseLayout;

class SelectWorkspace : public StandardDialog {
    Q_OBJECT
public:
    explicit SelectWorkspace(QWidget* parent = nullptr);

protected slots:
    void accept() override;

private slots:
    void adjustAcceptedButton(const QString& text);

private:
    BrowseLayout* m_directoryBrowseLayout = nullptr;
};
