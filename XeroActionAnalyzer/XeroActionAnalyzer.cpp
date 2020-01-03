#include "XeroActionAnalyzer.h"
#include "ActionTextParser.h"
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>

XeroActionAnalyzer::XeroActionAnalyzer(QWidget *parent) : QMainWindow(parent)
{
	createWindows();
	createMenus();
}

void XeroActionAnalyzer::createWindows()
{
	top_bottom_spliter_ = new QSplitter();
	top_bottom_spliter_->setOrientation(Qt::Orientation::Vertical);
	setCentralWidget(top_bottom_spliter_);

	action_view_ = new ActionView(db_, top_bottom_spliter_);
	top_bottom_spliter_->addWidget(action_view_);

	scroll_ = new QScrollArea(top_bottom_spliter_);
	scroll_->setWidgetResizable(true);
	top_bottom_spliter_->addWidget(scroll_);

	timeline_ = new ActionTimelineView(db_, scroll_);
	scroll_->setWidget(timeline_);

	if (settings_.contains("GEOMETRY"))
		restoreGeometry(settings_.value("GEOMETRY").toByteArray());

	if (settings_.contains("STATE"))
		restoreState(settings_.value("STATE").toByteArray());
}

void XeroActionAnalyzer::createMenus()
{
}

void XeroActionAnalyzer::showEvent(QShowEvent* ev)
{
	QStringList args = QCoreApplication::arguments();
	if (args.length() == 2)
		loadFile(args.back());
}

void XeroActionAnalyzer::loadFile(const QString& filename)
{
	QStringList list;
	QFile textFile(filename);

	db_.clear();

	if (!textFile.open(QIODevice::ReadOnly))
		return;

	QTextStream stream(&textFile);

	while (true)
	{
		QString line = stream.readLine();
		if (line.isNull())
			break;

		list.append(line);
	}

	if (list.size() > 0)
	{
		ActionTextParser parser(db_);
		parser.parse(list);

		action_view_->updateContents();
		timeline_->updateContents(11);
	}
}