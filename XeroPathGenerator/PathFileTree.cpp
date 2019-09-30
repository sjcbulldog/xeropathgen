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
#include <cassert>

using namespace xero::paths;

PathFileTree::PathFileTree(QWidget *parent) : QTreeView(parent)
{
}

PathFileTree::~PathFileTree()
{
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
