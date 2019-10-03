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

#include "PathFileTreeModel.h"
#include <QTreeView>

class PathFileTree : public QTreeView
{
	Q_OBJECT

public:
	PathFileTree(QWidget *parent = Q_NULLPTR);
	~PathFileTree();

	virtual void setModel(QAbstractItemModel *model) {
	  	model_ = dynamic_cast<PathFileTreeModel *>(model) ;
		QTreeView::setModel(model);
	}

	bool isGroupSelected();
	bool isPathSelected();
	std::shared_ptr<xero::paths::PathGroup> getSelectedGroup();
	std::shared_ptr<xero::paths::RobotPath> getSelectedPath();

protected:
	virtual void keyPressEvent(QKeyEvent* event) override;
	virtual void contextMenuEvent(QContextMenuEvent* ev) override;

private:
	void addPathGroup();
	void addPath();
	void deleteGroup();
	void deletePath();

private:
	PathFileTreeModel* model_;
	QMenu* path_selected_menu_;
	QMenu* group_selected_menu_;
	QMenu* none_selected_menu_;
	std::shared_ptr<xero::paths::PathGroup> menu_group_;
	std::shared_ptr<xero::paths::RobotPath> menu_path_;
};
