#include "ActionTextParser.h"
#include <QChar>
#include <iostream>

QRegularExpression ActionTextParser::start_("^(\\d+\\.\\d+):\\s+starting action:\\s+id\\s+(\\d+),\\s+action\\s+(.*)");
QRegularExpression ActionTextParser::cancel_("^(\\d+\\.\\d+):\\s+canceling action:\\s+id\\s+(\\d+),\\s+action\\s+(.*)");
QRegularExpression ActionTextParser::end_("^(\\d+\\.\\d+):\\s+completing action:\\s+id\\s+(\\d+),\\s+action\\s+(.*)");
QRegularExpression ActionTextParser::parallel_("^Parallel\\s+\\[\\[(.*)\\]\\]");
QRegularExpression ActionTextParser::sequence_("^Sequence\\s+\\[\\[(.*)\\]\\]");
QRegularExpression ActionTextParser::dispatch_("^DispatchAction\\s*\\(\\w+,\\s*(\\d+)");
//QRegularExpression ActionTextParser::dispatch_("^DispatchAction\\s*\\(\\w+,\\s*(\\d+){{.*}},\\w+)");
QRegularExpression ActionTextParser::terminate_("");

//DispatchAction(SUBSYSTEM, #{{ACTION}}, BLOCK)

ActionTextParser::ActionTextParser(ActionDB &db) : db_(db)
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

			assert(db_.findActionByID(idval) == nullptr);
			auto act = std::make_shared<Action>(idval, timeval, text);
			db_.add(act);
		}

		match = cancel_.match(line);
		if (match.hasMatch())
		{
			timeval = match.captured(1).toDouble();
			idval = match.captured(2).toInt();
			text = match.captured(3);

			auto act = db_.findActionByID(idval);
			if (act != nullptr)
				act->setCancelTime(timeval);
		}

		match = end_.match(line);
		if (match.hasMatch())
		{
			timeval = match.captured(1).toDouble();
			idval = match.captured(2).toInt();
			text = match.captured(3);

			auto act = db_.findActionByID(idval);
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

bool ActionTextParser::parseActionText(const QString& text, int& index, QString& action)
{
	int nested = 0;

	while (index < text.length())
	{
		if (index < text.length() - 1 && text.mid(index, 2) == "{{")
		{
			nested++;
			index += 2;
			action += "{{";
		}
		else if (index < text.length() - 1 && text.mid(index, 2) == "}}")
		{
			index += 2;
			action += "}}";
			nested--;
			if (nested == 0)
				break;
		}
		else
			action += text[index++];
	}

	if (action.startsWith("{{"))
		action = action.mid(2);

	if (action.endsWith("}}"))
		action = action.mid(0, action.length() - 2);

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

		QString child;
		if (!parseActionText(text, index, child))
			break;

		auto childact = db_.findActionByID(id);
		if (childact != nullptr)
			act->addChild(childact);

		if (index == text.length())
			break;

		if (text[index] == ',')
			index++;
	}
}

void ActionTextParser::findChildren()
{
	QRegularExpressionMatch match;

	for (auto act : db_.getActions())
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

		match = dispatch_.match(act->getText());
		if (match.hasMatch())
		{
			uint32_t id = match.captured(1).toUInt();
			auto child = db_.findActionByID(id);
			assert(child != nullptr);
			act->addChild(child);
		}
	}
}