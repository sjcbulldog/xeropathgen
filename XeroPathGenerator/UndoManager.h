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

