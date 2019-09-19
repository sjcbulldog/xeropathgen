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

#include "DriveBaseModel.h"
#include "GameField.h"
#include <RobotParams.h>
#include <RobotPath.h>
#include <Pose2d.h>
#include <QWidget>
#include <QPixmap>
#include <QTransform>
#include <memory>

class PathFieldView : public QWidget
{
	friend class ShowRobotWindow;

	Q_OBJECT

public:
	PathFieldView(QWidget *parent = Q_NULLPTR);
	virtual ~PathFieldView();
	
	void setCursorTime(double time) {
		cursor_time_ = time;
		repaint();
	}

	void enableCursor(bool b) {
		cursor_ = b;
	}

	void enableGrid(bool b) {
		draw_grid_ = b;
	}

	bool isGridEnabled() const {
		return draw_grid_;
	}

	void completeGrid(bool b) {
		grid_complete_ = b;
	}

	bool isCompleteGrid() const {
		return grid_complete_;
	}

	void showEquation(bool b) {
		show_equations_ = b;
	}

	bool isShowEquation() const {
		return  show_equations_;
	}

	void equationsStacked(bool b) {
		equations_stacked_ = b;
	}

	bool areEquationsStacked() const {
		return equations_stacked_;
	}

	void demoStep();
	double getDemoTime();
	bool isDemoPathDone();

	void setField(std::shared_ptr<GameField> field);
	void setPath(std::shared_ptr<xero::paths::RobotPath> path);
	void setRobot(std::shared_ptr<xero::paths::RobotParams> params);
	void setUnits(const std::string &units);

	void resetDemo();

	std::shared_ptr<xero::paths::RobotPath> getPath() {
		return path_;
	}

	QPointF worldToWindow(const QPointF& pt);
	QPointF windowToWorld(const QPointF& pt);
	std::vector<QPointF> worldToWindow(const std::vector<QPointF>& points);
	std::vector<QPointF> windowToWorld(const std::vector<QPointF>& points);

	void deleteWaypoint();
	void deleteWaypoint(std::shared_ptr<xero::paths::RobotPath> path, size_t index);
	void insertWaypoint();
	void addWaypoint(std::shared_ptr<xero::paths::RobotPath> path, size_t index, const xero::paths::Pose2d& pt);

	size_t getSelected() const {
		return selected_;
	}

	void setDemoMode(bool b);

	bool isInsertWaypointValid();
	bool isDeleteWaypointValid();

signals:
	void mouseMoved(xero::paths::Translation2d pos);
	void waypointSelected(size_t index);
	void waypointStartMoving(size_t index);
	void waypointMoving(size_t index);
	void waypointEndMoving(size_t index);
	void waypointInserted();
	void waypointDeleted();

protected:
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void keyPressEvent(QKeyEvent* event) override;

	QSize minimumSizeHint() const override;
	QSize sizeHint() const override;

	DriveBaseModel* getModel() {
		return model_;
	}

private:
	static constexpr double TriangleSize = 8.0;
	static constexpr double SplinePathDiameter = 3.0;
	static constexpr double CenterHitDistance = 8.0;
	static constexpr double RotateBubbleHalo = 5.0;
	static constexpr double CircleRadius = 4.0;
	static constexpr double BigWaypointMove = 12.0;
	static constexpr double SmallWaypointMove = 1.0;
	static constexpr double BigWaypointRotate = 5.0;
	static constexpr double SmallWaypointRotate = 0.5;

	static std::vector<QPointF> triangle_;

	enum WaypointRegion
	{
		None,
		Center,
		Rotate
	};

private:
	void emitMouseMoved(xero::paths::Translation2d pos);
	void emitWaypointSelected(size_t which);
	void emitWaypointStartMoving(size_t which);
	void emitWaypointMoving(size_t which);
	void emitWaypointEndMoving(size_t which);
	void emitWaypointDeleted();
	void emitWaypointInserted();

	void undo();

	void moveWaypoint(bool shift, int dx, int dy);
	void rotateWaypoint(bool shift, int dir);

	std::vector<QPointF> transformPoints(QTransform& trans, const std::vector<QPointF>& points);
	void createTransforms();
	void drawPath(QPainter& paint);
	void drawPoints(QPainter& paint);
	void drawOnePoint(QPainter& paint, const xero::paths::Pose2d& pt, bool selected);
	void drawSplines(QPainter &paint);
	void drawSpline(QPainter& paint, std::shared_ptr<xero::paths::SplinePair> pair, double length);
	void drawRobot(QPainter& paint);
	void drawCursor(QPainter& paint);
	void drawGrid(QPainter& paint);
	void drawEquations(QPainter& paint);
	bool hitTestWaypoint(const QPointF& pt, size_t& index, WaypointRegion& region);
	void invalidateWaypoint(size_t index);

	QString createEquation(QString left, int places, double a, double b, double c, double d, double e, double f);

private:
	QImage field_image_;
	std::shared_ptr<GameField> field_;
	std::shared_ptr<xero::paths::RobotPath> path_;
	double image_scale_;
	QTransform world_to_window_;
	QTransform window_to_world_;
	std::shared_ptr<xero::paths::RobotParams> robot_;
	size_t selected_;
	std::string units_;
	bool dragging_;
	bool rotating_;
	DriveBaseModel* model_;
	size_t demo_step_;

	double triangle_size_;

	bool draw_grid_;
	QColor grid_color_;
	double grid_spacing_;
	double grid_tick_size_;
	double grid_y_label_descend_;
	double grid_x_label_gap_;
	bool grid_complete_;

	bool show_equations_;
	bool equations_stacked_;

	bool cursor_;
	double cursor_time_;
};
