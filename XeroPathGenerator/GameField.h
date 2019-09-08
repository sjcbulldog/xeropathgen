#pragma once
#include <UnitConverter.h>
#include <Translation2d.h>
#include <string>
#include <QDir>
#include <QFile>
#include <QFileInfo>

class GameField
{
public:
	GameField(const QDir& path, const std::string& name, const std::string& image, const std::string &units,
		xero::paths::Translation2d topleft, xero::paths::Translation2d bottomright,
		xero::paths::Translation2d size) {
		path_ = path;
		name_ = name;
		image_ = image;
		units_ = units;
		top_left_ = topleft;
		bottom_right_ = bottomright;
		size_ = size;
	}

	~GameField() {
	}

	const std::string& getName() const {
		return name_;
	}

	QString getImageFile() {
		QFileInfo info(path_, image_.c_str());
		return info.absoluteFilePath();
	}

	xero::paths::Translation2d getTopLeft() const {
		return top_left_;
	}

	xero::paths::Translation2d getBottomRight() const {
		return bottom_right_;
	}

	xero::paths::Translation2d getSize() const {
		return size_;
	}

	void convert(const std::string& units) {
		if (units != units_)
		{
			double x = xero::paths::UnitConverter::convert(size_.getX(), units_, units);
			double y = xero::paths::UnitConverter::convert(size_.getY(), units_, units);

			size_ = xero::paths::Translation2d(x, y);

			units_ = units;
		}
	}

public:


private:
	std::string name_;
	std::string image_;
	std::string units_;
	xero::paths::Translation2d top_left_;
	xero::paths::Translation2d bottom_right_;
	xero::paths::Translation2d size_;
	QDir path_;
};

