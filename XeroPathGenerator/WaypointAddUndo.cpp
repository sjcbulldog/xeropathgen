#include "WaypointAddUndo.h"
#include "PathFieldView.h"


WaypointAddUndo::WaypointAddUndo(PathFieldView& field, const std::string& group, const std::string& path, size_t index) : field_(field)
{
	index_ = index;
	path_ = path;
	group_ = group;
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
	field_.deleteWaypoint(path_, group_, index_);
}