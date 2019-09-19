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
