#include "ActionTextParser.h"
#include <iostream>

ActionTextParser::ActionTextParser()
{
}

ActionTextParser::~ActionTextParser()
{

}

bool ActionTextParser::parse(const QStringList& list)
{
	for (const QString& line : list)
	{
		std::string linestr = line.toStdString();
		std::cout << linestr << std::endl;
	}
	return true;
}