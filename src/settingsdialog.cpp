#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QSettings>

SettingsDialog::SettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);
	connect(this, &QDialog::accepted, this, &SettingsDialog::save);
	load();
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::load()
{
	QSettings settings;

	ui->optionOnlyOne->setChecked(settings.value("OnlyOne", true).toBool());
	ui->optionRestore->setChecked(settings.value("Restore", true).toBool());
}

void SettingsDialog::save()
{
	QSettings settings;

	settings.setValue("OnlyOne", ui->optionOnlyOne->isChecked());
	settings.setValue("Restore", ui->optionRestore->isChecked());
}
