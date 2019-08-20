#include "ConstraintTreeModel.h"
#include <DistanceVelocityConstraint.h>
#include <QModelIndex>
#include <QDebug>
#include <QColor>

using namespace xero::paths;

ConstraintTreeModel::ConstraintTreeModel()
{
}

ConstraintTreeModel::~ConstraintTreeModel()
{
}

QModelIndex ConstraintTreeModel::index(int row, int col, const QModelIndex& parent) const
{
	(void)parent ;
	return createIndex(row, col);
}

QModelIndex ConstraintTreeModel::parent(const QModelIndex& index) const
{
  	(void)index ;
	return QModelIndex();
}

Qt::ItemFlags ConstraintTreeModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled;

	return Qt::ItemFlag::ItemIsEditable | Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled;
}

int ConstraintTreeModel::rowCount(const QModelIndex& parent) const
{
	if (path_ == nullptr)
		return 0;

	if (!generator_->hasProperty(Generator::DistanceContraintPropertyName))
		return 0;
	
	if (parent.isValid())
		return 0;

	return static_cast<int>(path_->getConstraints().size());
}

int ConstraintTreeModel::columnCount(const QModelIndex& parent) const
{
  	(void)parent ;
	return 3;
}

QVariant ConstraintTreeModel::headerData(int section, Qt::Orientation orient, int role) const
{
	QVariant ret;

	if (role == Qt::DisplayRole && orient == Qt::Orientation::Horizontal)
	{
		switch (section)
		{
		case 0:
			ret = "After (distance)";
			break;

		case 1:
			ret = "Before (distance)";
			break;

		case 2:
			ret = "Velocity";
			break;
		}
	}
	return ret;
}

bool ConstraintTreeModel::isRowValid(int row) const
{
	std::shared_ptr<PathConstraint> con;
	con = path_->getConstraints()[row];
	std::shared_ptr<DistanceVelocityConstraint> dist = std::dynamic_pointer_cast<DistanceVelocityConstraint>(con);

	if (dist->getAfter() >= dist->getBefore())
		return false;

	if (dist->getVelocity() <= 0.0)
		return false;

	return true;
}

QVariant ConstraintTreeModel::data(const QModelIndex& index, int role) const
{
	QVariant ret;

	if (path_ == nullptr)
		return ret;

	if (role == Qt::ForegroundRole)
	{
		if (!isRowValid(index.row()))
			ret = QVariant(QColor(0xff, 0x00, 0x00, 0xFF));
	}
	else if (role == Qt::EditRole || role == Qt::DisplayRole)
	{
		//
		// This is the constraint
		//
		std::shared_ptr<PathConstraint> con;
		con = path_->getConstraints()[index.row()];
		std::shared_ptr<DistanceVelocityConstraint> dist = std::dynamic_pointer_cast<DistanceVelocityConstraint>(con);

		if (dist != nullptr)
		{
			switch (index.column())
			{
			case 0:
				ret = QString::number(dist->getAfter(), 'f', 1);
				break;

			case 1:
				ret = QString::number(dist->getBefore(), 'f', 1);
				break;

			case 2:
				ret = QString::number(dist->getVelocity(), 'f', 1);
				break;
			}
		}
	}
	return ret;
}

bool ConstraintTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  	(void)role ;
  
	//
	// This is the constraint
	//
	std::shared_ptr<PathConstraint> con;
	con = path_->getConstraints()[index.row()];
	std::shared_ptr<DistanceVelocityConstraint> dist = std::dynamic_pointer_cast<DistanceVelocityConstraint>(con);

	if (dist != nullptr)
	{
		switch (index.column())
		{
		case 0:
			dist->setAfter(value.toDouble());
			emitConstraintChanged();
			break;

		case 1:
			dist->setBefore(value.toDouble());
			emitConstraintChanged();
			break;

		case 2:
			dist->setVelocity(value.toDouble());
			emitConstraintChanged();
			break;
		}
	}

	return true;
}


void ConstraintTreeModel::reset()
{
	beginResetModel();
	endResetModel();
}

void ConstraintTreeModel::emitConstraintChanged()
{
	emit constraintChanged();
}
