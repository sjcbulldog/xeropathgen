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

#include <PathCollection.h>
#include <RobotParams.h>
#include <Qt>
#include <QAbstractItemModel>

class PathFileTreeModel : public QAbstractItemModel
{
	Q_OBJECT;

public:
	PathFileTreeModel();
	virtual ~PathFileTreeModel();

	void setRobot(std::shared_ptr<xero::paths::RobotParams> robot) {
		robot_ = robot;
	}

	xero::paths::PathCollection& getPathCollection() {
		return paths_;
	}

	size_t getGroupCount() {
		return paths_.size();
	}

	bool isDirty() const {
		return dirty_;
	}

	void setDirty() {
		dirty_ = true;
	}

	void setClean() {
		dirty_ = false;
	}

	QModelIndex index(int row, int col, const QModelIndex& parent) const;
	QModelIndex parent(const QModelIndex& index) const;
	Qt::ItemFlags flags(const QModelIndex& index) const;
	int rowCount(const QModelIndex& parent) const;
	int columnCount(const QModelIndex& parent) const;
	QVariant data(const QModelIndex& index, int role) const;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

	bool containsPathGroup(const std::string& name);
	bool containsPath(const std::string& group, const std::string& name);
	std::shared_ptr<xero::paths::PathGroup> addPathGroup(const std::string& name);
	std::shared_ptr<xero::paths::PathGroup> addPathGroup();
	void insertPathGroup(int row, std::shared_ptr<xero::paths::PathGroup> gr);
	std::shared_ptr<xero::paths::RobotPath> addRobotPath(const std::string& group, const std::string& name);
	void deleteGroup(const std::string& name, bool undo = true);
	void deletePath(const std::string& group, const std::string& name, bool undo = true);
	std::shared_ptr<xero::paths::RobotPath> findPathByName(const std::string& group, const std::string& path);
	void clear();
	void reset(bool clear);
	void renamePath(const std::string& group, const std::string& current, const std::string& newname);
	void renameGroup(const std::string& current, const std::string& newname);
	void addPath(const std::string& group, int index, std::shared_ptr<xero::paths::RobotPath> path);
	std::shared_ptr<xero::paths::RobotPath> addNewPath(std::shared_ptr<xero::paths::PathGroup> group);

	void getAllPaths(std::list<std::shared_ptr<xero::paths::RobotPath>>& results) {
		paths_.getAllPaths(results);
	}

	void convert(const std::string& from, const std::string& to);

signals:
	void pathAdded(std::shared_ptr<xero::paths::RobotPath> path);
	void pathDeleted();
	void groupAdded(std::shared_ptr<xero::paths::PathGroup> group);
	void groupDeleted();

private:
	bool isNameValid(const QString& name);
	void emitPathAdded(std::shared_ptr<xero::paths::RobotPath> path);
	void emitPathDeleted();
	void emitGroupAdded(std::shared_ptr<xero::paths::PathGroup> group);
	void emitGroupDeleted();

private:
	std::shared_ptr<xero::paths::RobotParams> robot_;
	xero::paths::PathCollection paths_;
	bool dirty_;
};

