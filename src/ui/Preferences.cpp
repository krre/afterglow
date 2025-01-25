#include "Preferences.h"
#include "base/BrowseLayout.h"
#include "core/Application.h"
#include "core/Global.h"
#include "core/Settings.h"
#include <QLabel>
#include <QGroupBox>
#include <QCheckBox>
#include <QPushButton>
#include <QMessageBox>
#include <QLineEdit>

Preferences::Preferences(QWidget* parent) : StandardDialog(parent) {
    setWindowTitle(tr("Preferences"));

    m_workspaceBrowseLayout = new BrowseLayout;

    auto workspaceLayout = new QHBoxLayout;
    workspaceLayout->addWidget(new QLabel(tr("Workspace:")));
    workspaceLayout->addLayout(m_workspaceBrowseLayout);

    auto pathGroupBox = new QGroupBox(tr("Path"));
    pathGroupBox->setLayout(workspaceLayout);

    auto verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(pathGroupBox);

    auto interfaceGroupBox = new QGroupBox(tr("Interface"));
    auto interfaceVerticalLayout = new QVBoxLayout(interfaceGroupBox);

    m_sessionCheckBox = new QCheckBox(tr("Restore project session"));
    m_sessionCheckBox->setChecked(true);
    interfaceVerticalLayout->addWidget(m_sessionCheckBox);

    auto openPrefsPushButton = new QPushButton(tr("Open %1").arg(Application::PrefsName));
    interfaceVerticalLayout->addWidget(openPrefsPushButton, 0, Qt::AlignLeft);

    auto resetSettingsPushButton = new QPushButton(tr("Reset Settings"));
    interfaceVerticalLayout->addWidget(resetSettingsPushButton, 0, Qt::AlignLeft);

    verticalLayout->addWidget(interfaceGroupBox);
    setContentLayout(verticalLayout);
    resizeToWidth(600);

    connect(openPrefsPushButton, &QPushButton::clicked, this, &Preferences::openPrefs);
    connect(resetSettingsPushButton, &QPushButton::clicked, this, &Preferences::onResetSettingsClicked);

    readSettings();
}

void Preferences::onResetSettingsClicked() {
    int button = QMessageBox::question(this, tr("Reset Settings"), tr("Settings will be reseted to default.\n"
                                                                      "You are need restart application.\n"
                                                                      "Continue?"),
                          QMessageBox::Ok,
                          QMessageBox::Cancel);
    if (button == QMessageBox::Ok) {
        Settings::reset();
    }
}

void Preferences::onAccepted() {
    writeSettings();
    accept();
}

void Preferences::readSettings() {
    m_workspaceBrowseLayout->lineEdit()->setText(Global::workspacePath());
    m_sessionCheckBox->setChecked(Settings::value("gui.mainWindow.session.restore").toBool());
}

void Preferences::writeSettings() {
    Settings::setValue("workspace", m_workspaceBrowseLayout->lineEdit()->text());
    Settings::setValue("gui.mainWindow.session.restore", m_sessionCheckBox->isChecked());
}
