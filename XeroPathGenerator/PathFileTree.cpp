#include "PathFileTree.h"

using namespace xero::paths;

PathFileTree::PathFileTree(QWidget *parent) : QTreeView(parent)
{
}

PathFileTree::~PathFileTree()
{
}

bool PathFileTree::isGroupSelected()
{
	QModelIndex index = currentIndex();
	if (!index.isValid())
		return false;

	PathBase* base = static_cast<PathBase*>(index.internalPointer());
	PathGroup* group = dynamic_cast<PathGroup*>(base);
	return group != nullptr;
}

bool PathFileTree::isPathSelected()
{
	QModelIndex index = currentIndex();
	if (!index.isValid())
		return false;

	PathBase* base = static_cast<PathBase*>(index.internalPointer());
	RobotPath* group = dynamic_cast<RobotPath*>(base);
	return group != nullptr;
}

std::shared_ptr<xero::paths::PathGroup> PathFileTree::getSelectedGroup()
{
	if (!isGroupSelected())
		return nullptr;

	QModelIndex index = currentIndex();
	QVariant groupname = model_->data(index, Qt::DisplayRole);
	return model_->getPathCollection().getGroupByName(groupname.toString().toStdString());
}

std::shared_ptr<xero::paths::RobotPath> PathFileTree::getSelectedPath()
{
	if (!isPathSelected())
		return nullptr;

	QModelIndex pathindex = currentIndex();
	QModelIndex groupindex = pathindex.parent();

	QVariant pathname = model_->data(pathindex, Qt::DisplayRole);
	QVariant groupname = model_->data(groupindex, Qt::DisplayRole);

	auto group = model_->getPathCollection().getGroupByName(groupname.toString().toStdString());
	if (group == nullptr)
		return nullptr;

	return group->findPathByName(pathname.toString().toStdString());
}
