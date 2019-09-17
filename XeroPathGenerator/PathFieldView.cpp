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
#include "PathFieldView.h"
#include "SplinePair.h"
#include "TankDriveBaseModel.h"
#include "SwerveDriveBaseModel.h"
#include "UndoManager.h"
#include <TrajectoryNames.h>
#include <Pose2d.h>
#include <QPainter>
#include <QPointF>
#include <QMouseEvent>
#include <QFontMetrics>
#include <cmath>

using namespace xero::paths;

std::vector<QPointF> PathFieldView::triangle_ =
{
	{ TriangleSize, 0.0},
	{ -TriangleSize / 2.0, TriangleSize / 2.0},
	{ -TriangleSize / 2.0, -TriangleSize / 2.0 }
};

PathFieldView::PathFieldView(QWidget *parent) : QWidget(parent)
{
	units_ = "in";
	setMouseTracking(true);
	setFocusPolicy(Qt::ClickFocus);
	selected_ = std::numeric_limits<size_t>::max();
	image_scale_ = 1.0;
	dragging_ = false;
	rotating_ = false;
	model_ = nullptr;
	cursor_ = true;

	draw_grid_ = true;
	grid_color_ = QColor(0xC0, 0xC0, 0xC0, 0xFF);
	grid_spacing_ = 36.0;
	grid_tick_size_ = 6.0;
	grid_y_label_descend_ = -8.0;
	grid_x_label_gap_ = -4.0;

	grid_complete_ = true;

	show_equations_ = true;
	equations_stacked_ = true;
}

PathFieldView::~PathFieldView()
{
}

void PathFieldView::setUnits(const std::string& units)
{
	grid_spacing_ = UnitConverter::convert(grid_spacing_, units_, units);
	grid_tick_size_ = UnitConverter::convert(grid_tick_size_, units_, units);
	grid_y_label_descend_ = UnitConverter::convert(grid_y_label_descend_, units_, units);
	grid_x_label_gap_ = UnitConverter::convert(grid_x_label_gap_, units_, units);

	for (size_t i = 0; i < triangle_.size(); i++)
	{
		double x = UnitConverter::convert(triangle_[i].rx(), units_, units);
		double y = UnitConverter::convert(triangle_[i].ry(), units_, units);
		triangle_[i] = QPointF(x, y);
	}

	units_ = units;
	createTransforms();
	repaint();
}

bool PathFieldView::isInsertWaypointValid()
{
	if (path_ == nullptr)
		return false;

	//
	// The -1 is because insert is also not valid if the last waypoint is selected
	//
	return selected_ < path_->size() - 1;
}

bool PathFieldView::isDeleteWaypointValid()
{
	if (path_ == nullptr)
		return false;

	return selected_ < path_->size() - 1 && selected_ != 0;
}

void PathFieldView::resetDemo()
{
	model_->reset();
	demo_step_ = 0;
}

void PathFieldView::demoStep()
{
	demo_step_++;
	if (model_ != nullptr)
	{
		model_->setDemoStep(demo_step_);
	}
}

double PathFieldView::getDemoTime()
{
	if (model_ != nullptr)
		return model_->getTime();

	return 0.0;
}

bool PathFieldView::isDemoPathDone()
{
	if (model_ != nullptr)
		return model_->isDone();

	return true;
}

void PathFieldView::setDemoMode(bool b)
{
	if (b)
	{
		selected_ = std::numeric_limits<size_t>::max();
		dragging_ = false;
		rotating_ = false;

		if (robot_->getDriveType() == RobotParams::DriveType::TankDrive)
			model_ = new TankDriveBaseModel(path_, robot_);
		else
			model_ = new SwerveDriveBaseModel(path_, robot_);

		resetDemo();
	}
	else
	{
		if (model_ != nullptr)
			delete model_;

		model_ = nullptr;
	}

	repaint();
}

QSize PathFieldView::minimumSizeHint() const
{
	return QSize(field_image_.size().width() / 4, field_image_.size().height() / 4);
}

QSize PathFieldView::sizeHint() const
{
	return field_image_.size();
}

void PathFieldView::paintEvent(QPaintEvent* event)
{
  	(void)event ;

	//
	// Draw the field
	//
	QPainter paint(this);
	QRectF rect(0.0f, 0.0f, field_image_.width() * image_scale_, field_image_.height() * image_scale_);
	paint.drawImage(rect, field_image_);

	//
	// Draw the grid
	//
	if (draw_grid_)
		drawGrid(paint);

	//
	// Draw the path
	//
	if (path_ != nullptr)
		drawPath(paint);

	if (model_ != nullptr)
		drawRobot(paint);

	if (show_equations_)
		drawEquations(paint);

	if (cursor_ && model_ == nullptr && selected_ == std::numeric_limits<size_t>::max())
		drawCursor(paint);

}

void PathFieldView::drawEquations(QPainter& paint)
{
	if (path_ == nullptr)
		return;

	std::shared_ptr<SplinePair> pair = path_->getSplineAtTime(cursor_time_);
	if (pair != nullptr)
	{
		int topgap = 10;
		int topbottom = 5;
		int side = 15;
		paint.save();

		QFont font = paint.font();
		font.setBold(true);
		font.setPointSize(12);
		paint.setFont(font);

		QString xequ, yequ;

		xequ = createEquation("x", 2, pair->getX().a(), pair->getX().b(), pair->getX().c(), pair->getX().d(), pair->getX().e(), pair->getX().f());
		yequ = createEquation("y", 2, pair->getY().a(), pair->getY().b(), pair->getY().c(), pair->getY().d(), pair->getY().e(), pair->getY().f());

		QString equ = xequ + "         " + yequ;

		QFontMetrics metric(font);
		QRect r;

		int totalw, totalh;

		if (equations_stacked_)
		{
			totalw = std::max(metric.horizontalAdvance(xequ), metric.horizontalAdvance(yequ)) + 2 * side;
			totalh = 2 * metric.lineSpacing() + 2 * topbottom;
		}
		else
		{
			totalw = metric.horizontalAdvance(equ) + 2 * side;
			totalh = metric.lineSpacing() + 2 * topbottom;
		}

		r = QRect(width() / 2 - totalw / 2, topgap, totalw, totalh);
		paint.setBrush(QBrush(QColor(0x40, 0x40, 0x40, 0x90)));
		paint.setPen(Qt::NoPen);
		paint.drawRect(r);

		paint.setBrush(QBrush(QColor(0xff, 0xff, 0xff, 0xff)));
		paint.setPen(QPen(QColor(0xff, 0xff, 0xff, 0xff)));

		QTextOption option;
		option.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		if (equations_stacked_)
		{
			r = QRect(width() / 2 - totalw / 2, topgap + topbottom, totalw, metric.lineSpacing());
			paint.drawText(r, xequ, option);

			r = QRect(width() / 2 - totalw / 2, topgap + topbottom + metric.lineSpacing(), totalw, metric.lineSpacing());
			paint.drawText(r, yequ, option);
		}
		else
		{
			paint.drawText(r, equ, option);
		}

		paint.restore();
	}
}

QString PathFieldView::createEquation(QString left, int places, double a, double b, double c, double d, double e, double f)
{
	QString str = left + "=";

	str += QString::number(a, 'f', 3) + QString("*t^5");
	str += QString(" + ") + QString::number(b, 'f', places) + QString("*t^4");
	str += QString(" + ") + QString::number(c, 'f', places) + QString("*t^3");
	str += QString(" + ") + QString::number(d, 'f', places) + QString("*t^2");
	str += QString(" + ") + QString::number(e, 'f', places) + QString("*t");
	str += QString(" + ") + QString::number(f, 'f', places);
	return str;
}

void PathFieldView::drawGrid(QPainter& paint)
{
	QPointF p1, p2;

	paint.save();
	paint.setPen(QPen(grid_color_));
	paint.setBrush(Qt::BrushStyle::NoBrush);

	QFont f = paint.font();
	f.setBold(true);
	f.setPointSize(14);
	paint.setFont(f);
	QFontMetrics metrics(f);

	for (double x = 0; x < field_->getSize().getX(); x += grid_spacing_)
	{
		if (grid_complete_)
		{
			p1 = QPointF(x, field_->getSize().getY());
			p2 = QPointF(x, 0);

			p1 = worldToWindow(p1);
			p2 = worldToWindow(p2);

			paint.drawLine(p1, p2);
		}
		else
		{
			p1 = QPointF(x, field_->getSize().getY());
			p2 = QPointF(x, field_->getSize().getY() - grid_tick_size_);

			p1 = worldToWindow(p1);
			p2 = worldToWindow(p2);

			paint.drawLine(p1, p2);

			p1 = QPointF(x, 0.0);
			p2 = QPointF(x, grid_tick_size_);

			p1 = worldToWindow(p1);
			p2 = worldToWindow(p2);

			paint.drawLine(p1, p2);
		}

		paint.save();

		QString txt = QString::number(x, 'f', 0);
		int totalw = metrics.horizontalAdvance(txt);
		int totalh = metrics.lineSpacing();

		paint.setBrush(QBrush(QColor(0x0, 0x0, 0x0, 0x80)));
		paint.setPen(Qt::NoPen);

		p1 = QPointF(x, grid_y_label_descend_);
		p1 = worldToWindow(p1);

		QRectF r(p1.rx() - totalw / 2, p1.ry() - totalh / 3.0, totalw, totalh);
		paint.drawRect(r);

		paint.setBrush(QBrush(QColor(0xFF, 0xFF, 0xFF, 0xFF)));
		paint.setPen(QPen(QColor(0xff, 0xff, 0xff, 0xff)));

		QTextOption option;
		option.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		paint.drawText(r, txt, option);

		paint.restore();
	}

	for (double y = 0; y < field_->getSize().getY(); y += grid_spacing_)
	{
		if (grid_complete_)
		{
			p1 = QPointF(0, y);
			p2 = QPointF(field_->getSize().getX(), y);

			p1 = worldToWindow(p1);
			p2 = worldToWindow(p2);

			paint.drawLine(p1, p2);
		}
		else
		{
			p1 = QPointF(field_->getSize().getX(), y);
			p2 = QPointF(field_->getSize().getX() - grid_tick_size_, y);

			p1 = worldToWindow(p1);
			p2 = worldToWindow(p2);

			paint.drawLine(p1, p2);

			p1 = QPointF(0.0, y);
			p2 = QPointF(grid_tick_size_, y);

			p1 = worldToWindow(p1);
			p2 = worldToWindow(p2);

			paint.drawLine(p1, p2);
		}

		paint.save();

		QString txt = QString::number(y, 'f', 0);
		int totalw = metrics.horizontalAdvance(txt);
		int totalh = metrics.lineSpacing();

		p1 = QPointF(grid_x_label_gap_, y);
		p1 = worldToWindow(p1);

		paint.setBrush(QBrush(QColor(0x0, 0x0, 0x0, 0x80)));
		paint.setPen(Qt::NoPen);

		QRectF r(p1.rx() - totalw, p1.ry() - totalh / 2.0, totalw, totalh);
		paint.drawRect(r);

		paint.setBrush(QBrush(QColor(0xFF, 0xFF, 0xFF, 0xFF)));
		paint.setPen(QPen(QColor(0xff, 0xff, 0xff, 0xff)));

		QTextOption option;
		option.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		paint.drawText(r, txt, option);
		paint.restore();
	}

	paint.restore();
}

void PathFieldView::emitMouseMoved(Translation2d pos)
{
	emit mouseMoved(pos);
}

void PathFieldView::emitWaypointSelected(size_t index)
{
	emit waypointSelected(index);
}

void PathFieldView::emitWaypointDeleted()
{
	emit waypointDeleted();
}

void PathFieldView::emitWaypointInserted()
{
	emit waypointInserted();
}

void PathFieldView::emitWaypointMoved(size_t index)
{
	emit waypointMoved(index);
}

void PathFieldView::emitWaypointMoving(size_t index)
{
	emit waypointMoving(index);
}

void PathFieldView::mouseMoveEvent(QMouseEvent* ev)
{
	if (model_ != nullptr)
		return;

	QPointF world = windowToWorld(ev->localPos());

	if (dragging_)
	{
		const Pose2d& pt = path_->getPoints()[selected_];
		Translation2d t(world.rx(), world.ry());
		Pose2d newpt(t, pt.getRotation());
		path_->replacePoint(selected_, newpt);

		emitWaypointMoving(selected_);
		repaint(geometry());
	}
	else if (rotating_)
	{
		const Pose2d& pt = path_->getPoints()[selected_];

		double dy = world.ry() - pt.getTranslation().getY();
		double dx = world.rx() - pt.getTranslation().getX();
		double angle = std::atan2(dy, dx) - MathUtils::kPI / 2;
		Rotation2d r = Rotation2d::fromRadians(angle);
		Pose2d newpt(pt.getTranslation(), r);
		path_->replacePoint(selected_, newpt);

		emitWaypointMoving(selected_);
		repaint(geometry());
	}

	emitMouseMoved(Translation2d(world.rx(), world.ry()));
}

void PathFieldView::mousePressEvent(QMouseEvent* ev)
{
	if (ev->buttons() != Qt::LeftButton || model_ != nullptr)
		return;

	size_t index;
	WaypointRegion region;
	if (hitTestWaypoint(QPointF(ev->x(), ev->y()), index, region))
	{
		grabMouse();

		if (region == WaypointRegion::Center)
		{
			if (selected_ != index)
			{
				if (selected_ != std::numeric_limits<size_t>::max())
				{
					selected_ = std::numeric_limits<size_t>::max();
					invalidateWaypoint(selected_);
				}

				selected_ = index;
				invalidateWaypoint(selected_);
				emitWaypointSelected(selected_);
			}
			dragging_ = true;
		}
		else
		{
			rotating_ = true;
		}
	}
	else
	{
		if (selected_ != std::numeric_limits<size_t>::max())
		{
			size_t save = selected_;
			selected_ = std::numeric_limits<size_t>::max();
			invalidateWaypoint(save);
			emitWaypointSelected(selected_);
		}
	}
}

void PathFieldView::mouseReleaseEvent(QMouseEvent* ev)
{
  	(void)ev ;
	
	if (model_ != nullptr)
		return;

	if (dragging_ || rotating_)
		emitWaypointMoved(selected_);

	dragging_ = false;
	rotating_ = false;

	releaseMouse();

}

void PathFieldView::undo()
{
}

void PathFieldView::deleteWaypoint()
{
	if (path_ == nullptr || selected_ >= path_->size())
		return;

	path_->removePoint(selected_);
	emitWaypointDeleted();
	repaint(geometry());
}

void PathFieldView::insertWaypoint()
{
	if (selected_ >= path_->size() - 1)
		return;

	const Pose2d& p1 = path_->getPoints()[selected_];
	const Pose2d& p2 = path_->getPoints()[selected_ + 1];

	Rotation2d r = Rotation2d::fromRadians((p1.getRotation().toRadians() + p2.getRotation().toRadians()) / 2.0);
	Translation2d t((p1.getTranslation().getX() + p2.getTranslation().getX()) / 2.0, (p1.getTranslation().getY() + p2.getTranslation().getY()) / 2.0);
	Pose2d newpt(t, r);
	path_->insertPoint(selected_, newpt) ;

	// Move selected waypoint to the one we just created
	selected_++;

	emitWaypointInserted();
	emitWaypointSelected(selected_);
	repaint(geometry());
}

void PathFieldView::moveWaypoint(bool shift, int dx, int dy)
{
	if (selected_ <= path_->size())
	{
		double delta = shift ? SmallWaypointMove : BigWaypointMove;
		delta = UnitConverter::convert(delta, "in", units_);

		const Pose2d& pt = path_->getPoints()[selected_];
		Translation2d t(pt.getTranslation().getX() + dx * delta, pt.getTranslation().getY() + dy * delta);
		Pose2d newpt(t, pt.getRotation());
		path_->replacePoint(selected_, newpt);

		emitWaypointMoved(selected_);
		repaint(geometry());
	}
}

void PathFieldView::rotateWaypoint(bool shift, int dir)
{
	if (selected_ <= path_->size())
	{
		double delta = shift ? SmallWaypointRotate : BigWaypointRotate;
		delta *= dir;

		const Pose2d& pt = path_->getPoints()[selected_];
		Rotation2d r = Rotation2d::fromDegrees(MathUtils::boundDegrees(pt.getRotation().toDegrees() + delta));
		Pose2d newpt(pt.getTranslation(), r);
		path_->replacePoint(selected_, newpt);

		emitWaypointMoved(selected_);
		repaint(geometry());
	}
}

void PathFieldView::keyPressEvent(QKeyEvent* ev)
{
	if (ev->key() == Qt::Key::Key_Z && ev->modifiers() == Qt::KeyboardModifier::ControlModifier)
	{
		UndoManager &mgr = UndoManager::getUndoManager();
		mgr.undo();
	}
	else
	{
		bool shift = (ev->modifiers() == Qt::KeyboardModifier::ShiftModifier);

		switch (ev->key())
		{
		case Qt::Key::Key_Delete:
			deleteWaypoint();
			break;
		case Qt::Key::Key_Insert:
			insertWaypoint();
			break;
		case Qt::Key::Key_Up:
			moveWaypoint(shift, 0, 1);
			break;
		case Qt::Key::Key_Down:
			moveWaypoint(shift, 0, -1);
			break;
		case Qt::Key::Key_Left:
			moveWaypoint(shift, -1, 0);
			break;
		case Qt::Key::Key_Right:
			moveWaypoint(shift, 1, 0);
			break;
		case Qt::Key::Key_PageUp:
			rotateWaypoint(shift, 1);
			break;
		case Qt::Key::Key_PageDown:
			rotateWaypoint(shift, -1);
			break;
		case Qt::Key::Key_Plus:
			rotateWaypoint(shift, 1);
			break;
		case Qt::Key::Key_Minus:
			rotateWaypoint(shift, -1);
			break;
		}
	}
}

std::vector<QPointF> PathFieldView::transformPoints(QTransform& trans, const std::vector<QPointF>& points)
{
	std::vector<QPointF> result;

	for (const QPointF& pt : points)
	{
		QPointF newpt = trans.map(pt);
		result.push_back(newpt);
	}

	return result;
}

void PathFieldView::drawCursor(QPainter& paint)
{
	if (path_ == nullptr)
		return;

	Pose2dWithTrajectory pt;
	if (!path_->getPoseAtTime(cursor_time_, pt))
		return;

	QTransform mm;
	mm.translate(pt.translation().getX(), pt.translation().getY());
	mm.rotateRadians(pt.rotation().toRadians());

	double rl = robot_->getRobotLength();
	double rw = robot_->getRobotWidth();

	std::vector<QPointF> robot =
	{
		{ rl / 2.0, rw / 2.0 },
		{ -rl / 2.0, rw / 2.0 },
		{ -rl / 2.0, -rw / 2.0 },
		{ rl / 2.0, -rw / 2.0 },
	};

	std::vector<QPointF> wheel =
	{
		{ 0, 0 },
		{ rl / 4.0, 0.0 }, 
		{ rl / 4.0, rw / 8.0},
		{ 0.0, rw / 8.0},
	};

	paint.setBrush(Qt::BrushStyle::NoBrush);
	QPen pen = QPen(QColor(0x00, 0x00, 0x00, 0xff));
	pen.setWidthF(2.0f);
	paint.setPen(pen);

	std::vector<QPointF> drawpts = worldToWindow(transformPoints(mm, robot));
	paint.drawPolygon(&drawpts[0], static_cast<int>(drawpts.size()));

	paint.setBrush(QBrush(QColor(0, 0, 0, 0xff)));

	QTransform trans;
	trans.translate(rl / 4.0, rw / 2.0);
	drawpts = transformPoints(trans, wheel);
	drawpts = worldToWindow(transformPoints(mm, drawpts));
	paint.drawPolygon(&drawpts[0], static_cast<int>(drawpts.size()));

	trans = QTransform();
	trans.translate(rl / 4.0, -rw / 2.0 - rw / 8.0);
	drawpts = transformPoints(trans, wheel);
	drawpts = worldToWindow(transformPoints(mm, drawpts));
	paint.drawPolygon(&drawpts[0], static_cast<int>(drawpts.size()));

	trans = QTransform();
	trans.translate(-rl / 2.0, rw / 2.0);
	drawpts = transformPoints(trans, wheel);
	drawpts = worldToWindow(transformPoints(mm, drawpts));
	paint.drawPolygon(&drawpts[0], static_cast<int>(drawpts.size()));

	trans = QTransform();
	trans.translate(-rl / 2.0, -rw / 2.0 - rw / 8.0);
	drawpts = transformPoints(trans, wheel);
	drawpts = worldToWindow(transformPoints(mm, drawpts));
	paint.drawPolygon(&drawpts[0], static_cast<int>(drawpts.size()));
}

void PathFieldView::drawRobot(QPainter& paint)
{
	if (path_ == nullptr || !path_->hasTrajectory(TrajectoryName::Main) || path_->getTrajectory(TrajectoryName::Main)->size() == 0)
		return;

	Translation2d fl, fr, bl, br;
	model_->getLocation(fl, fr, bl, br);
	std::vector<QPointF> corners;
	corners.push_back(QPointF(fl.getX(), fl.getY()));
	corners.push_back(QPointF(fr.getX(), fr.getY()));
	corners.push_back(QPointF(bl.getX(), bl.getY()));
	corners.push_back(QPointF(br.getX(), br.getY()));

	auto winloc = worldToWindow(corners);

	QPen pen(QColor(0xff, 0xff, 0x00, 0xff));
	pen.setWidthF(2.0f);
	paint.setPen(pen);

	paint.drawLine(winloc[1], winloc[3]);
	paint.drawLine(winloc[2], winloc[3]);
	paint.drawLine(winloc[2], winloc[0]);

	pen = QPen(QColor(0x00, 0x00, 0xff, 0xff));
	pen.setWidthF(2.0f);
	paint.setPen(pen);

	paint.drawLine(winloc[0], winloc[1]);
}

void PathFieldView::drawPath(QPainter &paint)
{
	if (path_->hasSplines())
		drawSplines(paint);

	drawPoints(paint);
}

void PathFieldView::drawPoints(QPainter& paint)
{
	const auto& points = path_->getPoints();

	for (size_t i = 0; i < path_->size(); i++)
		drawOnePoint(paint, points[i], i == selected_);
}

void PathFieldView::drawOnePoint(QPainter& paint, const Pose2d& pt, bool selected)
{
	if (selected)
	{
		QPen pen(QColor(0xff, 0xff, 0x00, 0xff));
		pen.setWidthF(2.0f);
		paint.setPen(pen);
	}
	else
	{
		QPen pen(QColor(0x00, 0x00, 0x00, 0xFF));
		pen.setWidthF(2.0f);
		paint.setPen(pen);
	}
	QBrush brush(QColor(0xFF, 0xA5, 0x00, 0xFF));
	paint.setBrush(brush);

	QTransform mm;
	mm.translate(pt.getTranslation().getX(), pt.getTranslation().getY());
	mm.rotateRadians(pt.getRotation().toRadians());

	std::vector<QPointF> mapped = worldToWindow(transformPoints(mm, triangle_));
	paint.drawPolygon(&mapped[0], 3);

	if (selected)
	{
		double rl = robot_->getRobotLength();
		double rw = robot_->getRobotWidth();

		std::vector<QPointF> robot =
		{
			{ rl / 2.0, rw / 2.0 },
			{ -rl / 2.0, rw / 2.0 },
			{ -rl / 2.0, -rw / 2.0 },
			{ rl / 2.0, -rw / 2.0 },
		};

		std::vector<QPointF> line =
		{
			{ 0.0, rw / 2.0},
			{ 0.0, rw * 3.0 / 4.0 },
		};

		paint.setBrush(Qt::BrushStyle::NoBrush);
		QPen pen(QColor(0xff, 0xff, 0x00, 0xff));
		pen.setWidthF(2.0f);
		paint.setPen(pen);

		std::vector<QPointF> robotsel = worldToWindow(transformPoints(mm, robot));
		paint.drawPolygon(&robotsel[0], static_cast<int>(robotsel.size()));

		std::vector<QPointF> linesel = worldToWindow(transformPoints(mm, line));
		paint.drawLine(linesel[0], linesel[1]);

		paint.setBrush(QBrush(QColor(0xff, 0xff, 0x00, 0xff)));
		QPointF tip(0.0, rw * 3.0 / 4.0);
		QPointF tipsel = worldToWindow(mm.map(tip));
		QRectF circle(tipsel.rx() - CircleRadius, tipsel.ry() - CircleRadius, CircleRadius * 2.0, CircleRadius * 2.0);
		paint.drawEllipse(circle);
	}
}

void PathFieldView::drawSplines(QPainter& paint)
{
	auto splines = path_->getSplines();

	for (size_t i = 0; i < splines.size(); i++)
		drawSpline(paint, splines[i], 2.0 * path_->getDistance(i + 1));
}

void PathFieldView::drawSpline(QPainter& paint, std::shared_ptr<SplinePair> pair, double length)
{
	double elength = UnitConverter::convert(length, units_, "in");
	double step = 1.0 / elength;
	double px, py;

	QColor c(0xF0, 0x80, 0x80, 0xFF);

	QBrush brush(c);
	paint.setBrush(brush);

	QPen pen(c);
	paint.setPen(pen);

	for (float t = 0.0f; t < 1.0f; t += step)
	{
		Translation2d loc = pair->evalPosition(t);
		Rotation2d heading = pair->evalHeading(t);

		px = loc.getX() - robot_->getRobotWidth() * heading.getSin() / 2.0;
		py = loc.getY() + robot_->getRobotWidth() * heading.getCos() / 2.0;

		QPointF qp = worldToWindow(QPointF(px, py));
		paint.drawPoint(qp);

		px = loc.getX() + robot_->getRobotWidth() * heading.getSin() / 2.0;
		py = loc.getY() - robot_->getRobotWidth() * heading.getCos() / 2.0;

		qp = worldToWindow(QPointF(px, py));
		paint.drawPoint(qp);
	}
}

void PathFieldView::resizeEvent(QResizeEvent* event)
{
  	(void)event ;
	createTransforms();
}

void PathFieldView::setField(std::shared_ptr<GameField> field)
{
	field_ = field;

	//
	// Load the image,
	//
	QImage image(field_->getImageFile());
	field_image_ = image;

	//
	// Create new transforms
	//
	createTransforms();

	//
	// Redraw the widget
	//
	repaint(geometry());
}

void PathFieldView::setPath(std::shared_ptr<RobotPath> path)
{
	if (path_ != path)
	{
		path_ = path;
		if (model_ != nullptr)
			model_->setPath(path);
		selected_ = std::numeric_limits<size_t>::max();
		repaint(geometry());
	}
}

void PathFieldView::setRobot(std::shared_ptr<xero::paths::RobotParams> params)
{
	robot_ = params;
}

void PathFieldView::createTransforms()
{
	//
	// First calculate the scale to use to scale the image to the window
	// without changing the aspec ratio.  Pick the largest scale factor that will fit
	// the most constrained dimension
	//
	double sx = (double)width() / (double)field_image_.width();
	double sy = (double)height() / (double)field_image_.height();

	if (sx < sy)
		image_scale_ = sx;
	else
		image_scale_ = sy;

	//
	// Now, we know now to scale the image to window pixels, scale the top left and bottom
	// right coordiantes from the image to the window
	//
	Translation2d tl = field_->getTopLeft().scale(image_scale_);
	Translation2d br = field_->getBottomRight().scale(image_scale_);

	//
	// Now we know two points on the field and how they coorespond to two points on the window.
	// The field point (0, FieldSize.Y) cooresponds to the point tl.
	// The field point (FieldsSize.X, 0) coorespons to the point br.
	//
	// Compute the transform based on this.  
	//
	world_to_window_ = QTransform();

	world_to_window_.translate(tl.getX(), br.getY());
	double scale = (br.getX() - tl.getX()) / field_->getSize().getX();
	world_to_window_.scale(scale, -scale);

	window_to_world_ = world_to_window_.inverted();
}

QPointF PathFieldView::worldToWindow(const QPointF& pt)
{
	return world_to_window_.map(pt);
}

QPointF PathFieldView::windowToWorld(const QPointF& pt)
{
	return window_to_world_.map(pt);
}

std::vector<QPointF> PathFieldView::worldToWindow(const std::vector<QPointF> &points)
{
	return transformPoints(world_to_window_, points);
}

std::vector<QPointF> PathFieldView::windowToWorld(const std::vector<QPointF>& points)
{
	return transformPoints(window_to_world_, points);
}

bool PathFieldView::hitTestWaypoint(const QPointF& pt, size_t& index, WaypointRegion& region)
{
	if (path_ == nullptr)
		return false;

	auto points = path_->getPoints();
	for (size_t i = 0; i < path_->size(); i++)
	{
		QPointF world = worldToWindow(QPointF(points[i].getTranslation().getX(), points[i].getTranslation().getY()));
		double dist = QVector2D(pt - world).length();

		//
		// Test to see if we are close enough to the center of the
		// waypoint to be a h it
		//
		if (dist < CenterHitDistance)
		{
			index = i;
			region = WaypointRegion::Center;
			return true;
		}
	}

	if (selected_ < path_->size())
	{
		//
		// Now see if we hit the rotation bubble on a
		//
		QTransform mm;
		const Pose2d& ppt = points[selected_];
		mm.translate(ppt.getTranslation().getX(), ppt.getTranslation().getY());
		mm.rotateRadians(ppt.getRotation().toRadians());

		QPointF bubble = mm.map(QPointF(0.0, robot_->getRobotWidth() * 3.0 / 4.0));
		QRectF rect(bubble.rx() - RotateBubbleHalo, bubble.ry() - RotateBubbleHalo, RotateBubbleHalo * 2.0, RotateBubbleHalo * 2.0);
		QPointF wpt = windowToWorld(pt);
		if (rect.contains(wpt))
		{
			index = selected_;
			region = WaypointRegion::Rotate;
			return true;
		}
	}
	return false;
}

void PathFieldView::invalidateWaypoint(size_t index)
{
	if (index >= path_->size())
		return;

	//
	// For now, invalidate the entire window
	//
	repaint(geometry());
}
