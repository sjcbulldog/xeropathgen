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

#include "WaypointAddUndo.h"
#include "PathFieldView.h"


WaypointAddUndo::WaypointAddUndo(PathFieldView& field, const std::string& group, const std::string& path, size_t index) : field_(field)
{
	index_ = index;
	path_ = path;
	group_ = group;
}

WaypointAddUndo::~WaypointAddUndo()
{
}

QString WaypointAddUndo::toString()
{
	QString ret("WaypointAddUndo ");
	ret += QString::number(index_);
	return ret;
}

void WaypointAddUndo::undo()
{
	field_.deleteWaypoint(path_, group_, index_);
}