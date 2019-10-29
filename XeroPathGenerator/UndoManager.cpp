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

#include "UndoManager.h"
#include <QDebug>
#include <cassert>

UndoManager* UndoManager::theone_ = nullptr;

UndoManager::UndoManager()
{
	assert(theone_ == nullptr);
	theone_ = this;
	depth_ = 64;
}

UndoManager::~UndoManager()
{
	assert(theone_ == this);
	theone_ = nullptr;
}

void UndoManager::pushUndoStack(std::shared_ptr<UndoItem> item)
{
	undo_stack_.push_front(item);
	while (undo_stack_.size() > depth_)
		undo_stack_.pop_back();
}

void UndoManager::undo()
{
	if (undo_stack_.size() > 0)
	{
		auto item = undo_stack_.front();
		undo_stack_.pop_front();
		item->undo();
	}
}

void UndoManager::dump()
{
	int i = 1;

	for (auto item : undo_stack_)
	{
		qDebug() << QString::number(i) << " " << item->toString();
		i++;
	}
}
