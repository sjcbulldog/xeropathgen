#pragma once

#include <QWidget>
#include "ui_ConstraintEditor.h"
#include "ConstraintTreeModel.h"

class ConstraintEditor : public QWidget
{
	Q_OBJECT

public:
	ConstraintEditor(QWidget *parent = Q_NULLPTR);
	~ConstraintEditor();

	void setPath(std::shared_ptr<xero::paths::RobotPath> path) {
		model_.setPath(path);
		path_ = path;
		enableDisable();
	}

	void setGenerator(std::shared_ptr<Generator> gen) {
		model_.setGenerator(gen);
		generator_ = gen;
		enableDisable();
	}

signals:
	void constraintAdded();
	void constraintRemoved();
	void constraintChanged();

private:
	void emitConstraintAdded();
	void emitConstraintRemoved();
	void emitConstraintChanged();

private:
	void addOne();
	void deleteOne();
	void enableDisable();

private:
	Ui::ConstraintEditor ui;
	ConstraintTreeModel model_;
	std::shared_ptr<Generator> generator_;
	std::shared_ptr<xero::paths::RobotPath> path_;
};
