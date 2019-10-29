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
#include "FlagsEditor.h"
#include <PathFlag.h>

class FlagDeleteUndo : public UndoItem
{
public:
	FlagDeleteUndo(FlagsEditor& editor, int row, const xero::paths::PathFlag& flag);
	virtual ~FlagDeleteUndo();
	virtual QString toString();
	virtual void undo();

private:
	FlagsEditor& editor_;

	int row_;

	//
	// The Flags that was deleted
	//
	xero::paths::PathFlag deleted_;
};

