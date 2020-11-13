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

#include "FlagChangedUndo.h"

FlagChangedUndo::FlagChangedUndo(FlagsTreeModel& model, int row, FlagChangedUndo::WhichValue which, double value) : model_(model)
{
	assert(which == WhichValue::Before || which == WhichValue::After);
	row_ = row;
	which_ = which;
	value_ = value;
}

FlagChangedUndo::FlagChangedUndo(FlagsTreeModel& model, int row, QString value) : model_(model)
{
	row_ = row;
	which_ = WhichValue::Name;
	str_value_ = value;
}

FlagChangedUndo::~FlagChangedUndo()
{
}

QString FlagChangedUndo::toString()
{
	QString ret("FlagChangedUndo ");
	switch (which_)
	{
	case WhichValue::Before:
		ret += "before ";
		ret += QString::number(value_, 'f', 2);
		break;
	case WhichValue::After:
		ret += "after ";
		ret += QString::number(value_, 'f', 2);
		break;
	case WhichValue::Name:
		ret += "name ";
		ret += str_value_;
		break;
	}


	return ret;
}

void FlagChangedUndo::undo()
{
	switch (which_)
	{
	case WhichValue::Before:
		model_.changeBefore(row_, value_);
		break;
	case WhichValue::After:
		model_.changeAfter(row_, value_);
		break;
	case WhichValue::Name:
		model_.changeName(row_, str_value_.toStdString());
		break;
	}
}