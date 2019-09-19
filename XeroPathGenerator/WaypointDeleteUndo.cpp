#include "WaypointDeleteUndo.h"
#include "PathFieldView.h"

using namespace xero::paths;

WaypointDeleteUndo::WaypointDeleteUndo(PathFieldView& field, std::shared_ptr<RobotPath>path, size_t index, const Pose2d& pt) : fields_(field)
{
	path_ = path;
	index_ = index;
	waypoint_ = pt;
}

WaypointDeleteUndo::~WaypointDeleteUndo()
{

}

QString WaypointDeleteUndo::toString()
{
	QString ret("WaypointDeleteUndo ");
	ret += QString::number(index_);
	return ret;
}

void WaypointDeleteUndo::undo()
{
	fields_.addWaypoint(path_, index_, waypoint_);
}