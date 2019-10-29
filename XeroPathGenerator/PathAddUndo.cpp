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

#include "PathAddUndo.h"
#include "PathFileTreeModel.h"

PathAddUndo::PathAddUndo(PathFileTreeModel& model, const std::string& group, const std::string& path) : model_(model)
{
	path_ = path;
	group_ = group;
}

PathAddUndo::~PathAddUndo()
{
}

QString PathAddUndo::toString() 
{
	QString ret("PathAddUndo ");
	ret += group_.c_str();
	ret += " ";
	ret += path_.c_str();
	return ret;
}

void PathAddUndo::undo()
{
	model_.deletePath(group_, path_, false);
}
