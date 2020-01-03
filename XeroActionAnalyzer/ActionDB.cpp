#include "ActionDB.h"

ActionDB::ActionDB()
{

}

ActionDB::~ActionDB()
{

}

double ActionDB::getLastEventTime()
{
	double maxt = 0.0;

	for (auto act : actions_)
	{
		maxt = std::max(maxt, act->getStartTime());

		if (act->isDone())
			maxt = std::max(maxt, act->getEndTime());

		if (act->isCanceled())
			maxt = std::max(maxt, act->getCancelTime());
	}

	return maxt;
}