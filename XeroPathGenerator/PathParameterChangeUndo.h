#pragma once
#include "UndoItem.h"
#include "RobotPath.h"

class PathParamTreeModel;
class PathFileTreeModel;

class PathParameterChangeUndo :	public UndoItem
{
public:
	PathParameterChangeUndo(PathParamTreeModel& model, PathFileTreeModel& file, std::shared_ptr<xero::paths::RobotPath> path);
	virtual ~PathParameterChangeUndo();

	virtual QString toString();
	virtual void undo();

private:
	PathParamTreeModel& model_;
	PathFileTreeModel& file_;
	std::string groupname_;
	std::string pathname_;
	double start_vel_;
	double end_vel_;
	double max_vel_;
	double max_accel_;
	double max_jerk_;
	double start_angle_;
	double start_angle_delay_;
	double end_angle_;
	double end_angle_delay_;
};

