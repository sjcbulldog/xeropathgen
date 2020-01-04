#pragma once

#include "ActionView.h"
#include "ActionTimelineView.h"
#include <QMainWindow>
#include <QSplitter>
#include <QTextEdit>
#include <QSettings>
#include <QScrollArea>

class XeroActionAnalyzer : public QMainWindow
{
	Q_OBJECT

public:
	XeroActionAnalyzer(QWidget *parent = Q_NULLPTR);

protected:
	void showEvent(QShowEvent* ev);
	void closeEvent(QCloseEvent* ev);

private:
	void createWindows();
	void createMenus();
	void loadFile(const QString& filename);

	void fileOpen();
	void fileClose();

private:
	QSettings settings_;
	QTabWidget* tabs_;

	QMenu* file_;
};
