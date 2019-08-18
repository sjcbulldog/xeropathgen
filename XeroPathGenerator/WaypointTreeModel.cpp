#include "WaypointTreeModel.h"

using namespace xero::paths;

WaypointTreeModel::WaypointTreeModel()
{
}

WaypointTreeModel::~WaypointTreeModel()
{

}

QModelIndex WaypointTreeModel::index(int row, int col, const QModelIndex& parent) const
{
  	(void)parent ;
	return createIndex(row, col);
}

QModelIndex WaypointTreeModel::parent(const QModelIndex& index) const
{
  	(void)index ;
	return QModelIndex();
}

Qt::ItemFlags WaypointTreeModel::flags(const QModelIndex& index) const
{
	if (!enabled_)
		return 0;

	if (index.column() == 0 || index.row() > 2)
		return Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled;

	return Qt::ItemFlag::ItemIsEditable | Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled;
}

int WaypointTreeModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid() || !enabled_)
		return 0;

	return 5;
}

int WaypointTreeModel::columnCount(const QModelIndex& parent) const
{
  	(void)parent ;
	
	if (!enabled_)
		return 0;

	return 2;
}

QVariant WaypointTreeModel::headerData(int section, Qt::Orientation orient, int role) const
{
	QVariant ret;

	if (role == Qt::DisplayRole && orient == Qt::Orientation::Horizontal)
	{
		if (section == 0)
			ret = "Name";
		else if (section == 1)
			ret = "Value";
	}

	return ret;
}

QVariant WaypointTreeModel::data(const QModelIndex& index, int role) const
{
	QVariant ret;

	if (role != Qt::DisplayRole && role != Qt::EditRole)
		return ret;

	switch (index.row())
	{
	case 0:
		if (index.column() == 0)
			ret = "X";
		else
			ret = QString::number(waypoint_.getTranslation().getX(), 'f', 1);
		break;
	case 1:
		if (index.column() == 0)
			ret = "Y";
		else
			ret = QString::number(waypoint_.getTranslation().getY(), 'f', 1);
		break;
	case 2:
		if (index.column() == 0)
			ret = "Heading";
		else
			ret = QString::number(waypoint_.getRotation().toDegrees(), 'f', 1);
		break;
	case 3:
		if (index.column() == 0)
			ret = "Index";
		else
			ret = QString::number(index_);
		break;
	case 4:
		if (index.column() == 0)
			ret = "Distance";
		else
			ret = QString::number(dist_, 'f', 1);
		break;
	}

	return ret;
}

bool WaypointTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  	(void)role ;
	
	if (index.isValid())
	{
		assert(index.column() == 1);

		if (!value.canConvert<double>())
			return false;
		double v = value.toDouble();

		switch (index.row())
		{
		case 0:
			waypoint_ = Pose2d(Translation2d(v, waypoint_.getTranslation().getY()), waypoint_.getRotation());
			break;
		case 1:
			waypoint_ = Pose2d(Translation2d(waypoint_.getTranslation().getX(), v), waypoint_.getRotation());
			break;
		case 2:
			waypoint_ = Pose2d(waypoint_.getTranslation(), Rotation2d::fromDegrees(v));
			break;
		}

		emit dataChanged(index, index);
	}
	return true;
}

void WaypointTreeModel::reset()
{
	beginResetModel();
	endResetModel();
}
