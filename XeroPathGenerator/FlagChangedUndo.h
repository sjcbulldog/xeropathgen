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
#include "FlagsTreeModel.h"

class FlagChangedUndo : public UndoItem
{
public:
	enum class WhichValue
	{
		Before,
		After,
		Name
	};

public:
	FlagChangedUndo(FlagsTreeModel& model, int row, WhichValue which, double orig);
	FlagChangedUndo(FlagsTreeModel& model, int row, QString orig);
	virtual ~FlagChangedUndo();

	virtual QString toString();
	virtual void undo();

private:
	int row_;
	FlagsTreeModel& model_;
	WhichValue which_;
	double value_;
	QString str_value_;
};

