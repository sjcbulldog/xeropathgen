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
#include <RobotPath.h>
#include <Pose2d.h>

class XeroPathGenerator;

class WaypointChangedUndo :	public UndoItem
{
public:
	WaypointChangedUndo(XeroPathGenerator& gen, const std::string &group, const std::string &path, size_t index, const xero::paths::Pose2d& pt);
	virtual ~WaypointChangedUndo();

	virtual QString toString();
	virtual void undo();

private:
	XeroPathGenerator& gen_;
	std::string group_;
	std::string path_;
	size_t index_;
	xero::paths::Pose2d waypoint_;
};

