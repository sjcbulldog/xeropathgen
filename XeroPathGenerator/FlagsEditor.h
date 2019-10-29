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

#include <QWidget>
#include "ui_FlagsEditor.h"
#include "FlagsTreeModel.h"
#include <RobotPath.h>
#include <PathFlag.h>
#include <QMenu>

class FlagsTreeModel;

class FlagsEditor : public QWidget
{
	Q_OBJECT

public:
	FlagsEditor(QWidget *parent = Q_NULLPTR);
	~FlagsEditor();

	void setPath(std::shared_ptr<xero::paths::RobotPath> path) {
		model_.setPath(path);
		path_ = path;
		enableDisable();
	}

	void deleteFlag(int row);
	void insertFlag(int row, const xero::paths::PathFlag& flag);
	void enableDisable();

signals:
	void flagAdded();
	void flagRemoved();
	void flagChanged();

private:
	void emitFlagAdded();
	void emitFlagRemoved();
	void emitFlagChanged();

	void addOne();
	void deleteOne();
	void contextMenu(const QPoint& pt);
	void deleteIDFlag();
	std::string getNewName();

private:
	Ui::FlagsEditor ui;
	std::shared_ptr<xero::paths::RobotPath> path_;
	FlagsTreeModel model_;
	int row_;
	QMenu* flag_selected_menu_;
	QMenu* none_selected_menu_;
};
