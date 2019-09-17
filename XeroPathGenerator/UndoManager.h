#pragma once

#include "UndoItem.h"
#include <memory>
#include <list>

class UndoManager
{
public:
	UndoManager();
	virtual ~UndoManager();

	void pushUndoStack(std::shared_ptr<UndoItem> item);
	void undo();
	void dump();

	static UndoManager& getUndoManager() {
		return *theone_;
	}

private:
	static UndoManager* theone_;

private:
	size_t depth_;
	std::list<std::shared_ptr<UndoItem>> undo_stack_;
};

