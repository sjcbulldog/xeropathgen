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
#pragma once

#include <QMainWindow>
#include <QHelpEngine>
#include <QTabWidget>
#include <QSplitter>
#include <QTextBrowser>

class HelpDisplay : public QSplitter
{
	Q_OBJECT

public:
	HelpDisplay(QWidget *parent = Q_NULLPTR);
	~HelpDisplay();

private:
	void gotoLink(const QUrl &url);
	void gotoLinkWithKeyword(const QUrl& url, const QString& keyword);

private:
	QHelpEngine* help_;
	QTabWidget* tabs_;
	QTextBrowser* browser_;
};
