//
// Copyright 2019 Jack W. Griffin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
//

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