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

#include "ConstraintAddUndo.h"

ConstraintAddUndo::ConstraintAddUndo(ConstraintEditor& editor, int row) : editor_(editor)
{
	row_ = row;
}

ConstraintAddUndo::~ConstraintAddUndo()
{

}

QString ConstraintAddUndo::toString()
{
	QString ret("ConstraintAddUndo");
	ret += " " + QString::number(row_);
	return ret;
}

void ConstraintAddUndo::undo()
{
	editor_.deleteConstraint(row_);
}