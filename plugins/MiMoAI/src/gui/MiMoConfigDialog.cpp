/*
 * Copyright (C) 2024 MiMoAI
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335, USA.
 */

#include "MiMoConfigDialog.hpp"
#include "MiMoAI.hpp"
#include "ui_MiMoConfigDialog.h"

#include <QSettings>
#include "StelApp.hpp"
#include "StelGui.hpp"
#include "StelTranslator.hpp"

MiMoConfigDialog::MiMoConfigDialog(QObject* parent)
	: StelDialogSeparate("MiMoConfig", parent)
	, ui(new Ui_MiMoConfigDialogForm)
	, plugin(nullptr)
{
	setObjectName("MiMoConfigDialog");
}

MiMoConfigDialog::~MiMoConfigDialog()
{
	delete ui;
}

void MiMoConfigDialog::retranslate()
{
	if (dialog)
	{
		ui->retranslateUi(dialog);
	}
}

void MiMoConfigDialog::createDialogContent()
{
	plugin = GETSTELMODULE(MiMoAI);
	Q_ASSERT(plugin);

	ui->setupUi(dialog);

	connect(&StelApp::getInstance(), SIGNAL(languageChanged()), this, SLOT(retranslate()));
	connect(ui->titleBar, &TitleBar::closeClicked, this, [this]() {
		saveConfig();
		setVisible(false);
	});
	connect(ui->titleBar, SIGNAL(movedTo(QPoint)), this, SLOT(handleMovedTo(QPoint)));
	connect(ui->pushButton_save, SIGNAL(clicked()), this, SLOT(saveConfig()));
	connect(ui->pushButton_restoreDefaults, SIGNAL(clicked()), this, SLOT(restoreDefaults()));

	loadConfig();
}

void MiMoConfigDialog::saveConfig()
{
	QSettings* conf = StelApp::getInstance().getSettings();
	conf->beginGroup("MiMoAI");

	conf->setValue("api_key", ui->lineEdit_apiKey->text());
	conf->setValue("cluster", ui->comboBox_cluster->currentIndex());
	conf->setValue("model", ui->lineEdit_model->text());
	conf->setValue("system_prompt", ui->plainTextEdit_systemPrompt->toPlainText());

	conf->endGroup();

	plugin->loadSettings();
}

void MiMoConfigDialog::loadConfig()
{
	QSettings* conf = StelApp::getInstance().getSettings();
	conf->beginGroup("MiMoAI");

	ui->lineEdit_apiKey->setText(conf->value("api_key", "").toString());
	ui->comboBox_cluster->setCurrentIndex(conf->value("cluster", 0).toInt());
	ui->lineEdit_model->setText(conf->value("model", "gpt-4o").toString());
	ui->plainTextEdit_systemPrompt->setPlainText(conf->value("system_prompt", "").toString());

	conf->endGroup();
}

void MiMoConfigDialog::restoreDefaults()
{
	ui->lineEdit_apiKey->clear();
	ui->comboBox_cluster->setCurrentIndex(0);
	ui->lineEdit_model->setText("gpt-4o");
	ui->plainTextEdit_systemPrompt->clear();
}
