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
#include "ConstraintTreeModel.h"

class ConstraintChangeUndo :	public UndoItem
{
public:
	enum class WhichValue
	{
		Before,
		After,
		Velocity
	};

public:
	ConstraintChangeUndo(ConstraintTreeModel &model, int row, WhichValue which, double orig);
	virtual ~ConstraintChangeUndo();

	virtual QString toString();
	virtual void undo();

private:
	int row_;
	ConstraintTreeModel& model_;
	WhichValue which_;
	double value_;
};

