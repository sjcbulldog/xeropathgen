#include "XeroActionAnalyzer.h"
#include "XeroActionAnalyzer.h"
#include "ActionTextParser.h"
#include "ActionFileView.h"
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>

XeroActionAnalyzer::XeroActionAnalyzer(QWidget *parent) : QMainWindow(parent)
{
	createWindows();
	createMenus();
}

void XeroActionAnalyzer::createWindows()
{
	tabs_ = new QTabWidget(this);
	tabs_->setTabsClosable(true);
	tabs_->setTabShape(QTabWidget::Rounded);
	setCentralWidget(tabs_);

	if (settings_.contains("GEOMETRY"))
		restoreGeometry(settings_.value("GEOMETRY").toByteArray());

	if (settings_.contains("STATE"))
		restoreState(settings_.value("STATE").toByteArray());
}

void XeroActionAnalyzer::createMenus()
{
	QAction* action;

	file_ = new QMenu(tr("&File"));
	menuBar()->addMenu(file_);
	action = file_->addAction(tr("Open ..."));
	(void)connect(action, &QAction::triggered, this, &XeroActionAnalyzer::fileOpen);
	file_->addSeparator();
	action = file_->addAction(tr("Close"));
	(void)connect(action, &QAction::triggered, this, &XeroActionAnalyzer::fileClose);
	file_->addSeparator();
}

void XeroActionAnalyzer::fileOpen()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Load Path File"), "", tr("Path File (*.log);;All Files (*.*)"));
	if (filename.length() == 0)
		return;

	loadFile(filename);
}

void XeroActionAnalyzer::fileClose()
{
	int index = tabs_->currentIndex();
	tabs_->removeTab(index);
}

void XeroActionAnalyzer::showEvent(QShowEvent* ev)
{
	QStringList args = QCoreApplication::arguments();
	auto it = args.begin();
	it++;

	while (it != args.end())
	{
		loadFile(*it++);
	}
}

void XeroActionAnalyzer::closeEvent(QCloseEvent* ev)
{
	settings_.setValue("GEOMETRY", saveGeometry());
	settings_.setValue("STATE", saveState());
}

void XeroActionAnalyzer::loadFile(const QString& filename)
{
	QStringList list;
	QFile textFile(filename);

	ActionFileView* view = new ActionFileView(tabs_);

	if (!textFile.open(QIODevice::ReadOnly))
	{
		QString txt = "cannot open file '" + filename + "' readonly";
		QMessageBox box(QMessageBox::Icon::Critical,
			"Error", txt, QMessageBox::StandardButton::Ok);
		box.exec();
		return;
	}

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
		ActionTextParser parser(view->getDB());
		parser.parse(list);
		view->updateContents();

		QFileInfo info(filename);
		tabs_->addTab(view, info.fileName());
	}
}
