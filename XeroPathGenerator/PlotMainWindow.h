#pragma once

#include "PathPlotView.h"
#include <RobotPath.h>
#include <QWidget>
#include <QVBoxLayout>
#include <QScrollBar>

class PlotMainWindow : public QWidget
{
	Q_OBJECT

public:
	PlotMainWindow(QWidget *parent = Q_NULLPTR);
	~PlotMainWindow();

	void setPath(std::shared_ptr<xero::paths::RobotPath> path);

	PathPlotView* plotview() {
		return view_;
	}

	QScrollBar* bar() {
		return bar_;
	}

	double getTime() const {
		return bar_->value() / 1000.0;
	}

	void update();

private:
	void updateScrollBar();

private:
	QVBoxLayout* layout_;
	QScrollBar* bar_;
	PathPlotView* view_;
	std::shared_ptr<xero::paths::RobotPath> path_;
};
