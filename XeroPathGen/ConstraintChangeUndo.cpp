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

#include "ConstraintChangeUndo.h"

ConstraintChangeUndo::ConstraintChangeUndo(ConstraintTreeModel& model, int row, ConstraintChangeUndo::WhichValue which, double value) : model_(model)
{
	row_ = row;
	which_ = which;
	value_ = value;
}

ConstraintChangeUndo::~ConstraintChangeUndo()
{
}

QString ConstraintChangeUndo::toString()
{
	QString ret("ConstraintChangeUndo ");
	switch (which_)
	{
	case WhichValue::Before:
		ret += "before ";
		break;
	case WhichValue::After:
		ret += "after ";
		break;
	case WhichValue::Velocity:
		ret += "velocity ";
		break;
	}

	ret += QString::number(value_, 'f', 2);
	return ret;
}

void ConstraintChangeUndo::undo()
{
	switch (which_)
	{
	case WhichValue::Before:
		model_.changeBefore(row_, value_);
		break;
	case WhichValue::After:
		model_.changeAfter(row_, value_);
		break;
	case WhichValue::Velocity:
		model_.changeVelocity(row_, value_);
		break;
	}
}