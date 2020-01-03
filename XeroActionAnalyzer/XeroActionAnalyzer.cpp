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

	(void)connect(action_view_, &QTreeWidget::currentItemChanged, this, &XeroActionAnalyzer::itemSelected);

	scroll_ = new QScrollArea(top_bottom_spliter_);
	scroll_->setWidgetResizable(true);
	top_bottom_spliter_->addWidget(scroll_);

	timeline_ = new ActionTimelineView(db_, scroll_);
	scroll_->setWidget(timeline_);

	if (settings_.contains("GEOMETRY"))
		restoreGeometry(settings_.value("GEOMETRY").toByteArray());

	if (settings_.contains("STATE"))
		restoreState(settings_.value("STATE").toByteArray());

	if (settings_.contains("SPLITTER"))
	{
		QList<QVariant> stored = settings_.value("SPLITTER").toList();
		QList<int> sizes;
		for (const QVariant& v : stored)
			sizes.push_back(v.toInt());
		top_bottom_spliter_->setSizes(sizes);
	}
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

void XeroActionAnalyzer::closeEvent(QCloseEvent* ev)
{
	settings_.setValue("GEOMETRY", saveGeometry());
	settings_.setValue("STATE", saveState());

	QList<QVariant> stored;
	for (int size : top_bottom_spliter_->sizes())
		stored.push_back(QVariant(size));
	settings_.setValue("SPLITTER", stored);
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

void XeroActionAnalyzer::itemSelected(QTreeWidgetItem* current, QTreeWidgetItem* prev)
{
	if (current != nullptr)
	{
		QString idtxt = current->text(3);
		int id = idtxt.toInt();
		timeline_->updateContents(id);
	}
	else
	{
		timeline_->updateContents(-1);
	}
}