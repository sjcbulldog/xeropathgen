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