//
// Copyright 2019 Jack W. Griffin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
//
#pragma once

#include "Callout.h"
#include <RobotPath.h>
#include <QChartView>
#include <QValueAxis>
#include <QLineSeries>
#include <QGraphicsLineItem>
#include <map>

class PathPlotView : public QtCharts::QChartView
{
	friend class PlotVariableSelector;

	Q_OBJECT

public:
	enum VarType
	{
		VTTime,
		VTPosition,
		VTVelocity,
		VTAcceleration,
		VTJerk
	};

public:
	PathPlotView(QWidget *parent = nullptr);
	~PathPlotView();

	void setCursorTime(double stime);

	void setPath(std::shared_ptr<xero::paths::RobotPath> path);
	void setUnits(QString u) { 
		units_ = u; 
		updateAxisUnits();
		repaint();
	}
	void update();

	void addPlotVariable(const char* trajname, VarType type) {
		if (type != VarType::VTTime)
			vars_.push_back(Variable(trajname, type));
	}

	void clearPlotVariables() {
		vars_.clear();
	}

	void addPlotVariables(QList<QString> list);
	QList<QString> getPlotVariables();

protected:
	void paintEvent(QPaintEvent* event) override; 
	void keyPressEvent(QKeyEvent* event) override;
	bool viewportEvent(QEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

private:
	void plotAreaChanged(const QRectF& plotArea);
	void seriesHover(QtCharts::QLineSeries*series, const QPointF& point, bool state);
	void seriesClick(QtCharts::QLineSeries* series, const QPointF& point);
	void updateAxisUnits();

private:
	struct Variable
	{
		std::string trajectory_;
		VarType type_;

		Variable(const char* trajname, VarType type)
		{
			trajectory_ = trajname;
			type_ = type;
		}
	};

	static std::string typeToName(VarType t)
	{
		std::string ret;

		switch (t)
		{
		case VarType::VTAcceleration:
			ret = "accel";
			break;

		case VarType::VTVelocity:
			ret = "velocity";
			break;

		case VarType::VTPosition:
			ret = "position";
			break;

		case VarType::VTJerk:
			ret = "jerk";
			break;

		case VarType::VTTime:
			ret = "time" ;
			break ;
		}

		return ret;
	}

	static VarType nameToType(const std::string& name);

private:
	void updateEmpty();
	void updateWithPath();
	void createAxis(QtCharts::QChart* ch);
	void createLegend(QtCharts::QChart* ch);
	size_t countType(VarType type);
	double getValue(std::shared_ptr<xero::paths::PathTrajectory> traj, size_t index, VarType t);
	QtCharts::QValueAxis* getAxis(VarType t);
	void setMinMax(VarType t, double minv, double mavx);
	double getMin(VarType t);
	double getMax(VarType t);

private:
	double cursor_time_;
	std::shared_ptr<xero::paths::RobotPath> path_;
	QtCharts::QValueAxis* pos_axis_;
	QtCharts::QValueAxis* vel_axis_;
	QtCharts::QValueAxis* acc_axis_;
	QtCharts::QValueAxis* jerk_axis_;
	QtCharts::QValueAxis* time_axis_;
	QString units_;
	std::list<Variable> vars_;
	std::map<VarType, std::pair<double, double>> min_max_;
	double total_scroll_x_;
	double total_scroll_y_;
	QGraphicsLineItem* lineitem_;
	Callout* callout_;
	QLineSeries* first_;
	std::list<Callout*> callouts_;
	Callout* errors_;
	bool has_title_;
};
