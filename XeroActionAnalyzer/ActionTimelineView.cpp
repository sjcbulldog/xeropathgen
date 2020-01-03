#include "ActionTimelineView.h"
#include <QPainter>
#include <QRectF>

ActionTimelineView::ActionTimelineView(ActionDB &db, QWidget* parent) : QWidget(parent), db_(db)
{
	side_margin_ = 48.0;
	top_margin_ = 4.0;
	top_text_area_ = 16;
	bar_height_ = 12.0;
	slot_height_ = 22;
	slot_bar_height_ = 14;
	action_number_width_ = 24;
	bar_slot_margin_ = 4;

	max_time_ = 0;
	id_ = -1;
}

ActionTimelineView::~ActionTimelineView()
{
}

double ActionTimelineView::topOfBar()
{
	return top_margin_ + top_text_area_;
}

double ActionTimelineView::bottomOfBar()
{
	return topOfBar() + bar_height_;
}

double ActionTimelineView::leftOfBar()
{
	return side_margin_ + action_number_width_;
}

double ActionTimelineView::rightOfBar()
{
	return width() - side_margin_;
}

double ActionTimelineView::timeToWindow(double t)
{
	double w = (rightOfBar() - leftOfBar()) / max_time_;
	return leftOfBar() + w * t;
}

double ActionTimelineView::slotToWindow(int slot)
{
	return bottomOfBar() + bar_slot_margin_ + slot_ * slot_height_;
}

void ActionTimelineView::paintTimes(QPainter& paint)
{
	double w = (width() - side_margin_ * 2) / max_time_;

	QPen pen(QColor(0, 0, 0, 255));
	paint.save();
	paint.setPen(pen);

	QFontMetrics fm(paint.font());

	for (int i = 0; i <= max_time_; i++)
	{
		double x = timeToWindow(static_cast<double>(i));
		QString str = QString::number(i);
		int txtwidth = fm.horizontalAdvance(str);
		int txtheight = fm.height();

		QPointF p(x - txtwidth / 2.0, topOfBar() - txtheight / 2);
		paint.drawText(p, str);
	}

	paint.restore();
}

void ActionTimelineView::paintBar(QPainter& paint)
{
	paint.save();

	QBrush brush(QColor(0, 192, 0));

	QRectF rf(leftOfBar(), topOfBar(), rightOfBar() - leftOfBar(), bar_height_);
	paint.fillRect(rf, brush);

	paint.restore();
}

void ActionTimelineView::paintAction(QPainter& paint, std::shared_ptr<Action> act)
{
	if (act->hasChildren())
	{
		for (auto child : act->getChildren())
		{
			paintAction(paint, child);
		}
	}


	paint.save();
	//
	// Draw the action in the current slot
	//
	double y = slotToWindow(slot_);
	double xs, xe;

	xs = timeToWindow(act->getStartTime());
	if (act->isDone())
		xe = timeToWindow(act->getEndTime());
	else if (act->isCanceled())
		xe = timeToWindow(act->getCancelTime());
	else
		xe = max_time_;

	if (act->hasChildren())
	{
		QBrush b(QColor(0, 0, 255, 255));
		paint.setBrush(b);
	}
	else
	{
		QBrush b(QColor(0, 255, 255, 255));
		paint.setBrush(b);
	}

	if (xe - xs < 0.01)
	{
		QPointF pts[3];
		pts[0] = QPointF(xs, y);
		pts[1] = QPointF(xs - 3.0, y + slot_bar_height_);
		pts[2] = QPointF(xs + 3.0, y + slot_bar_height_);
		paint.drawPolygon(pts, 3);
	}
	else
	{
		QRectF rf(xs, y, xe - xs, slot_bar_height_);
		paint.drawRect(rf);
	}

	QString txt = QString::number(act->getID());
	QFontMetricsF fm(paint.font());
	QPointF pt(xs - fm.horizontalAdvance(txt) - 8.0, y + slot_bar_height_ / 2 + fm.height() / 2);
	paint.drawText(pt, txt);

	txt = act->getText();
	if (txt.length() > 60)
		txt = txt.left(60);
	pt = QPointF(xe + 8.0, y + slot_bar_height_ / 2 + fm.height() / 2);
	paint.drawText(pt, txt);

	paint.restore();
	slot_++;

	setMinimumHeight(static_cast<int>(slotToWindow(slot_)));
}

void ActionTimelineView::resizeEvent(QResizeEvent* ev)
{

}

void ActionTimelineView::paintEvent(QPaintEvent* ev)
{
	QPainter painter(this);

	paintBar(painter);
	paintTimes(painter);
	if (id_ != -1)
	{
		std::shared_ptr<Action> act = db_.findActionByID(id_);
		if (act != nullptr)
		{
			slot_ = 0;
			paintAction(painter, act);
		}
	}
}

void ActionTimelineView::updateContents(int id)
{
	id_ = id;

	double mt = db_.getLastEventTime();
	max_time_ = static_cast<int>(mt);
	if (fmod(mt, 1.0) > 0.015)
		max_time_++;

	update();
}