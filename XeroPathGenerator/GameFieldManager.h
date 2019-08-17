#pragma once
#include "ManagerBase.h"
#include <QFile>
#include <QDebug>
#include <string>
#include <list>
#include <memory>


class GameField;

class GameFieldManager : public ManagerBase
{
public:
	GameFieldManager();
	virtual ~GameFieldManager();

	std::list<std::string> getNames();
	std::list<std::shared_ptr<GameField>> getFields();

	bool exists(const std::string& name);

	std::shared_ptr<GameField> getFieldByName(const std::string& name);
	std::shared_ptr<GameField> getDefaultField();

	void convert(const std::string &units);

	static constexpr char* gameTag = "game";
	static constexpr char* imageTag = "field-image";
	static constexpr char* cornersTag = "field-corners";
	static constexpr char* topLeftCornerTag = "top-left";
	static constexpr char* bottomRightCornerTag = "bottom-right";
	static constexpr char* sizeTag = "field-size";
	static constexpr char* unitsTag = "field-unit"; 

protected:
	virtual bool checkCount() {
		if (fields_.size() < 0)
			qDebug() << "no fields found, cannot start application - must have at least one field defined";

		return fields_.size() > 0;
	}

private:

private:
	bool processJSONFile(QFile& file);

private:
	std::list<std::shared_ptr<GameField>> fields_;
};

