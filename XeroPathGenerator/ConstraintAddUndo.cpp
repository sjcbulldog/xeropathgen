#include "ConstraintAddUndo.h"

ConstraintAddUndo::ConstraintAddUndo(ConstraintEditor& model, int row) : model_(model)
{
	row_ = row;
}

ConstraintAddUndo::~ConstraintAddUndo()
{

}

QString ConstraintAddUndo::toString()
{
	QString ret("ConstraintAddUndo");
	ret += " " + QString::number(row_);
	return ret;
}

void ConstraintAddUndo::undo()
{
	model_.deleteConstraint(row_);
}