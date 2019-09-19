#include "WaypointChangedUndo.h"
#include "XeroPathGenerator.h"

WaypointChangedUndo::WaypointChangedUndo(XeroPathGenerator &gen, std::shared_ptr<xero::paths::RobotPath> p, 
	size_t index, const xero::paths::Pose2d& pt) : gen_(gen)
{
	path_ = p;
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
	gen_.updateWaypoint(path_, index_, waypoint_);
}