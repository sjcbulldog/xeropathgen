#pragma once

#include <QtCore/QString>
#include <cstdint>
#include <list>
#include <memory>
#include <cassert>

class Action
{
public:
	Action(uint32_t id, double atime, const QString& text)
	{
		id_ = id;
		start_time_ = atime;
		text_ = text;
		canceled_ = false;
		ended_ = false;
		parent_ = nullptr;
	}

	virtual ~Action() 
	{
	}

	double getDuration() {
		assert(isDone() || isCanceled());

		return end_time_ - start_time_;
	}

	const std::list<std::shared_ptr<Action>>& getChildren() const {
		return children_;
	}

	bool hasChildren() const {
		return children_.size() > 0;
	}

	Action* getParent() {
		return parent_;
	}

	int level() {
		int level = 0;

		Action* act = parent_;
		while (act != nullptr)
		{
			level++;
			act = act->parent_;
		}

		return level;
	}

	void setParent(Action* parent) {
		parent_ = parent;
	}

	void addChild(std::shared_ptr<Action> act) {
		children_.push_back(act);
		act->setParent(this);
	}

	void setEndTime(double etime)
	{
		end_time_ = etime;
		ended_ = true;
	}

	void setCancelTime(double etime)
	{
		end_time_ = etime;
		canceled_ = true;
	}

	uint32_t getID() const {
		return id_;
	}

	double getStartTime() const {
		return start_time_;
	}

	double getEndTime() const {
		assert(ended_ == true);
		return end_time_;
	}

	double getCancelTime() const {
		assert(canceled_ == true);
		return end_time_;
	}

	bool isDone() const {
		return ended_;
	}

	bool isCanceled() const {
		return canceled_;
	}

	const QString& getText() const {
		return text_;
	}


private:
	double start_time_;
	double end_time_;
	uint32_t id_;
	QString text_;
	bool canceled_;
	bool ended_;
	Action* parent_;

	std::list<std::shared_ptr<Action>> children_;
};

