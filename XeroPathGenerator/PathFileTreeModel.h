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

#include <Qt>
#include <QAbstractItemModel>
#include <PathCollection.h>

class PathFileTreeModel : public QAbstractItemModel
{
public:
	PathFileTreeModel();
	virtual ~PathFileTreeModel();


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
	std::shared_ptr<xero::paths::RobotPath> addRobotPath(const std::string& group, const std::string& name);
	void deleteGroup(const std::string& name);
	void deletePath(const std::string& group, const std::string& name);
	std::shared_ptr<xero::paths::RobotPath> findPathByName(const std::string& group, const std::string& path);
	void clear();
	void reset();

	void getAllPaths(std::list<std::shared_ptr<xero::paths::RobotPath>>& results) {
		paths_.getAllPaths(results);
	}

	void convert(const std::string& from, const std::string& to);

private:
	bool isNameValid(const QString& name);

private:
	xero::paths::PathCollection paths_;
	bool dirty_;
};

