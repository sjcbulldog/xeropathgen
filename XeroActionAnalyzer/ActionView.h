#pragma once

#include "Action.h"
#include <QWidget>
#include <list>
#include <memory>

class ActionView : public QWidget
{
	Q_OBJECT

public:
	ActionView(QWidget *parent = Q_NULLPTR);
	~ActionView();

	void setActionList(std::list<std::shared_ptr<Action>> actions) {
		actions_ = actions;
	}

protected:
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void keyPressEvent(QKeyEvent* event) override;

private:
	std::list<std::shared_ptr<Action>> actions_;
};
