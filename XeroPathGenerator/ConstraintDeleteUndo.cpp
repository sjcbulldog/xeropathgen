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

#include "ConstraintDeleteUndo.h"

using namespace xero::paths;

ConstraintDeleteUndo::ConstraintDeleteUndo(ConstraintEditor& editor, int row, const DistanceVelocityConstraint& con) : editor_(editor), deleted_(con)
{
	row_ = row;
}

ConstraintDeleteUndo::~ConstraintDeleteUndo()
{
}

QString ConstraintDeleteUndo::toString()
{
	QString result("ConstraintDeleteUndo:");
	result += " " + QString::number(row_);
	result += " " + QString::number(deleted_.getAfter());
	result += " " + QString::number(deleted_.getBefore());
	result += " " + QString::number(deleted_.getVelocity());

	return result;
}

void ConstraintDeleteUndo::undo()
{
	editor_.addConstraint(row_, deleted_);
}