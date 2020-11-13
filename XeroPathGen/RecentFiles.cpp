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
#include "RecentFiles.h"
#include "XeroPathGen.h"
#include <algorithm>

RecentFiles::RecentFiles(QSettings &settings, QMenu &menu) : settings_(settings), menu_(menu)
{
	count_ = 4;
}

RecentFiles::~RecentFiles()
{
}

void RecentFiles::setCount(size_t c)
{
	count_ = c;
	adjustSize();
}

void RecentFiles::removeRecentFile(XeroPathGen* app, QString file)
{
	auto it = std::find(files_.begin(), files_.end(), file);
	if (it != files_.end())
		files_.erase(it);

	menu_.clear();
	for (auto& file2 : files_) {
		QAction* action = menu_.addAction(file2);
		connect(action, &QAction::triggered, app, [app, file2] { app->fileOpenWithName(file2); });
	}
}

void RecentFiles::addRecentFile(XeroPathGen *app, QString file, bool end)
{
	auto it = std::find(files_.begin(), files_.end(), file);
	if (it != files_.end())
		files_.erase(it);

	if (end)
		files_.push_back(file);
	else
		files_.push_front(file);

	adjustSize();

	menu_.clear();
	for (auto& file2 : files_) {
		QAction* action = menu_.addAction(file2);
		connect(action, &QAction::triggered, app, [app, file2] { app->fileOpenWithName(file2); });
	}

	if (!end)
		writeRecentFiles();
}

void RecentFiles::adjustSize()
{
	while (files_.size() > count_)
		files_.pop_back();
}

void RecentFiles::initialize(XeroPathGen* app)
{
	size_t i = 0;

	while (true)
	{
		QString entry = "recent/" + QString::number(i);
		if (!settings_.contains(entry))
			break;
		QString value = settings_.value(entry).toString();
		addRecentFile(app, value, true);

		i++;
	}
	adjustSize();
}

void RecentFiles::writeRecentFiles()
{
	size_t i = 0;

	while (true)
	{
		QString entry = "recent/" + QString::number(i);
		if (!settings_.contains(entry))
			break;

		settings_.remove(entry);
		i++;
	}

	i = 0;
	for (auto& file : files_)
	{
		QString entry = "recent/" + QString::number(i);
		settings_.setValue(entry, QVariant(file));
		i++;
	}
}
