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
#include "PathPlotView.h"
#include "TrajectoryNames.h"
#include <QChartView>
#include <QChart>
#include <QLineSeries>
#include <QValueAxis>

using namespace xero::paths;
using namespace QtCharts;

PathPlotView::PathPlotView(QWidget *parent) : QChartView(parent)
{
	setRenderHint(QPainter::Antialiasing);
	chart()->setAnimationOptions(QChart::SeriesAnimations);
	time_axis_ = nullptr;
	pos_axis_ = nullptr;
	vel_axis_ = nullptr;
	acc_axis_ = nullptr;
	jerk_axis_ = nullptr;
	setRubberBand(QChartView::RectangleRubberBand);
	total_scroll_x_ = 0;
	total_scroll_y_ = 0;
	lineitem_ = nullptr;
	callout_ = nullptr;
	errors_ = nullptr;

	(void)connect(chart(), &QChart::plotAreaChanged, this, &PathPlotView::plotAreaChanged);
}

PathPlotView::~PathPlotView()
{
}

bool PathPlotView::viewportEvent(QEvent* event)
{
	return QChartView::viewportEvent(event);
}

void PathPlotView::mousePressEvent(QMouseEvent* event)
{
	QChartView::mousePressEvent(event);
}

void PathPlotView::mouseMoveEvent(QMouseEvent* event)
{
	QChartView::mouseMoveEvent(event);
}

void PathPlotView::mouseReleaseEvent(QMouseEvent* event)
{
	QChartView::mouseReleaseEvent(event);
}

void PathPlotView::paintEvent(QPaintEvent* event)
{
	QChartView::paintEvent(event);
}

void PathPlotView::keyPressEvent(QKeyEvent* event)
{
	switch (event->key()) {
	case Qt::Key_Plus:
		chart()->zoomIn();
		break;
	case Qt::Key_Minus:
		chart()->zoomOut();
		break;
	case Qt::Key_Left:
		chart()->scroll(-10, 0);
		total_scroll_x_ -= 10;
		break;
	case Qt::Key_Right:
		chart()->scroll(10, 0);
		total_scroll_x_ += 10;
		break;
	case Qt::Key_Up:
		chart()->scroll(0, 10);
		total_scroll_y_ += 10;
		break;
	case Qt::Key_Down:
		chart()->scroll(0, -10);
		total_scroll_y_ -= 10;
		break;
	case Qt::Key_Home:
		chart()->scroll(-total_scroll_x_, -total_scroll_y_);
		chart()->zoomReset();
		total_scroll_x_ = 0;
		total_scroll_y_ = 0;
		for (auto callout : callouts_)
			delete callout;
		callouts_.clear();
		break;
	default:
		QGraphicsView::keyPressEvent(event);
		break;
	}
}

void PathPlotView::plotAreaChanged(const QRectF& rect)
{
	(void)rect;
	setCursorTime(cursor_time_);
}

void PathPlotView::setCursorTime(double stime)
{
	if (time_axis_ == nullptr)
		return;

	cursor_time_ = stime;
	QPointF pt = chart()->mapToPosition(QPointF(stime, 0.0));
	QRectF rect = chart()->plotArea();

	if (lineitem_ != nullptr)
	{
		chart()->scene()->removeItem(lineitem_);
		delete lineitem_;
	}

	QPen pen(QColor(0x00, 0x00, 0x00, 0xff));
	pen.setWidth(4);
	lineitem_ = chart()->scene()->addLine(pt.rx(), rect.top(), pt.rx(), rect.bottom(), pen);
}

QList<QString> PathPlotView::getPlotVariables()
{
	QList<QString> list;

	for (const Variable& var : vars_)
	{
		QString item = QString(var.trajectory_.c_str()) + QString(",") + typeToName(var.type_).c_str();
		list.push_back(item);
	}

	return list;
}

PathPlotView::VarType PathPlotView::nameToType(const std::string& name)
{
	if (name == "accel")
		return VarType::VTAcceleration;

	if (name == "position")
		return VarType::VTPosition;

	if (name == "velocity")
		return VarType::VTVelocity;

	if (name == "jerk")
		return VarType::VTJerk;

	return VarType::VTTime;
}

void PathPlotView::addPlotVariables(QList<QString> list)
{
	for (QString one : list)
	{
		QStringList onelist = one.split(',');
		if (onelist.size() == 2)
		{
			VarType type = nameToType(onelist.back().toStdString());
			if (type != VarType::VTTime)
				addPlotVariable(onelist.front().toStdString().c_str(), type);
		}
	}
}

void PathPlotView::setPath(std::shared_ptr<RobotPath> path)
{
	path_ = path;
	update();
}

void PathPlotView::update()
{
	if (path_ == nullptr || path_->getTrajectory(TrajectoryName::Main) == nullptr || !path_->hasData())
		updateEmpty();
	else
		updateWithPath();

	if (path_ != nullptr && path_->hasErrors())
	{
		if (errors_ == nullptr)
			errors_ = new Callout(chart());

		QString txt;
		for (const std::string & str : path_->errors())
		{
			if (txt.length() > 0)
				txt += "\r\n";

			txt += str.c_str();
		}

		errors_->setText(txt);
		errors_->setAnchor(QPointF(0, 0));
		errors_->setZValue(11);
		errors_->updateGeometry();
		errors_->show();
	}
	else
	{
		if (errors_ != nullptr)
			errors_->setVisible(false) ;
	}
}

size_t PathPlotView::countType(VarType t)
{
	size_t count = 0;

	for (const Variable& var : vars_)
	{
		if (var.type_ == t)
			count++;
	}

	return count;
}

void PathPlotView::updateEmpty()
{
	QChart* ch = chart();

	QFont font = ch->titleFont();
	font.setPointSize(20);
	font.setBold(true);
	ch->setTitleFont(font);
	if (path_ == nullptr)
		ch->setTitle("No path selected");
	else if (path_->hasErrors())
		ch->setTitle("Errors in the path - no trajectory data available");
	else
		ch->setTitle("Unknown issues - no trajectory data available");

	ch->removeAllSeries();
	QLegend* legend = ch->legend();
	legend->setVisible(false);

	if (time_axis_ != nullptr)
		time_axis_->setVisible(false);

	if (pos_axis_ != nullptr)
		pos_axis_->setVisible(false);

	if (vel_axis_ != nullptr)
		vel_axis_->setVisible(false);

	if (acc_axis_ != nullptr)
		acc_axis_->setVisible(false);

	if (jerk_axis_ != nullptr)
		jerk_axis_->setVisible(false);
}

double PathPlotView::getValue(std::shared_ptr<PathTrajectory> traj, size_t index, VarType t)
{
	double ret = 0.0;

	switch (t)
	{
	case VarType::VTAcceleration:
		ret = (*traj)[index].acceleration();
		break;

	case VarType::VTVelocity:
		ret = (*traj)[index].velocity();
		break;

	case VarType::VTPosition:
		ret = (*traj)[index].position();
		break;

	case VarType::VTJerk:
		ret = (*traj)[index].jerk();
		break;

	case VarType::VTTime:
	  	ret = (*traj)[index].time() ;
		break ;
	}

	return ret;
}

QValueAxis *PathPlotView::getAxis(VarType t)
{
	QValueAxis* axis = nullptr;

	switch (t)
	{
	case VarType::VTAcceleration:
		axis = acc_axis_;
		break;

	case VarType::VTVelocity:
		axis = vel_axis_;
		break;

	case VarType::VTPosition:
		axis = pos_axis_;
		break;

	case VarType::VTJerk:
		axis = jerk_axis_;
		break;

	case VarType::VTTime:
		axis = time_axis_;
		break;
	}

	return axis;
}

void PathPlotView::updateAxisUnits()
{
	if (pos_axis_ != nullptr)
		pos_axis_->setTitleText("Position (" + units_ + ")");

	if (vel_axis_ != nullptr)
		vel_axis_->setTitleText("Velocity (" + units_ + " / sec)");

	if (acc_axis_ != nullptr)
		acc_axis_->setTitleText("Accel (" + units_ + " / sec^2)");

	if (jerk_axis_ != nullptr)
		jerk_axis_->setTitleText("Jerk (" + units_ + " / sec^3)");
}

void PathPlotView::createAxis(QChart *ch)
{
	if (time_axis_ == nullptr)
	{
		time_axis_ = new QValueAxis();
		time_axis_->setLabelsVisible(true);
		time_axis_->setTickCount(10);
		time_axis_->setTitleText("time (s)");
		time_axis_->setTitleVisible(true);
		ch->addAxis(time_axis_, Qt::AlignBottom);
	}
	else
	{
		time_axis_->setVisible(true);
	}

	if (pos_axis_ == nullptr)
	{
		pos_axis_ = new QValueAxis();
		pos_axis_->setLabelsVisible(true);
		pos_axis_->setTickCount(8);
		pos_axis_->setTitleText("Position (" + units_ + ")");
		pos_axis_->setTitleVisible(true);
		ch->addAxis(pos_axis_, Qt::AlignLeft);
	}

	if (countType(VarType::VTPosition) > 0)
		pos_axis_->setVisible(true);
	else
		pos_axis_->setVisible(false);

	if (vel_axis_ == nullptr)
	{
		vel_axis_ = new QValueAxis();
		vel_axis_->setLabelsVisible(true);
		vel_axis_->setTickCount(8);
		vel_axis_->setTitleText("Velocity (" + units_ + " / sec)");
		vel_axis_->setTitleVisible(true);
		ch->addAxis(vel_axis_, Qt::AlignRight);
	}

	if (countType(VarType::VTVelocity) > 0)
		vel_axis_->setVisible(true);
	else
		vel_axis_->setVisible(false);

	if (acc_axis_ == nullptr)
	{
		acc_axis_ = new QValueAxis();
		acc_axis_->setLabelsVisible(true);
		acc_axis_->setTickCount(8);
		acc_axis_->setTitleText("Accel (" + units_ + " / sec^2)");
		acc_axis_->setTitleVisible(true);
		ch->addAxis(acc_axis_, Qt::AlignRight);
	}

	if (countType(VarType::VTAcceleration) > 0)
		acc_axis_->setVisible(true);
	else
		acc_axis_->setVisible(false);

	if (jerk_axis_ == nullptr)
	{
		jerk_axis_ = new QValueAxis();
		jerk_axis_->setLabelsVisible(true);
		jerk_axis_->setTickCount(8);
		jerk_axis_->setTitleText("Jerk (" + units_ + " / sec^3)");
		jerk_axis_->setTitleVisible(true);
		ch->addAxis(jerk_axis_, Qt::AlignLeft);
	}

	if (countType(VarType::VTJerk) > 0)
		jerk_axis_->setVisible(true);
	else
		jerk_axis_->setVisible(false);
}

void PathPlotView::createLegend(QChart* ch)
{
	QFont font;

	QLegend* legend = ch->legend();
	legend->setVisible(true);
	legend->setAlignment(Qt::AlignBottom);
	legend->setMarkerShape(QLegend::MarkerShape::MarkerShapeCircle);
	font = legend->font();
	font.setPointSize(8);
	font.setBold(true);
	legend->setFont(font);
}

void PathPlotView::updateWithPath()
{
	min_max_.clear();

	QChart* ch = chart();
	QPen pen;

	QFont font = ch->titleFont();
	font.setPointSize(16);
	font.setBold(true);
	ch->setTitleFont(font);

	ch->removeAllSeries();
	if (path_->hasErrors())
		ch->setTitle("Trajectory Profile (errors)");
	else
		ch->setTitle("Trajectory Profile");
	ch->setDropShadowEnabled(true);
	first_ = nullptr;

	createAxis(ch);
	createLegend(ch);

	double timemin = std::numeric_limits<double>::max(), timemax = std::numeric_limits<double>::min();

	QLineSeries* ser;
	for (const Variable& var : vars_)
	{
		std::string vname = typeToName(var.type_);
		ser = new QLineSeries();
		if (first_ == nullptr)
			first_ = ser;

		(void)connect(ser, &QLineSeries::hovered, this, [ser, this](const QPointF& pt, bool state) { this->seriesHover(ser, pt, state); });
		(void)connect(ser, &QLineSeries::clicked, this, [ser, this](const QPointF& pt) { this->seriesClick(ser, pt); });

		std::string name = var.trajectory_ + "-" + vname;
		ser->setName(name.c_str());

		double minv = std::numeric_limits<double>::max(), maxv = std::numeric_limits<double>::min();
		auto traj = path_->getTrajectory(var.trajectory_);
		if (traj != nullptr)
		{
			for (size_t i = 0; i < traj->size(); i++) {
				const Pose2dWithTrajectory& pt = (*traj)[i];

				if (pt.time() < timemin)
					timemin = pt.time();
				if (pt.time() > timemax)
					timemax = pt.time();

				double value = getValue(traj, i, var.type_);
				if (value > maxv)
					maxv = value;

				if (value < minv)
					minv = value;

				ser->append(pt.time(), value);
			}

			setMinMax(VarType::VTTime, timemin, timemax);
			setMinMax(var.type_, minv, maxv);

			ch->addSeries(ser);
			ser->attachAxis(time_axis_);
			ser->attachAxis(getAxis(var.type_));
		}
	}

	VarType t;

	t = VarType::VTTime;
	getAxis(t)->setRange(getMin(t), getMax(t));
	
	t = VarType::VTPosition;
	getAxis(t)->setRange(getMin(t), getMax(t));
	getAxis(t)->applyNiceNumbers();

	t = VarType::VTVelocity;
	getAxis(t)->setRange(getMin(t), getMax(t));
	getAxis(t)->applyNiceNumbers();

	t = VarType::VTAcceleration;
	getAxis(t)->setRange(getMin(t), getMax(t));
	getAxis(t)->applyNiceNumbers();

	t = VarType::VTJerk;
	getAxis(t)->setRange(getMin(t), getMax(t));
	getAxis(t)->applyNiceNumbers();

	setCursorTime(0.0);
}

void PathPlotView::setMinMax(VarType t, double minv, double maxv)
{
	auto it = min_max_.find(t);
	if (it == min_max_.end())
	{
		min_max_[t] = std::make_pair(minv, maxv);
	}
	else
	{
		auto pair = min_max_[t];
		if (minv < pair.first)
			pair.first = minv;
		if (maxv > pair.second)
			pair.second = maxv;

		min_max_[t] = pair;
	}
}

double PathPlotView::getMin(VarType t)
{
	auto it = min_max_.find(t);
	if (it == min_max_.end())
		return 0.0;

	return min_max_[t].first;
}

double PathPlotView::getMax(VarType t)
{
	auto it = min_max_.find(t);
	if (it == min_max_.end())
		return 0.0;

	return min_max_[t].second;
}

void PathPlotView::seriesHover(QLineSeries *ser, const QPointF &pt, bool state)
{
	QPointF npt = chart()->mapToPosition(pt, ser);
	npt = chart()->mapToValue(npt, first_);

	(void)ser;
	if (callout_ == nullptr)
		callout_ = new Callout(chart());

	if (state)
	{
		QString text;
		
		text += "Time: " + QString::number(pt.x(), 'f', 2) + "\n";
		text += ser->name() + ": " + QString::number(pt.y(), 'f', 1);

		callout_->setText(text);
		callout_->setAnchor(npt);
		callout_->setZValue(11);
		callout_->updateGeometry();
		callout_->show();
	}
	else
		callout_->hide();
}


void PathPlotView::seriesClick(QLineSeries* ser, const QPointF& pt)
{
	(void)ser;
	(void)pt;

	callouts_.push_back(callout_);
	callout_ = nullptr;
}
