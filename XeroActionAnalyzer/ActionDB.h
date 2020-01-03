#pragma once

#include "Action.h"
#include <list>
#include <memory>

class ActionDB
{
public:
	ActionDB();
	virtual ~ActionDB();

	std::shared_ptr<Action> findActionByID(uint32_t id) {
		auto it = std::find_if(actions_.begin(), actions_.end(), [id](auto act) { return act->getID() == id; });
		if (it == actions_.end())
			return nullptr;

		return *it;
	}

	void clear() {
		actions_.clear();
	}

	std::list<std::shared_ptr<Action>>& getActions() {
		return actions_;
	}

	void add(std::shared_ptr<Action> act) {
		actions_.push_back(act);
	}

	double getLastEventTime();

private:
	std::list<std::shared_ptr<Action>> actions_;
};

