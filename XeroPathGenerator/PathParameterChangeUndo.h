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

