#pragma once

#include "PathFileTreeModel.h"
#include <QTreeView>

class PathFileTree : public QTreeView
{
	Q_OBJECT

public:
	PathFileTree(QWidget *parent = Q_NULLPTR);
	~PathFileTree();

	void setModel(PathFileTreeModel* model) {
		model_ = model;
		QTreeView::setModel(model);
	}

	bool isGroupSelected();
	bool isPathSelected();
	std::shared_ptr<xero::paths::PathGroup> getSelectedGroup();
	std::shared_ptr<xero::paths::RobotPath> getSelectedPath();

private:
	PathFileTreeModel* model_;
};
