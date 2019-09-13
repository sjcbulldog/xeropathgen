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
#include <QAbstractItemModel>
#include <memory>

class PathParamTreeModel : public QAbstractItemModel
{
public:
	PathParamTreeModel();
	virtual ~PathParamTreeModel();

	virtual QModelIndex index(int row, int col, const QModelIndex& parent) const;
	virtual QModelIndex parent(const QModelIndex& index) const;
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;
	virtual int rowCount(const QModelIndex& parent) const;
	virtual int columnCount(const QModelIndex& parent) const;
	virtual QVariant data(const QModelIndex& index, int role) const;
	virtual QVariant headerData(int section, Qt::Orientation orient, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

	void setPath(std::shared_ptr<xero::paths::RobotPath> path) {
		path_ = path;
		reset();
	}

	void setGenerator(std::shared_ptr<Generator> gen) {
		generator_ = gen;
		reset();
	}

	void reset();

	void setAnglesRequired(bool b) {
		angles_required_ = b;
		if (b)
			read_write_rows_ = 9;
		else
			read_write_rows_ = 5;

		reset();
	}

	
private:
	static std::vector<std::string> RowLabelsAngles;
	static std::vector<std::string> RowLabelsNoAngles;

private:
	int read_write_rows_;
	std::shared_ptr<xero::paths::RobotPath> path_;
	std::shared_ptr<Generator> generator_;
	bool angles_required_;
};

