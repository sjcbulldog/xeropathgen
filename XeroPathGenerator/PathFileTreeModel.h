#pragma once

#include <Qt>
#include <QAbstractItemModel>
#include <PathCollection.h>

class PathFileTreeModel : public QAbstractItemModel
{
public:
	PathFileTreeModel();
	virtual ~PathFileTreeModel();


	xero::paths::PathCollection& getPathCollection() {
		return paths_;
	}

	size_t getGroupCount() {
		return paths_.size();
	}

	bool isDirty() const {
		return dirty_;
	}

	void setDirty() {
		dirty_ = true;
	}

	void setClean() {
		dirty_ = false;
	}

	QModelIndex index(int row, int col, const QModelIndex& parent) const;
	QModelIndex parent(const QModelIndex& index) const;
	Qt::ItemFlags flags(const QModelIndex& index) const;
	int rowCount(const QModelIndex& parent) const;
	int columnCount(const QModelIndex& parent) const;
	QVariant data(const QModelIndex& index, int role) const;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

	bool containsPathGroup(const std::string& name);
	bool containsPath(const std::string& group, const std::string& name);
	std::shared_ptr<xero::paths::PathGroup> addPathGroup(const std::string& name);
	std::shared_ptr<xero::paths::RobotPath> addRobotPath(const std::string& group, const std::string& name);
	void deleteGroup(const std::string& name);
	void deletePath(const std::string& group, const std::string& name);
	std::shared_ptr<xero::paths::RobotPath> findPathByName(const std::string& group, const std::string& path);
	void clear();
	void reset();

	void getAllPaths(std::list<std::shared_ptr<xero::paths::RobotPath>>& results) {
		paths_.getAllPaths(results);
	}

	void convert(const std::string& from, const std::string& to);

private:
	bool isNameValid(const QString& name);

private:
	xero::paths::PathCollection paths_;
	bool dirty_;
};

