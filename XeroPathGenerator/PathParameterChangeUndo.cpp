#include "PathParameterChangeUndo.h"
#include "PathParamTreeModel.h"

PathParameterChangeUndo::PathParameterChangeUndo(PathParamTreeModel& model, std::shared_ptr<xero::paths::RobotPath> path) : model_(model)
{
	path_ = path;
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
	path_->setStartVelocity(start_vel_);
	path_->setEndVelocity(end_vel_);
	path_->setMaxVelocity(max_vel_);
	path_->setMaxAccel(max_accel_);
	path_->setMaxJerk(max_jerk_);
	path_->setStartAngle(start_angle_);
	path_->setStartAngleDelay(start_angle_delay_);
	path_->setEndAngle(end_angle_);
	path_->setEndAngleDelay(end_angle_delay_);

	model_.updated();
}