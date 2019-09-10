#pragma once

#include <QMainWindow>
#include <QHelpEngine>
#include <QTabWidget>
#include <QSplitter>
#include <QTextBrowser>

class HelpDisplay : public QSplitter
{
	Q_OBJECT

public:
	HelpDisplay(QWidget *parent = Q_NULLPTR);
	~HelpDisplay();

private:
	void gotoLink(const QUrl &url);
	void gotoLinkWithKeyword(const QUrl& url, const QString& keyword);

private:
	QHelpEngine* help_;
	QTabWidget* tabs_;
	QTextBrowser* browser_;
};
