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

