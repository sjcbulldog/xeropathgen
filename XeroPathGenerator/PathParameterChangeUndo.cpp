#include "PathParameterChangeUndo.h"
#include "PathParamTreeModel.h"
#include "PathFileTreeModel.h"
#include "PathGroup.h"

PathParameterChangeUndo::PathParameterChangeUndo(PathParamTreeModel& model, PathFileTreeModel& file, std::shared_ptr<xero::paths::RobotPath> path) : model_(model), file_(file)
{
	groupname_ = path->getParent()->getName();
	pathname_ = path->getName();
	start_vel_ = path->getStartVelocity();
	end_vel_ = path->getEndVelocity();
	max_vel_ = path->getMaxVelocity();
	max_accel_ = path->getMaxAccel();
	max_jerk_ = path->getMaxJerk();
	start_angle_ = path->getStartAngle();
	start_angle_delay_ = path->getStartAngleDelay();
	end_angle_ = path->getEndAngle();
	end_angle_delay_ = path->getEndAngleDelay();
}

PathParameterChangeUndo::~PathParameterChangeUndo()
{
}

QString PathParameterChangeUndo::toString()
{
	QString ret("PathParameterChangeUndo");
	return ret;
}

void PathParameterChangeUndo::undo()
{
	auto path = file_.findPathByName(groupname_, pathname_);
	if (path != nullptr)
	{
		path->setStartVelocity(start_vel_);
		path->setEndVelocity(end_vel_);
		path->setMaxVelocity(max_vel_);
		path->setMaxAccel(max_accel_);
		path->setMaxJerk(max_jerk_);
		path->setStartAngle(start_angle_);
		path->setStartAngleDelay(start_angle_delay_);
		path->setEndAngle(end_angle_);
		path->setEndAngleDelay(end_angle_delay_);

		model_.updated();
	}
}