#include "GeneratorParameterStore.h"
#include <QHash>
#include <QVariant>
#include <QString>

GeneratorParameterStore::GeneratorParameterStore()
{
}

GeneratorParameterStore::~GeneratorParameterStore()
{
}

void GeneratorParameterStore::assignValues(const QHash<QString, QVariant>& value)
{
	clear();

	for (auto key : value.keys())
		(*this)[key] = value[key];
}