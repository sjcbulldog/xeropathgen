#pragma once

#include "Generator.h"
#include <RobotPath.h>
#include <Qt>
#include <QAbstractItemModel>

class ConstraintTreeModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	ConstraintTreeModel();
	virtual ~ConstraintTreeModel();

	void setPath(std::shared_ptr<xero::paths::RobotPath> path) {
		path_ = path;
		reset();
	}

	void setGenerator(std::shared_ptr<Generator> gen) {
		generator_ = gen;
		reset();
	}

	QModelIndex index(int row, int col, const QModelIndex& parent) const;
	QModelIndex parent(const QModelIndex& index) const;
	Qt::ItemFlags flags(const QModelIndex& index) const;
	int rowCount(const QModelIndex& parent) const;
	int columnCount(const QModelIndex& parent) const;
	virtual QVariant headerData(int section, Qt::Orientation orient, int role = Qt::DisplayRole) const;
	QVariant data(const QModelIndex& index, int role) const;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

	void reset();

signals:
	void constraintChanged();

private:
	void emitConstraintChanged();
	bool isRowValid(int row) const;

private:
	std::shared_ptr<xero::paths::RobotPath> path_;
	std::shared_ptr<Generator> generator_;
};
