#pragma once
#include "base/Dialog.h"

class BrowseLayout;

class SelectWorkspace : public Dialog {
    Q_OBJECT
public:
    explicit SelectWorkspace(QWidget* parent = nullptr);

protected slots:
    void accept() override;

private slots:
    void adjustAcceptedButton(const QString& text);

private:
    BrowseLayout* directoryBrowseLayout = nullptr;
};
