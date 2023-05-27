#pragma once
#include "ui/installer/RustInstaller.h"
#include <QWidget>

class RustInstaller;

class InstallerTab : public QWidget {
public:
    InstallerTab(RustInstaller* rustInstaller, QWidget* parent = nullptr);

    virtual void load() = 0;
    virtual void setWidgetsEnabled(bool enabled) { Q_UNUSED(enabled) }

protected:
    RustInstaller* rustupInstaller() const;

private:
    RustInstaller* m_rustInstaller;
};
