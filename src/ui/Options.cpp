#include "Options.h"
#include "base/BrowseLayout.h"
#include "core/Const.h"
#include "core/Global.h"
#include "core/Settings.h"
#include <QtWidgets>

Options::Options(QWidget* parent) : Dialog(parent) {
    setWindowTitle(tr("Options"));

    workspaceBrowseLayout = new BrowseLayout;

    auto workspaceLayout = new QHBoxLayout;
    workspaceLayout->addWidget(new QLabel(tr("Workspace:")));
    workspaceLayout->addLayout(workspaceBrowseLayout);

    auto pathGroupBox = new QGroupBox(tr("Path"));
    pathGroupBox->setLayout(workspaceLayout);

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(pathGroupBox);

    auto interfaceGroupBox = new QGroupBox(tr("Interface"));
    auto interfaceVerticalLayout = new QVBoxLayout(interfaceGroupBox);

    sessionCheckBox = new QCheckBox(tr("Restore project session"));
    sessionCheckBox->setChecked(true);
    interfaceVerticalLayout->addWidget(sessionCheckBox);

    auto openPrefsPushButton = new QPushButton(tr("Open %1").arg(Const::App::PrefsName));
    interfaceVerticalLayout->addWidget(openPrefsPushButton, 0, Qt::AlignLeft);

    auto resetSettingsPushButton = new QPushButton(tr("Reset Settings"));
    interfaceVerticalLayout->addWidget(resetSettingsPushButton, 0, Qt::AlignLeft);

    verticalLayout->addWidget(interfaceGroupBox);
    setContentLayout(verticalLayout);
    resizeToWidth(600);

    connect(openPrefsPushButton, &QPushButton::clicked, this, &Options::openPrefs);
    connect(resetSettingsPushButton, &QPushButton::clicked, this, &Options::onResetSettingsClicked);

    readSettings();
}

void Options::onResetSettingsClicked() {
    int button = QMessageBox::question(this, tr("Reset Settings"), tr("Settings will be reseted to default.\n"
                                                                      "You are need restart application.\n"
                                                                      "Continue?"),
                          QMessageBox::Ok,
                          QMessageBox::Cancel);
    if (button == QMessageBox::Ok) {
        Settings::reset();
    }
}

void Options::onAccepted() {
    writeSettings();
    accept();
}

void Options::readSettings() {
    workspaceBrowseLayout->lineEdit()->setText(Global::workspacePath());
    sessionCheckBox->setChecked(Settings::value("gui.mainWindow.session.restore").toBool());
}

void Options::writeSettings() {
    Settings::setValue("workspace", workspaceBrowseLayout->lineEdit()->text());
    Settings::setValue("gui.mainWindow.session.restore", sessionCheckBox->isChecked());
}
