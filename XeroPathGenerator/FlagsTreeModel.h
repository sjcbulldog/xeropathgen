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

#include "Generator.h"
#include <RobotPath.h>
#include <Qt>
#include <QAbstractItemModel>

class FlagsTreeModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	FlagsTreeModel();
	virtual ~FlagsTreeModel();

	void setPath(std::shared_ptr<xero::paths::RobotPath> path) {
		path_ = path;
		reset();
	}

	QModelIndex index(int row, int col, const QModelIndex& parent) const;
	QModelIndex parent(const QModelIndex& index) const;
	Qt::ItemFlags flags(const QModelIndex& index) const;
	int rowCount(const QModelIndex& parent) const;
	int columnCount(const QModelIndex& parent) const;
	virtual QVariant headerData(int section, Qt::Orientation orient, int role = Qt::DisplayRole) const;
	QVariant data(const QModelIndex& index, int role) const;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

	void changeBefore(int row, double value);
	void changeAfter(int row, double value);
	void changeName(int row, const std::string& name);

	void reset();


signals:
	void flagChanged();

private:
	void emitFlagChanged();
	bool isRowValid(int row) const;

private:
	std::shared_ptr<xero::paths::RobotPath> path_;
};
