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

#include <QAbstractItemModel>
#include <Pose2d.h>

class WaypointTreeModel : public QAbstractItemModel
{
public:
	WaypointTreeModel();
	virtual ~WaypointTreeModel();

	virtual QModelIndex index(int row, int col, const QModelIndex& parent) const;
	virtual QModelIndex parent(const QModelIndex& index) const;
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;
	virtual int rowCount(const QModelIndex& parent) const;
	virtual int columnCount(const QModelIndex& parent) const;
	virtual QVariant data(const QModelIndex& index, int role) const;
	virtual QVariant headerData(int section, Qt::Orientation orient, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

	void setWaypoint(const xero::paths::Pose2d& way, size_t index, double dist) {
		waypoint_ = way;
		dist_ = dist;
		index_ = index;
		reset();
	}

	const xero::paths::Pose2d& getWaypoint() const {
		return waypoint_;
	}

	void enable(bool e) {
		enabled_ = e;
		reset();
	}

	void reset();

private:
	xero::paths::Pose2d waypoint_;
	double dist_;
	size_t index_;
	bool enabled_;
};

