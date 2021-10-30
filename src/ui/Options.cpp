#include "Options.h"
#include "core/Const.h"
#include "core/Global.h"
#include "core/Settings.h"
#include <QtWidgets>

Options::Options(QWidget* parent) : Dialog(parent) {
    setWindowTitle(tr("Options"));

    auto pathGroupBox = new QGroupBox(tr("Path"));
    auto gridLayout = new QGridLayout(pathGroupBox);
    workspaceLineEdit = new QLineEdit(pathGroupBox);

    gridLayout->addWidget(new QLabel(tr("Workspace:")), 0, 0, 1, 1);
    gridLayout->addWidget(workspaceLineEdit, 0, 1, 1, 1);

    auto workspacePushButton = new QPushButton(tr("Browse..."));
    gridLayout->addWidget(workspacePushButton, 0, 2, 1, 1);

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

    connect(workspacePushButton, &QPushButton::clicked, this, &Options::onWorkspaceClicked);
    connect(openPrefsPushButton, &QPushButton::clicked, this, &Options::openPrefs);
    connect(resetSettingsPushButton, &QPushButton::clicked, this, &Options::onResetSettingsClicked);

    readSettings();
}

void Options::onWorkspaceClicked() {
    QString dirPath = QFileDialog::getExistingDirectory(this);

    if (!dirPath.isEmpty()) {
        workspaceLineEdit->setText(dirPath);
    }
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
    workspaceLineEdit->setText(Global::workspacePath());
    sessionCheckBox->setChecked(Settings::value("gui.mainWindow.session.restore").toBool());
}

void Options::writeSettings() {
    Settings::setValue("workspace", workspaceLineEdit->text());
    Settings::setValue("gui.mainWindow.session.restore", sessionCheckBox->isChecked());
}
