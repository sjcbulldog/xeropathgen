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

#include "PathDeleteUndo.h"
#include "PathFileTreeModel.h"

PathDeleteUndo::PathDeleteUndo(PathFileTreeModel& model, const std::string& group, int index, std::shared_ptr<xero::paths::RobotPath> path) : model_(model)
{
	path_ = path;
	index_ = index;
	group_ = group;
}

PathDeleteUndo::~PathDeleteUndo()
{
}

QString PathDeleteUndo::toString()
{
	QString ret("PathDeleteUndo");
	return ret;
}

void PathDeleteUndo::undo()
{
	model_.addPath(group_, index_, path_);
}