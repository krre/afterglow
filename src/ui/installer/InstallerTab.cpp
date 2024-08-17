#include "InstallerTab.h"
#include "RustInstaller.h"

InstallerTab::InstallerTab(RustInstaller* rustInstaller, QWidget* parent)
    : QWidget(parent), m_rustInstaller(rustInstaller) {

}

RustInstaller* InstallerTab::rustupInstaller() const {
    return m_rustInstaller;
}
