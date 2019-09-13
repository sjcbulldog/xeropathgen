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

#include "HelpDisplay.h"
#include <QApplication>
#include <QHelpContentWidget>
#include <QHelpIndexWidget>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QDebug>


HelpDisplay::HelpDisplay(QWidget *parent) : QSplitter(parent)
{
	auto dirs = QStandardPaths::standardLocations(QStandardPaths::CacheLocation).front();
	if (dirs.size() == 0)
		return;

	QDir dirobj(dirs);
	if (!dirobj.exists())
		dirobj.mkpath(dirs);

	QString helpfile = dirs + "/xeropath.qhc";
	help_ = new QHelpEngine(helpfile);
	if (!help_->setupData())
	{
		qDebug() << "cannot setup help collection '" << helpfile << "' - " << help_->error();
		return;
	}

	QString helpcoll = QApplication::applicationDirPath() + "/xeropath.qch";
	auto registered = help_->registeredDocumentations();

	QString ns = QHelpEngineCore::namespaceName(helpcoll);
	if (ns.isEmpty() || !registered.contains(ns))
	{
		if (!help_->registerDocumentation(helpcoll))
		{
			qDebug() << "cannot register help file '" << helpcoll << "' - " << help_->error();
			return;
		}
	}

	tabs_ = new QTabWidget();
	tabs_->setMaximumWidth(200);
	tabs_->addTab(help_->contentWidget(), "Contents");
	tabs_->addTab(help_->indexWidget(), "Index");

	help_->contentWidget()->setVisible(true);
	help_->indexWidget()->setVisible(true);
	
	browser_ = new QTextBrowser();
	QUrl url("qthelp://errorcodexero.pathgen.help/doc/intro.html");
	auto databytes = help_->fileData(url);
	browser_->setHtml(QString(databytes));
	browser_->setOpenLinks(false);

	(void)connect(help_->contentWidget(), &QHelpContentWidget::linkActivated, this, &HelpDisplay::gotoLink);
	(void)connect(help_->indexWidget(), &QHelpIndexWidget::linkActivated, this, &HelpDisplay::gotoLinkWithKeyword);
	(void)connect(browser_, &QTextBrowser::anchorClicked, this, &HelpDisplay::gotoLink);

	insertWidget(0, tabs_);
	insertWidget(1, browser_);
}

HelpDisplay::~HelpDisplay()
{
}

void HelpDisplay::gotoLink(const QUrl& url)
{
	QUrl toload;

	if (url.isRelative())
	{
		QString str = url.toString();
		if (str.startsWith("./"))
			str = str.mid(2);
		str = "qthelp://errorcodexero.pathgen.help/doc/" + str;
		toload = QUrl(str);
	}
	else
	{
		toload = url;
	}

	qDebug() << "setting help browser source '" << toload << "'";
	auto databytes = help_->fileData(toload);
	browser_->setHtml(QString(databytes));
}

void HelpDisplay::gotoLinkWithKeyword(const QUrl& url, const QString& keyword)
{
	(void)keyword;
	qDebug() << "setting help browser source '" << url << "'";
	auto databytes = help_->fileData(url);
	browser_->setHtml(QString(databytes));
}