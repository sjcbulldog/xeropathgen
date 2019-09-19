#include "WaypointAddUndo.h"
#include "PathFieldView.h"


WaypointAddUndo::WaypointAddUndo(PathFieldView& field, std::shared_ptr<xero::paths::RobotPath> path, size_t index) : field_(field)
{
	index_ = index;
	path_ = path;
}

WaypointAddUndo::~WaypointAddUndo()
{
}

QString WaypointAddUndo::toString()
{
	QString ret("WaypointAddUndo ");
	ret += QString::number(index_);
	return ret;
}

void WaypointAddUndo::undo()
{
	field_.deleteWaypoint(path_, index_);
}