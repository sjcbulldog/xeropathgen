//
// Copyright 2019 Jack W. Griffin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
//

#include "AboutDialog.h"
#include "GeneratorManager.h"
#include "GameFieldManager.h"
#include "GameField.h"
#include "build.h"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define BUILDVERSION STR(XERO_MAJOR_VERSION) "." STR(XERO_MINOR_VERSION) "." STR(XERO_MICRO_VERSION) "." STR(XERO_BUILD_VERSION)

AboutDialog::AboutDialog(GeneratorManager& gens, GameFieldManager& fields, QWidget *parent) : QDialog(parent), gens_(gens), fields_(fields)
{
	QString exedir = QCoreApplication::applicationDirPath();
	QString imagepath = exedir + "/images/ErrorCodeXero.png";
	QPixmap image(imagepath);

	ui.setupUi(this);
	ui.image_->setPixmap(image);
	ui.text_->setReadOnly(true);

	QFont font = ui.text_->font();
	font.setPointSize(16);
	QTextCharFormat fmt;

	fmt.setFont(font);
	ui.text_->setCurrentCharFormat(fmt);
	QString title = "XeroPathGenerator ";
	title += STR(XERO_MAJOR_VERSION) "." STR(XERO_MINOR_VERSION) "." STR(XERO_MICRO_VERSION);
	ui.text_->append(title);

	QString buildno = QString("\r\nBuild ") + QString(BUILDVERSION) + QString("\r\n\r\n");
	font.setPointSize(8);
	fmt.setFont(font);
	ui.text_->setCurrentCharFormat(fmt);
	ui.text_->append(buildno);

	font.setPointSize(12);
	fmt.setFont(font);
	ui.text_->setCurrentCharFormat(fmt);
	ui.text_->append("Brought to you by FRC Team 1425, Error Code Xero\r\n\r\n");
	ui.text_->append("Icons made by Dave Gandy from www.flaticon.com\r\n\r\n");
	ui.text_->append("PathFinder V1 taken from Jaci with much appreciation\r\n");
	ui.text_->append("https://github.com/JacisNonsense/Pathfinder\r\n\r\n");
	ui.text_->append("Ideas, algorithms, concepts, taken from Team 254, Cheesy Poofs with much appreciation.\r\n\r\n");

	ui.text_->append("Generators ");
	font.setPointSize(10);
	fmt.setFont(font);
	ui.text_->setCurrentCharFormat(fmt);
	for (auto gen : gens_.getGenerators())
	{
		ui.text_->append(QString(gen->getName().c_str()) + ", version " + gen->getVersion().toString());
	}

	font.setPointSize(12);
	fmt.setFont(font);
	ui.text_->setCurrentCharFormat(fmt);
	ui.text_->append("\r\nFields");
	font.setPointSize(10);
	fmt.setFont(font);
	ui.text_->setCurrentCharFormat(fmt);

	for (auto field : fields_.getFields())
	{
		ui.text_->append(QString(field->getName().c_str()));
	}


	(void)connect(ui.buttons_, &QDialogButtonBox::accepted, this, &AboutDialog::accept);
}

AboutDialog::~AboutDialog()
{
}
