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

#include <QObject>
#include "UndoItem.h"
#include <memory>
#include <list>

class UndoManager : public QObject
{
	Q_OBJECT;
public:
	UndoManager();
	virtual ~UndoManager();

	void pushUndoStack(std::shared_ptr<UndoItem> item);
	void undo();
	void dump();

	bool hasUndoInfo() const {
		return undo_stack_.size() > 0;
	}

	static UndoManager& getUndoManager() {
		return *theone_;
	}


private:
	static UndoManager* theone_;

private:
	size_t depth_;
	std::list<std::shared_ptr<UndoItem>> undo_stack_;
};

