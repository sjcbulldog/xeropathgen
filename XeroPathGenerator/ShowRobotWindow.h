#pragma once

#include <QWidget>
#include "ui_ShowRobotWindow.h"

class PathFieldView;

class ShowRobotWindow : public QWidget
{
	Q_OBJECT

public:
	ShowRobotWindow(PathFieldView *view, QWidget *parent = Q_NULLPTR);
	~ShowRobotWindow();

protected:
	void paintEvent(QPaintEvent* event) override;

private:
	static constexpr int bumpermargin = 10;
	static constexpr int TireWidth = 32;
	static constexpr int TireLength = 96;
	static constexpr int TireMargin = 16;
	static constexpr int ArrowBodyLength = 48;
	static constexpr int ArrowHeadLength = 24;
	static constexpr int ArrowBodyWidth = 12;
	static constexpr int ArrowHeadWidth = 24;
	static constexpr int VectorLength = 52;
	static constexpr int VectorHeadSize = 8;

private:
	void getRobotBounds(const QRect& bounds, QPoint& fl, QPoint& fr, QPoint& bl, QPoint& br);
	void drawRobot(const QRect& size, QPainter& paint);
	void augmentRobotTank(const QRect& size, QPainter& paint);
	void augmentRobotSwerve(const QRect& size, QPainter& paint);

	static QVector<QPoint> ArrowShape;

private:
	Ui::ShowRobotWindow ui;
	PathFieldView* view_;
};
