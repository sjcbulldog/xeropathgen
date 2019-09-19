#pragma once
#include "UndoItem.h"
#include "ConstraintTreeModel.h"

class ChangeContraintUndo :	public UndoItem
{
public:
	enum class WhichValue
	{
		Before,
		After,
		Velocity
	};

public:
	ChangeContraintUndo(ConstraintTreeModel &model, int row, WhichValue which, double orig);
	virtual ~ChangeContraintUndo();

	virtual QString toString();
	virtual void undo();

private:
	int row_;
	ConstraintTreeModel& model_;
	WhichValue which_;
	double value_;
};

