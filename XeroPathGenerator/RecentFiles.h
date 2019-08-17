#pragma once

#include <QSettings>
#include <QMenu>
#include <list>
#include <string>

class XeroPathGenerator;

class RecentFiles : public QObject
{
	Q_OBJECT

public:
	RecentFiles(QSettings &settings, QMenu &menu);
	virtual ~RecentFiles();

	void initialize(XeroPathGenerator* app);

	void setCount(size_t count);
	void addRecentFile(XeroPathGenerator *app, QString file, bool end = false);
	void removeRecentFile(XeroPathGenerator* app, QString file);

private:
	void adjustSize();
	void writeRecentFiles();

private:
	size_t count_;
	std::list<QString> files_;
	QSettings &settings_;
	QMenu& menu_;
};

