#include "ActionTextParser.h"
#include <QChar>
#include <iostream>

QRegularExpression ActionTextParser::start_("^(\\d+\\.\\d+):\\s+starting action:\\s+id\\s+=\\s+(\\d+),\\s+action\\s+(.*)");
QRegularExpression ActionTextParser::cancel_("^(\\d+\\.\\d+):\\s+canceling action:\\s+id\\s+=\\s+(\\d+),\\s+action\\s+(.*)");
QRegularExpression ActionTextParser::end_("^(\\d+\\.\\d+):\\s+completing action:\\s+id\\s+=\\s+(\\d+),\\s+action\\s+(.*)");
QRegularExpression ActionTextParser::parallel_("^Parallel\\s+\\[(.*)\\]");
QRegularExpression ActionTextParser::sequence_("^Sequence\\s+\\[(.*)\\]");

ActionTextParser::ActionTextParser()
{
}

ActionTextParser::~ActionTextParser()
{

}

bool ActionTextParser::parse(const QStringList& list)
{
	double timeval;
	int idval;
	QString text;

	for (const QString& line : list)
	{
		QRegularExpressionMatch match;

		match = start_.match(line);
		if (match.hasMatch())
		{
			timeval = match.captured(1).toDouble();
			idval = match.captured(2).toInt();
			text = match.captured(3);

			assert(findActionByID(idval) == nullptr);
			auto act = std::make_shared<Action>(timeval, idval, text);
			actions_.push_back(act);
		}

		match = cancel_.match(line);
		if (match.hasMatch())
		{
			timeval = match.captured(1).toDouble();
			idval = match.captured(2).toInt();
			text = match.captured(3);

			auto act = findActionByID(idval);
			if (act != nullptr)
				act->setCancelTime(timeval);
		}

		match = end_.match(line);
		if (match.hasMatch())
		{
			timeval = match.captured(1).toDouble();
			idval = match.captured(2).toInt();
			text = match.captured(3);

			auto act = findActionByID(idval);
			if (act != nullptr)
				act->setEndTime(timeval);
		}
	}

	findChildren();
	return true;
}

bool ActionTextParser::parseActionID(const QString& text, int &index, uint32_t& id)
{
	QString idstr;

	while (index < text.length() && text[index].isDigit())
		idstr += text[index++];

	if (idstr.length() == 0)
		return false;

	id = idstr.toUInt();
	return true;
}

void ActionTextParser::parseChildren(std::shared_ptr<Action> act, const QString& text)
{
	int index = 0;
	uint32_t id;

	while (index < text.length())
	{
		//
		// Parse an integer action id
		//
		if (!parseActionID(text, index, id))
			break;

		if (index >= text.length() || text[index] != ':')
			break;

		index++;

		QString child;
		if (!parserActionText(text, index, child))
			break;

	}
}

void ActionTextParser::findChildren()
{
	QRegularExpressionMatch match;

	for (auto act : actions_)
	{
		match = parallel_.match(act->getText());
		if (match.hasMatch())
		{
			parseChildren(act, match.captured(1));
		}

		match = sequence_.match(act->getText());
		if (match.hasMatch())
		{
			parseChildren(act, match.captured(1));
		}
	}
}