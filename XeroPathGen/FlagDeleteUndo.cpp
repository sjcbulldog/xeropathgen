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

#include "FlagDeleteUndo.h"

using namespace xero::paths;

FlagDeleteUndo::FlagDeleteUndo(FlagsEditor& editor, int row, const PathFlag& flags) : editor_(editor), deleted_(flags)
{
	row_ = row;
}

FlagDeleteUndo::~FlagDeleteUndo()
{
}

QString FlagDeleteUndo::toString()
{
	QString result("FlagDeleteUndo:");
	result += " " + QString::number(row_);
	result += " " + QString::number(deleted_.after());
	result += " " + QString::number(deleted_.before());
	result += " " + QString(deleted_.name().c_str());

	return result;
}

void FlagDeleteUndo::undo()
{
	editor_.insertFlag(row_, deleted_);
}
