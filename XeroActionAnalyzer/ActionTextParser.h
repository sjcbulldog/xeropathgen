#pragma once

#include <QStringList>

class ActionTextParser
{
public:
	ActionTextParser();
	virtual ~ActionTextParser();

	bool parse(const QStringList& list);

private:
};

