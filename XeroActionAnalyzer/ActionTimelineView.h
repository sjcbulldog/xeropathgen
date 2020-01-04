#pragma once

#include "Action.h"
#include "ActionDB.h"
#include <QWidget>

class ActionTimelineView : public QWidget
{
	Q_OBJECT

public:
	ActionTimelineView(ActionDB &db, QWidget *parent = Q_NULLPTR);
	~ActionTimelineView();

	void updateContents(int id);

protected:
	void resizeEvent(QResizeEvent* ev) override;
	void paintEvent(QPaintEvent* ev) override;
	void mouseMoveEvent(QMouseEvent* ev) override;
	bool event(QEvent*) override;

	void paintBar(QPainter& paint);
	void paintTimes(QPainter& paint);
	void paintAction(QPainter& paint, std::shared_ptr<Action> act);


private:
	double timeToWindow(double t);
	double topOfBar();
	double bottomOfBar();
	double rightOfBar();
	double leftOfBar();
	double slotToWindow(int slot);

	void addActionRect(const QRectF& rect, std::shared_ptr<Action> act);
	std::shared_ptr<Action> findActionFromPoint(const QPointF& pt);

private:
	ActionDB& db_;

	double side_margin_;
	double top_margin_;
	double top_text_area_;
	double bar_height_;
	double slot_height_;
	double slot_bar_height_;
	double action_number_width_;
	double bar_slot_margin_;

	int max_time_;
	int id_;
	int slot_;

	std::shared_ptr<Action> highlight_;

	std::list<std::pair<QRectF, std::shared_ptr<Action>>> action_rects_;
};
