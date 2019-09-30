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
#include "PathFileTreeModel.h"
#include "PathNameChangeUndo.h"
#include "AddPathUndo.h"
#include "AddGroupUndo.h"
#include "DeleteGroupUndo.h"
#include "DeletePathUndo.h"
#include "GroupNameChangeUndo.h"
#include "UndoManager.h"
#include <QMessageBox>

using namespace xero::paths;

PathFileTreeModel::PathFileTreeModel()
{
	dirty_ = false;
}

PathFileTreeModel::~PathFileTreeModel()
{
}

void PathFileTreeModel::emitPathAdded(std::shared_ptr<RobotPath> path)
{
	emit pathAdded(path);
}

void PathFileTreeModel::emitPathDeleted()
{
	emit pathDeleted();
}

void PathFileTreeModel::emitGroupAdded(std::shared_ptr<PathGroup> group)
{
	emit groupAdded(group);
}

void PathFileTreeModel::emitGroupDeleted()
{
	emit groupDeleted();
}

bool PathFileTreeModel::isNameValid(const QString &name)
{
	//
	// Rule 1: names must be at least one character long
	//
	if (name.length() == 0)
		return false;

	//
	// Rule 2: names must start with an alpha or underscore
	//
	if (!name[0].isLetter() && name[0] != '_')
		return false;

	//
	// Rule 3: all other characters must be alpha, number, or underscore
	//
	for (int i = 1; i < name.length(); i++)
	{
		if (!name[i].isLetterOrNumber() && name[i] != '_')
			return false;
	}

	return true;
}

Qt::ItemFlags PathFileTreeModel::flags(const QModelIndex& index) const
{
  	(void)index ;
	return Qt::ItemFlag::ItemIsEditable | Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled;
}

QModelIndex PathFileTreeModel::index(int row, int col, const QModelIndex& parent) const
{
	if (!parent.isValid())
	{
		//
		// Top most level, return a PathGroup
		//
		if (col > 0)
			return QModelIndex();

		auto group = paths_.getGroupByIndex(row);
		if (group == nullptr)
			return QModelIndex();

		return createIndex(row, col, group.get());
	}
	else
	{
		if (col > 0)
			return QModelIndex();

		//
		// Should be a PathGroup, return a RobotPath
		//
		std::shared_ptr<PathGroup> group;
		PathGroup* target = (PathGroup*)parent.internalPointer();

		for (size_t i = 0; i < paths_.size(); i++)
		{
			auto gr = paths_.getGroupByIndex(i);
			if (gr.get() == target)
			{
				group = gr;
				break;
			}
		}

		if (group == nullptr)
			return QModelIndex();

		auto path = group->getPathByIndex(row);
		if (path == nullptr)
			return QModelIndex();

		return createIndex(row, col, path.get());
	}
}

QModelIndex PathFileTreeModel::parent(const QModelIndex& index) const
{
	if (!index.isValid())
		return QModelIndex();

	PathBase* base = static_cast<PathBase*>(index.internalPointer());
	RobotPath* path = dynamic_cast<RobotPath*>(base);
	if (path != nullptr)
	{
		auto group = path->getParent();
		auto index2 = static_cast<int>(paths_.getIndexForGroup(group));
		return createIndex(index2, 0, group.get());
	}

	return QModelIndex();
}

int PathFileTreeModel::rowCount(const QModelIndex& parent) const
{
	if (!parent.isValid())
		return static_cast<int>(paths_.size());

	PathBase* base = static_cast<PathBase*>(parent.internalPointer());
	RobotPath* path = dynamic_cast<RobotPath*>(base);
	if (path != nullptr)
		return 0;

	PathGroup* group = dynamic_cast<PathGroup*>(base);
	if (group != nullptr)
		return static_cast<int>(group->size());

	return 0;
}

int PathFileTreeModel::columnCount(const QModelIndex& parent) const
{
  	(void)parent ;
	if (paths_.size() == 0)
		return 0;

	return 1;
}

QVariant PathFileTreeModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	PathBase* base = static_cast<PathBase*>(index.internalPointer());
	const std::string& name = base->getName();
	QVariant ret;

	switch (role)
	{
	case Qt::DisplayRole:
		ret = QVariant(name.c_str());
		break;
	case Qt::EditRole:
		ret = QVariant(name.c_str());
		break;
	default:
		ret = QVariant();
		break;
	}
	return ret;
}

bool PathFileTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (!index.isValid())
		return true;

	if (role == Qt::EditRole)
	{
		QString name = value.toString();
		if (!isNameValid(name))
		{
			QMessageBox box(QMessageBox::Icon::Critical,
				"Error", "invalid path or pathgroup name, must start with letter or underscore, and be all letters, numbers, or underscores", QMessageBox::StandardButton::Ok);
			box.exec();
			return false;
		}

		PathBase* base = static_cast<PathBase*>(index.internalPointer());
		RobotPath* path = dynamic_cast<RobotPath*>(base);
		if (path != nullptr)
		{
			auto undo = std::make_shared<PathNameChangeUndo>(*this, path->getParent()->getName(), name.toStdString(), path->getName());
			UndoManager::getUndoManager().pushUndoStack(undo);
		}
		else
		{
			PathGroup* group = dynamic_cast<PathGroup*>(base);
			assert(group != nullptr);

			auto undo = std::make_shared<GroupNameChangeUndo>(*this, name.toStdString(), group->getName());
			UndoManager::getUndoManager().pushUndoStack(undo);
		}

		base->setName(value.toString().toStdString());
		emit dataChanged(index, index);
		dirty_ = true;
	}

	return true;
}

bool PathFileTreeModel::containsPathGroup(const std::string& name)
{
	return paths_.hasGroup(name);
}

bool PathFileTreeModel::containsPath(const std::string& group, const std::string& name)
{
	return paths_.findPathByName(group, name) != nullptr;
}

std::shared_ptr<PathGroup> PathFileTreeModel::addPathGroup(const std::string& name)
{
	QModelIndex root;


	auto undo = std::make_shared<AddGroupUndo>(*this, name);
	UndoManager::getUndoManager().pushUndoStack(undo);

	auto group = paths_.addGroup(name);
	dirty_ = true;
	reset(false);

	emitGroupAdded(group);

	return group;
}

void PathFileTreeModel::insertPathGroup(int row, std::shared_ptr<PathGroup> gr)
{
	if (paths_.getGroupByName(gr->getName()) == nullptr)
	{
		paths_.addGroup(row, gr);
		reset(false);
	}

	emitGroupAdded(gr);
}

void PathFileTreeModel::addPath(const std::string& group, int index, std::shared_ptr<RobotPath> path)
{
	auto gr = paths_.getGroupByName(group);
	if (gr != nullptr)
	{
		gr->insertPath(index, path);
		reset(false);
		emitPathAdded(path);
	}
}

std::shared_ptr<RobotPath> PathFileTreeModel::addRobotPath(const std::string& group, const std::string& name)
{
	std::shared_ptr<PathGroup> grobj = paths_.getGroupByName(group);
	if (grobj == nullptr)
		return nullptr ;

	auto row = static_cast<int>(paths_.getIndexForGroup(grobj));

	QModelIndex parent = createIndex(row, 0, grobj.get());
	beginInsertRows(parent, static_cast<int>(grobj->size()), static_cast<int>(grobj->size()));
	auto undo = std::make_shared<AddPathUndo>(*this, group, name);
	UndoManager::getUndoManager().pushUndoStack(undo);
	auto path = paths_.addPath(group, name);
	path->addPoint(Pose2d(0.0, 0.0, Rotation2d::fromDegrees(0.0)));
	path->addPoint(Pose2d(100.0, 0.0, Rotation2d::fromDegrees(0.0)));
	path->generateSplines();
	endInsertRows();
	dirty_ = true;

	emitPathAdded(path);
	return path;
}

std::shared_ptr<xero::paths::RobotPath> PathFileTreeModel::addNewPath(std::shared_ptr<xero::paths::PathGroup> group)
{
	size_t index = 1;
	std::string name = "NewPath";

	while (containsPath(group->getName(), name))
		name = "NewPath_" + std::to_string(++index);

	auto path = addRobotPath(group->getName(), name);

	path->setMaxVelocity(robot_->getMaxVelocity());
	path->setMaxAccel(robot_->getMaxAccel());
	path->setMaxJerk(robot_->getMaxJerk());

	emitPathAdded(path);
	return path;
}

void PathFileTreeModel::deleteGroup(const std::string& group, bool undo)
{
	auto gr = paths_.getGroupByName(group);
	int index = static_cast<int>(paths_.getIndexForGroup(gr));
	paths_.delGroup(group);

	if (undo && gr != nullptr)
	{
		auto undoobj = std::make_shared<DeleteGroupUndo>(*this, index, gr);
		UndoManager::getUndoManager().pushUndoStack(undoobj);
	}

	dirty_ = true;
	reset(false);

	emitGroupDeleted();
}

void PathFileTreeModel::deletePath(const std::string &group, const std::string &path, bool undo)
{
	auto gr = paths_.getGroupByName(group);
	auto p = paths_.findPathByName(group, path);
	int index = static_cast<int>(gr->getIndexFromPath(p));

	if (undo)
	{
		auto undoobj = std::make_shared<DeletePathUndo>(*this, group, index, p);
		UndoManager::getUndoManager().pushUndoStack(undoobj);
	}

	paths_.delPath(group, path);
	dirty_ = true;
	reset(false);

	emitPathDeleted();
}

std::shared_ptr<RobotPath> PathFileTreeModel::findPathByName(const std::string& group, const std::string& path)
{
	return paths_.findPathByName(group, path);
}

void PathFileTreeModel::clear()
{
	beginResetModel();
	paths_.clear();
	endResetModel();
	dirty_ = true;
}

void PathFileTreeModel::reset(bool clear)
{
	beginResetModel();
	endResetModel();
	if (clear)
		dirty_ = false;
}

void PathFileTreeModel::renamePath(const std::string& group, const std::string& current, const std::string& newname)
{
	auto path = findPathByName(group, current);
	path->setName(newname);
	reset(false);
}

void PathFileTreeModel::renameGroup(const std::string& current, const std::string& newname)
{
	auto group = paths_.getGroupByName(current);
	group->setName(newname);
	reset(false);
}

void PathFileTreeModel::convert(const std::string& old, const std::string& newunits)
{
	std::list<std::shared_ptr<RobotPath>> allpaths;
	paths_.getAllPaths(allpaths);

	for (auto path : allpaths)
		path->convert(old, newunits);
}
