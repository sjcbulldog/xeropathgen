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
#include "ShowRobotWindow.h"
#include "PathFieldView.h"
#include <TrajectoryNames.h>
#include <QPainter>
#include <QImage>
#include <QPolygon>
#include <QTextOption>

using namespace xero::paths;

QVector<QPoint> ShowRobotWindow::ArrowShape =
{
	QPoint(0, -ArrowBodyWidth / 2),
	QPoint(ArrowBodyLength, -ArrowBodyWidth / 2),
	QPoint(ArrowBodyLength, -ArrowHeadWidth / 2),
	QPoint(ArrowBodyLength + ArrowHeadLength, 0),
	QPoint(ArrowBodyLength, ArrowHeadWidth / 2),
	QPoint(ArrowBodyLength, ArrowBodyWidth / 2),
	QPoint(0, ArrowBodyWidth / 2),
};

ShowRobotWindow::ShowRobotWindow(PathFieldView* view, QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::WindowStaysOnTopHint | windowFlags());
	view_ = view;

	resize(512, 512);
}

ShowRobotWindow::~ShowRobotWindow()
{
}

void ShowRobotWindow::getRobotBounds(const QRect& bounds, QPoint &fl, QPoint &fr, QPoint &bl, QPoint &br)
{
	int basewidth = bounds.width() / 2;
	int baseheight = bounds.height() / 2;
	int centerX = basewidth;
	int centerY = baseheight;

	fl = QPoint(centerX + baseheight / 2, centerY - basewidth / 2);
	fr = QPoint(centerX + baseheight / 2, centerY + basewidth / 2);
	bl = QPoint(centerX - baseheight / 2, centerY - basewidth / 2);
	br = QPoint(centerX - baseheight / 2, centerY + basewidth / 2);
}

void ShowRobotWindow::augmentRobotTank(const QRect& bounds, QPainter& paint)
{
	QPoint fl, fr, bl, br, pt;
	QPolygon p(ArrowShape);
	QString str;
	QFont font = paint.font();
	font.setPointSize(24);
	paint.setFont(font);
	QFontMetrics metrics(font);
	DriveBaseModel *model = view_->getModel() ;

	getRobotBounds(bounds, fl, fr, bl, br);

	QPen pen(QColor(0x00, 0x00, 0x00, 0xff));
	paint.setPen(pen);
	paint.save();
	paint.translate(fl.rx() + 20, fl.ry() - TireWidth / 2);
	paint.drawPolygon(p);

	str = "vel=" + QString::number(model->getVelocity(TrajectoryName::Left), 'f', 1);
	pt = QPoint(ArrowBodyLength + ArrowHeadLength + 10, 0);
	paint.drawText(pt, str);

	str = "acc=" + QString::number(model->getAcceleration(TrajectoryName::Left), 'f', 1);
	pt = QPoint(ArrowBodyLength + ArrowHeadLength + 10, metrics.lineSpacing());
	paint.drawText(pt, str);

	paint.restore();

	paint.save();
	paint.translate(fr.rx() + 20, fr.ry() + TireWidth / 2);
	paint.drawPolygon(p);

	str = "vel=" + QString::number(model->getVelocity(TrajectoryName::Right), 'f', 1);
	pt = QPoint(ArrowBodyLength + ArrowHeadLength + 10, 0);
	paint.drawText(pt, str);

	str = "acc=" + QString::number(model->getAcceleration(TrajectoryName::Right), 'f', 1);
	pt = QPoint(ArrowBodyLength + ArrowHeadLength + 10, metrics.lineSpacing());
	paint.drawText(pt, str);

	paint.restore();

	str = "heading=" + QString::number(model->getPose(TrajectoryName::Main).getRotation().toDegrees(), 'f', 1);
	QSize s = metrics.size(Qt::TextSingleLine, str);
	QPoint center((fr.rx() + br.rx()) / 2 - s.width() / 2, (fr.ry() + fl.ry()) / 2);
	paint.drawText(center, str);
}

void ShowRobotWindow::augmentRobotSwerve(const QRect& bounds, QPainter& paint)
{
	DriveBaseModel* model = view_->getModel();
	QPoint fl, fr, bl, br;
	double angle, velocity, accel;
	QString str;
	QFont font = paint.font();
	font.setPointSize(24);
	paint.setFont(font);
	QFontMetrics metrics(font);

	QPen pen(QColor(0, 0, 0, 0xff));
	pen.setWidth(4);
	paint.setPen(pen);

	getRobotBounds(bounds, fl, fr, bl, br);
	angle = model->getPose(TrajectoryName::FL).getRotation().toDegrees();
	velocity = model->getVelocity(TrajectoryName::FL);
	accel = model->getAcceleration(TrajectoryName::FL);
	paint.save();
	paint.translate(fl.rx(), fl.ry());
	paint.rotate(-angle);
	paint.drawLine(0, 0, VectorLength, 0);
	paint.drawLine(VectorLength, 0, VectorLength - VectorHeadSize, -VectorHeadSize);
	paint.drawLine(VectorLength, 0, VectorLength - VectorHeadSize, VectorHeadSize);
	str = "ang=" + QString::number(angle, 'f', 1);
	paint.drawText(VectorLength + 10, -metrics.lineSpacing(), str);
	str = "vel=" + QString::number(velocity, 'f', 1);
	paint.drawText(VectorLength + 10, 0, str);
	str = "acc=" + QString::number(accel, 'f', 1);
	paint.drawText(VectorLength + 10, metrics.lineSpacing(), str);
	paint.restore();

	angle = model->getPose(TrajectoryName::FR).getRotation().toDegrees();
	velocity = model->getVelocity(TrajectoryName::FR);
	accel = model->getAcceleration(TrajectoryName::FR);
	paint.save();
	paint.translate(fr.rx(), fr.ry());
	paint.rotate(-angle);
	paint.drawLine(0, 0, VectorLength, 0);
	paint.drawLine(VectorLength, 0, VectorLength - VectorHeadSize, -VectorHeadSize);
	paint.drawLine(VectorLength, 0, VectorLength - VectorHeadSize, VectorHeadSize);
	str = "ang=" + QString::number(angle, 'f', 1);
	paint.drawText(VectorLength + 10, -metrics.lineSpacing(), str);
	str = "vel=" + QString::number(velocity, 'f', 1);
	paint.drawText(VectorLength + 10, 0, str);
	str = "acc=" + QString::number(accel, 'f', 1);
	paint.drawText(VectorLength + 10, metrics.lineSpacing(), str);
	paint.restore();

	angle = model->getPose(TrajectoryName::BL).getRotation().toDegrees();
	velocity = model->getVelocity(TrajectoryName::BL);
	accel = model->getAcceleration(TrajectoryName::BL);
	paint.save();
	paint.translate(bl.rx(), bl.ry());
	paint.rotate(-angle);
	paint.drawLine(0, 0, VectorLength, 0);
	paint.drawLine(VectorLength, 0, VectorLength - VectorHeadSize, -VectorHeadSize);
	paint.drawLine(VectorLength, 0, VectorLength - VectorHeadSize, VectorHeadSize);
	str = "ang=" + QString::number(angle, 'f', 1);
	paint.drawText(VectorLength + 10, -metrics.lineSpacing(), str);
	str = "vel=" + QString::number(velocity, 'f', 1);
	paint.drawText(VectorLength + 10, 0, str);
	str = "acc=" + QString::number(accel, 'f', 1);
	paint.drawText(VectorLength + 10, metrics.lineSpacing(), str);
	paint.restore();

	angle = model->getPose(TrajectoryName::BR).getRotation().toDegrees();
	velocity = model->getVelocity(TrajectoryName::BR);
	accel = model->getAcceleration(TrajectoryName::BR);
	paint.save();
	paint.translate(br.rx(), br.ry());
	paint.rotate(-angle);
	paint.drawLine(0, 0, VectorLength, 0);
	paint.drawLine(VectorLength, 0, VectorLength - VectorHeadSize, -VectorHeadSize);
	paint.drawLine(VectorLength, 0, VectorLength - VectorHeadSize, VectorHeadSize);
	str = "ang=" + QString::number(angle, 'f', 1);
	paint.drawText(VectorLength + 10, -metrics.lineSpacing(), str);
	str = "vel=" + QString::number(velocity, 'f', 1);
	paint.drawText(VectorLength + 10, 0, str);
	str = "acc=" + QString::number(accel, 'f', 1);
	paint.drawText(VectorLength + 10, metrics.lineSpacing(), str);
	paint.restore();

	str = "angle=" + QString::number(model->getAngle(), 'f', 1);
	QSize s = metrics.size(Qt::TextSingleLine, str);
	QPoint center((fr.rx() + br.rx()) / 2 - s.width() / 2, (fr.ry() + fl.ry()) / 2);
	paint.drawText(center, str);
}

void ShowRobotWindow::drawRobot(const QRect& bounds, QPainter& paint)
{
	QPoint fl, fr, bl, br;
	QRect r;

	getRobotBounds(bounds, fl, fr, bl, br);
	QPoint bumperl(fl.rx() - bumpermargin, fl.ry());
	QPoint bumperr(fr.rx() - bumpermargin, fr.ry());

	QPen pen(QColor(0, 0, 0, 0xff));
	pen.setWidth(4);
	paint.setPen(pen);
	paint.drawLine(fl, bl);
	paint.drawLine(bl, br);
	paint.drawLine(br, fr);
	paint.drawLine(fl, fr);
	paint.drawLine(bumperl, bumperr);

	QBrush brush(QColor(0, 0, 0xff, 0xff));
	paint.setPen(Qt::PenStyle::NoPen);
	paint.setBrush(brush);

	r = QRect(fl.rx() - TireMargin - TireLength, fl.ry() - TireWidth, TireLength, TireWidth);
	paint.drawRect(r);

	r = QRect(bl.rx() + TireMargin, bl.ry() - TireWidth, TireLength, TireWidth);
	paint.drawRect(r);

	r = QRect(fr.rx() - TireMargin - TireLength, fr.ry(), TireLength, TireWidth);
	paint.drawRect(r);

	r = QRect(br.rx() + TireMargin, br.ry(), TireLength, TireWidth);
	paint.drawRect(r);

	if (view_->getModel()->isTankDrive())
		augmentRobotTank(bounds, paint);
	else
		augmentRobotSwerve(bounds, paint);
}

void ShowRobotWindow::paintEvent(QPaintEvent* event)
{
  	(void)event ;
	
	QPainter paint(this);
	QRect r;
	DriveBaseModel* model = view_->getModel();

	if (model == nullptr)
		return;

	QImage image(1024, 1024, QImage::Format_RGB32);
	image.fill(palette().color(QWidget::backgroundRole()));

	QPainter impaint(&image);
	r = QRect(0, 0, image.size().width(), image.size().height());
	drawRobot(r, impaint);

	QTransform transform;
	transform.rotate(-model->getAngle());
	QImage rotimage = image.transformed(transform, Qt::TransformationMode::SmoothTransformation);

	int cx = 0;
	int cy = 0;
	int width = size().width();
	int height = size().height() ;
	r = QRect(cx, cy, width, height);
	paint.drawImage(r, rotimage);
}
