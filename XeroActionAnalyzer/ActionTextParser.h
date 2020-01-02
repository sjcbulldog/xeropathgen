#pragma once

#include "Action.h"
#include <QStringList>
#include <QRegularExpression>
#include <list>
#include <memory>
#include <algorithm>

class ActionTextParser
{
public:
	ActionTextParser();
	virtual ~ActionTextParser();

	bool parse(const QStringList& list);

	std::list<std::shared_ptr<Action>> getActionList() {
		return actions_;
	}

protected:
	std::shared_ptr<Action> findActionByID(uint32_t id) {
		auto it = std::find_if(actions_.begin(), actions_.end(), [id](auto act) { return act->getID() == id; });
		if (it == actions_.end())
			return nullptr;

		return *it;
	}

	void findChildren();
	void parseChildren(std::shared_ptr<Action> act, const QString & list);
	bool parseActionID(const QString& text, int &index, uint32_t& id);
	bool parseActionText(const QString& text, int& index, QString& acttext);

private:

	std::list<std::shared_ptr<Action>> actions_;

	static QRegularExpression start_;
	static QRegularExpression end_;
	static QRegularExpression cancel_;
	static QRegularExpression parallel_;
	static QRegularExpression sequence_;
};

