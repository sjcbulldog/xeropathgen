#pragma once

#include <QHash>
#include <QString>
#include <QVariant>

class GeneratorParameterStore : public QHash<QString, QVariant>
{
public:
	GeneratorParameterStore();
	virtual ~GeneratorParameterStore();

	bool hasParameterValue(const QString& key) {
		return find(key) != end();
	}

	void assignValues(const QHash<QString, QVariant>& value);
};
