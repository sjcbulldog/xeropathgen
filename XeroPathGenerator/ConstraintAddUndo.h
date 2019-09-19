#pragma once
#include "UndoItem.h"
#include "ConstraintEditor.h"

class ConstraintAddUndo : public UndoItem
{
public:
	ConstraintAddUndo(ConstraintEditor& model, int row);
	virtual ~ConstraintAddUndo();

	virtual QString toString();
	virtual void undo();

private:
	ConstraintEditor& model_;
	int row_;
};

