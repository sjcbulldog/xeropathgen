#pragma once

#include "ActionView.h"
#include <QMainWindow>
#include <QSplitter>
#include <QTextEdit>
#include <QSettings>

class XeroActionAnalyzer : public QMainWindow
{
	Q_OBJECT

public:
	XeroActionAnalyzer(QWidget *parent = Q_NULLPTR);

protected:
	void showEvent(QShowEvent* ev);

private:
	void createWindows();
	void createMenus();
	void loadFile(const QString& filename);

private:
	QSplitter* top_bottom_spliter_;
	ActionView* action_view_;
	QTextEdit* action_text_;

	QSettings settings_;
};
