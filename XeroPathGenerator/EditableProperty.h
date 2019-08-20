#pragma once
#include <QString>
#include <QVariant>

class EditableProperty
{
public:
	enum PropertyType
	{
		PTDouble,
		PTString,
		PTStringList,
		PTInteger,
	};

public:
	EditableProperty(QString name, PropertyType type, QVariant value, QString desc, bool ronly = false) {
		name_ = name;
		type_ = type;
		value_ = value;
		description_ = desc;
		readonly_ = ronly;
	}

	QString getName() const {
		return name_;
	}

	PropertyType getType() const {
		return type_;
	}

	QVariant getValue() const {
		return value_;
	}

	void setValue(QVariant v) {
		value_ = v;
	}

	QString getDescription() const {
		return description_;
	}

	bool isReadOnly() const {
		return readonly_;
	}

	void addChoice(QString str) {
		choices_.push_back(str);
	}

	const QList<QString>& getChoices() const {
		return choices_;
	}

private:
	PropertyType type_;
	QString name_;
	QVariant value_;
	QString description_;
	QList<QString> choices_;
	bool readonly_;
};

