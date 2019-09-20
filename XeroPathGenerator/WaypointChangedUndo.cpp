#include "WaypointChangedUndo.h"
#include "XeroPathGenerator.h"

WaypointChangedUndo::WaypointChangedUndo(XeroPathGenerator &gen, const std::string& group, const std::string& path,
	size_t index, const xero::paths::Pose2d& pt) : gen_(gen)
{
	path_ = path;
	group_ = group;
	index_ = index;
	waypoint_ = pt;
}

WaypointChangedUndo::~WaypointChangedUndo()
{
}

QString WaypointChangedUndo::toString()
{
	return QString("WaypointChangedUndo");
}

void WaypointChangedUndo::undo()
{
	gen_.updateWaypoint(group_, path_, index_, waypoint_);
}