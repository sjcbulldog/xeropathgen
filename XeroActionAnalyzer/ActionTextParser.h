#pragma once

#include "Action.h"
#include "ActionDB.h"
#include <QStringList>
#include <QRegularExpression>
#include <list>
#include <memory>
#include <algorithm>

class ActionTextParser
{
public:
	ActionTextParser(ActionDB &db);
	virtual ~ActionTextParser();

	bool parse(const QStringList& list);

protected:

	void findChildren();
	void parseChildren(std::shared_ptr<Action> act, const QString & list);
	bool parseActionID(const QString& text, int &index, uint32_t& id);
	bool parseActionText(const QString& text, int& index, QString& acttext);

private:
	ActionDB& db_;

	static QRegularExpression start_;
	static QRegularExpression end_;
	static QRegularExpression cancel_;
	static QRegularExpression parallel_;
	static QRegularExpression sequence_;
	static QRegularExpression dispatch_;
	static QRegularExpression terminate_;
};

