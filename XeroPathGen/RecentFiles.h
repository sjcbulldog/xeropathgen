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

#include <QSettings>
#include <QMenu>
#include <list>
#include <string>

class XeroPathGen;

class RecentFiles : public QObject
{
	Q_OBJECT

public:
	RecentFiles(QSettings &settings, QMenu &menu);
	virtual ~RecentFiles();

	void initialize(XeroPathGen* app);

	void setCount(size_t count);
	void addRecentFile(XeroPathGen*app, QString file, bool end = false);
	void removeRecentFile(XeroPathGen* app, QString file);

private:
	void adjustSize();
	void writeRecentFiles();

private:
	size_t count_;
	std::list<QString> files_;
	QSettings &settings_;
	QMenu& menu_;
};

