#include "ChangeContraintUndo.h"

ChangeContraintUndo::ChangeContraintUndo(ConstraintTreeModel& model, int row, ChangeContraintUndo::WhichValue which, double value) : model_(model)
{
	row_ = row;
	which_ = which;
	value_ = value;
}

ChangeContraintUndo::~ChangeContraintUndo()
{
}

QString ChangeContraintUndo::toString()
{
	QString ret("ChangeContraintUndo ");
	switch (which_)
	{
	case WhichValue::Before:
		ret += "before ";
		break;
	case WhichValue::After:
		ret += "after ";
		break;
	case WhichValue::Velocity:
		ret += "velocity ";
		break;
	}

	ret += QString::number(value_, 'f', 2);
	return ret;
}

void ChangeContraintUndo::undo()
{
	switch (which_)
	{
	case WhichValue::Before:
		model_.changeBefore(row_, value_);
		break;
	case WhichValue::After:
		model_.changeAfter(row_, value_);
		break;
	case WhichValue::Velocity:
		model_.changeVelocity(row_, value_);
		break;
	}
}