#pragma once

#include <QString>

class UndoItem
{
public:
	UndoItem();
	virtual ~UndoItem();
	virtual QString toString() = 0;
	virtual void undo() = 0;

private:
};

