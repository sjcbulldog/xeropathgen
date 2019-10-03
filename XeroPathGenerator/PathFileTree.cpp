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
#include "PathFileTree.h"
#include "PathFileTreeModel.h"
#include <QKeyEvent>
#include <QMenu>
#include <cassert>

using namespace xero::paths;

PathFileTree::PathFileTree(QWidget *parent) : QTreeView(parent)
{
	none_selected_menu_ = nullptr;
	path_selected_menu_ = nullptr;
	group_selected_menu_ = nullptr;

	setContextMenuPolicy(Qt::DefaultContextMenu);
}

PathFileTree::~PathFileTree()
{
}

void PathFileTree::addPathGroup()
{
	PathFileTreeModel* tmodel = dynamic_cast<PathFileTreeModel*>(model());
	assert(tmodel != nullptr);

	(void)tmodel->addPathGroup();
}

void PathFileTree::addPath()
{
	PathFileTreeModel* tmodel = dynamic_cast<PathFileTreeModel*>(model());
	assert(tmodel != nullptr);

	(void)tmodel->addNewPath(menu_group_);
}

void PathFileTree::deletePath()
{
	PathFileTreeModel* tmodel = dynamic_cast<PathFileTreeModel*>(model());
	assert(tmodel != nullptr);

	tmodel->deletePath(menu_path_->getParent()->getName(), menu_path_->getName());
}

void PathFileTree::deleteGroup()
{
	PathFileTreeModel* tmodel = dynamic_cast<PathFileTreeModel*>(model());
	assert(tmodel != nullptr);

	tmodel->deleteGroup(menu_group_->getName());
}

void PathFileTree::contextMenuEvent(QContextMenuEvent* ev)
{
	QAction *action;
	PathFileTreeModel* tmodel = dynamic_cast<PathFileTreeModel*>(model());
	assert(tmodel != nullptr);

	menu_path_ = nullptr;
	menu_group_ = nullptr;

	QModelIndex index = indexAt(ev->pos());
	if (!index.isValid())
	{
		//
		// Not on any item
		//
		if (none_selected_menu_ == nullptr)
		{
			none_selected_menu_ = new QMenu();
			action = none_selected_menu_->addAction(tr("Add Path Group"));
			(void)connect(action, &QAction::triggered, this, &PathFileTree::addPathGroup);
		}
		none_selected_menu_->exec(mapToGlobal(ev->pos()));
	}
	else if (!index.parent().isValid())
	{
		//
		// On a group item
		//
		if (group_selected_menu_ == nullptr)
		{
			group_selected_menu_ = new QMenu();
			action = group_selected_menu_->addAction(tr("Add Path"));
			(void)connect(action, &QAction::triggered, this, &PathFileTree::addPath);

			action = group_selected_menu_->addAction(tr("Delete Group"));
			(void)connect(action, &QAction::triggered, this, &PathFileTree::deleteGroup);
		}

		menu_group_ = tmodel->getPathCollection().getGroupByIndex(index.row());
		group_selected_menu_->exec(mapToGlobal(ev->pos()));
	}
	else
	{
		//
		// On a path item
		//
		if (path_selected_menu_ == nullptr)
		{
			path_selected_menu_ = new QMenu();
			action = path_selected_menu_->addAction(tr("Delete Path"));
			(void)connect(action, &QAction::triggered, this, &PathFileTree::deletePath);
		}

		auto gr = tmodel->getPathCollection().getGroupByIndex(index.parent().row());
		menu_path_ = gr->getPathByIndex(index.row());
		path_selected_menu_->exec(mapToGlobal(ev->pos()));
	}
}

void PathFileTree::keyPressEvent(QKeyEvent* ev)
{
	PathFileTreeModel* tmodel = dynamic_cast<PathFileTreeModel*>(model());
	assert(tmodel != nullptr);

	if (ev->key() == Qt::Key::Key_Delete)
	{
		if (isGroupSelected())
		{
			tmodel->deleteGroup(getSelectedGroup()->getName());
		}
		else if (isPathSelected())
		{
			auto path = getSelectedPath();
			tmodel->deletePath(path->getParent()->getName(), path->getName());
		}
	}
	else if (ev->key() == Qt::Key::Key_Insert)
	{
		if (isGroupSelected())
		{
			tmodel->addNewPath(getSelectedGroup());
		}
	}
}

bool PathFileTree::isGroupSelected()
{
	QModelIndex index = currentIndex();
	if (!index.isValid())
		return false;

	PathBase* base = static_cast<PathBase*>(index.internalPointer());
	PathGroup* group = dynamic_cast<PathGroup*>(base);
	return group != nullptr;
}

bool PathFileTree::isPathSelected()
{
	QModelIndex index = currentIndex();
	if (!index.isValid())
		return false;

	PathBase* base = static_cast<PathBase*>(index.internalPointer());
	RobotPath* group = dynamic_cast<RobotPath*>(base);
	return group != nullptr;
}

std::shared_ptr<xero::paths::PathGroup> PathFileTree::getSelectedGroup()
{
	if (!isGroupSelected())
		return nullptr;

	QModelIndex index = currentIndex();
	QVariant groupname = model_->data(index, Qt::DisplayRole);
	return model_->getPathCollection().getGroupByName(groupname.toString().toStdString());
}

std::shared_ptr<xero::paths::RobotPath> PathFileTree::getSelectedPath()
{
	if (!isPathSelected())
		return nullptr;

	QModelIndex pathindex = currentIndex();
	QModelIndex groupindex = pathindex.parent();

	QVariant pathname = model_->data(pathindex, Qt::DisplayRole);
	QVariant groupname = model_->data(groupindex, Qt::DisplayRole);

	auto group = model_->getPathCollection().getGroupByName(groupname.toString().toStdString());
	if (group == nullptr)
		return nullptr;

	return group->findPathByName(pathname.toString().toStdString());
}
