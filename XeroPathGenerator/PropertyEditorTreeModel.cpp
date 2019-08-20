#include "PropertyEditorTreeModel.h"
#include "EditableProperty.h"

PropertyEditorTreeModel::PropertyEditorTreeModel()
{
}

PropertyEditorTreeModel::~PropertyEditorTreeModel()
{
}

std::shared_ptr<EditableProperty> PropertyEditorTreeModel::getProperty(const QModelIndex& index)
{
	if (!index.isValid())
		return nullptr;

	if (index.row() >= static_cast<int>(props_.size()))
		return nullptr;

	return props_[index.row()];
}

std::shared_ptr<EditableProperty> PropertyEditorTreeModel::getProperty(QString name)
{
	for (auto prop : props_)
	{
		if (prop->getName() == name)
			return prop;
	}

	return nullptr;
}

QModelIndex PropertyEditorTreeModel::index(int row, int col, const QModelIndex& parent) const
{
  	(void)parent ;
	return createIndex(row, col);
}

QModelIndex PropertyEditorTreeModel::parent(const QModelIndex& index) const
{
  	(void)index ;
	return QModelIndex();
}

Qt::ItemFlags PropertyEditorTreeModel::flags(const QModelIndex& index) const
{
	if (!index.isValid() || index.column() != 1 || props_[index.row()]->isReadOnly())
		return Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled;

	return Qt::ItemFlag::ItemIsEditable | Qt::ItemFlag::ItemIsSelectable | Qt::ItemFlag::ItemIsEnabled;
}

int PropertyEditorTreeModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return static_cast<int>(props_.size());
}

int PropertyEditorTreeModel::columnCount(const QModelIndex& parent) const
{
  	(void)parent ;
	return 3;
}

QVariant PropertyEditorTreeModel::headerData(int section, Qt::Orientation orient, int role) const
{
	QVariant ret;

	if (role == Qt::DisplayRole && orient == Qt::Orientation::Horizontal)
	{
		if (section == 0)
			ret = "Name";
		else if (section == 1)
			ret = "Value";
		else
			ret = "Description";
	}

	return ret;
}

QVariant PropertyEditorTreeModel::data(const QModelIndex& index, int role) const
{
	QVariant ret;

	if (role != Qt::DisplayRole && role != Qt::EditRole)
		return ret;

	auto prop = props_[index.row()];
	if (index.column() == 0)
	{

		ret = prop->getName();
	}
	else if (index.column() == 1)
	{
		ret = prop->getValue();
		if (prop->getType() == EditableProperty::PropertyType::PTDouble)
		{
			double d = ret.toDouble();
			ret = QString::number(d, 'f', 2);
		}
		else if (prop->getType() == EditableProperty::PropertyType::PTInteger)
		{
			int d = ret.toInt();
			ret = QString::number(d);
		}
	}
	else if (index.column() == 2)
	{
		ret = prop->getDescription();
	}
	return ret;
}

bool PropertyEditorTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  	(void)role ;
	if (index.isValid())
	{
		assert(index.column() == 1);
		auto prop = props_[index.row()];
		prop->setValue(value);
	}
	return true;
}
