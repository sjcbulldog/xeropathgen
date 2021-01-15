//
// Copyright 2019 Jack W. Griffin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
//
#include "XeroPathGen.h"
#include "GameFieldManager.h"
#include "GeneratorManager.h"
#include "RobotManager.h"
#include <QtWidgets/QApplication>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDateTime>
#include <QDebug>
#include <QSplashScreen>
#include <fstream>
#include <sstream>
#include <iostream>
#include <build.h>

std::ofstream logfilestream;
std::stringstream log2stream;

void myMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	QByteArray localMsg = msg.toLocal8Bit();
	const char* file = context.file ? context.file : "";
	const char* function = context.function ? context.function : "";
	switch (type) {
	case QtDebugMsg:
		logfilestream << "Debug: ";
		log2stream << "Debug: ";
		break;
	case QtInfoMsg:
		logfilestream << "Info: ";
		log2stream << "Info: ";
		break;
	case QtWarningMsg:
		logfilestream << "Warning: ";
		log2stream << "Warning: ";
		break;
	case QtCriticalMsg:
		logfilestream << "Critical: ";
		log2stream << "Critical: ";
		break;
	case QtFatalMsg:
		logfilestream << "Error: ";
		log2stream << "Error: ";
		break;
	}
	logfilestream << localMsg.constData() << "(" << file << ":" << context.line << ", " << function << std::endl;
	log2stream << localMsg.constData() << std::endl;
}

static void pruneLogFiles(QDir &logdir)
{
	int count = 0;
	QStringList paths;
	QStringList list;
	list << "*.log";
	QFileInfoList files = logdir.entryInfoList(list, QDir::Filter::NoFilter, QDir::SortFlag::Time);
	while (files.size() > 20)
	{
		QFile file(files.back().absoluteFilePath());
		file.remove();
		files.pop_back();
		count++;
	}

	qDebug() << "Pruning old log files, " << count << "deleted";
}

static bool createLogFile()
{
	QStringList dirs = QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation);
	QString dir = dirs.front();
	if (dir.length() == 0)
		return false;

	dir += "/logs";
	QDir logdir(dir);
	if (!logdir.exists())
	{
		if (!QDir().mkpath(dir))
			return false;
	}

	QDateTime now = QDateTime::currentDateTime();
	QString log = dir + "/" + now.toString("dd_MM_yyyy_hh_mm_ss_zzz.'log'");
	logfilestream.open(log.toStdString());

	log = "Version";
	log += " " + QString::number(XERO_MAJOR_VERSION);
	log += "." + QString::number(XERO_MINOR_VERSION);
	log += "." + QString::number(XERO_MICRO_VERSION);
	log += "." + QString::number(XERO_BUILD_VERSION);
	logfilestream.open(log.toStdString());

	pruneLogFiles(logdir);

	return true;
}

int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("ErrorCodeXero");
	QCoreApplication::setOrganizationDomain("www.wilsonvillerobotics.com");
	QCoreApplication::setApplicationName("XeroPathGenerator");
	QCoreApplication::setApplicationVersion("1.0.0");

	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication a(argc, argv);

	QString exedir = QCoreApplication::applicationDirPath();
	QString imagepath = exedir + "/images/Splash.png";
	QPixmap image(imagepath);


	std::string error;

	QSplashScreen splash(image);
	QFont font = splash.font();
	font.setPointSizeF(16.0);
	splash.setFont(font);
	splash.show();
	splash.showMessage("Initializing ...");
	a.processEvents();

	if (createLogFile())
	{
		QDateTime now = QDateTime::currentDateTime();
		qInstallMessageHandler(myMessageOutput);
		qDebug() << "Starting Xero Path Generator, " << now.toString("dd/MM/yyyy @ hh:mm:ss");
	}

	QStringList dirs = QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation);
	QString appdir = dirs.front();

	GameFieldManager fields;
	GeneratorManager generators;
	RobotManager robots;

	argc--;
	argv++;

	while (argc-- > 0)
	{
		std::string arg = *argv++;
		if (arg == "--fields")
		{
			if (argc == 0)
			{
				QMessageBox box(QMessageBox::Icon::Critical,
					"Error", "--fields flag requires path argument", QMessageBox::StandardButton::Ok);
				box.exec();
				return -1;
			}

			fields.setDirectory(*argv++);
			argc--;
		}
		else if (arg == "--generators")
		{
			if (argc == 0)
			{
				QMessageBox box(QMessageBox::Icon::Critical,
					"Error", "--generators flag requires path argument", QMessageBox::StandardButton::Ok);
				box.exec();
				return -1;
			}

			generators.setDirectory(*argv++);
			argc--;
		}
		else if (arg == "--robots")
		{
			if (argc == 0)
			{
				QMessageBox box(QMessageBox::Icon::Critical,
					"Error", "--robots flag requires path argument", QMessageBox::StandardButton::Ok);
				box.exec();
				return -1;
			}

			robots.setDirectory(*argv++);
			argc--;
		}
	}

	fields.addDefaultDirectory((appdir + "/fields").toStdString());
	generators.addDefaultDirectory((appdir + "/generators").toStdString());
	robots.addDefaultDirectory((appdir + "/robots").toStdString());
	std::chrono::milliseconds delay(250);

#ifdef NOTYET
	QString field = exedir + "/../fields";
	QDir dir(field);
	fields.addDirectory(dir.absolutePath().toStdString());

	QString gen = exedir + "/../generators";
	dir = QDir(gen);
	generators.addDirectory(dir.absolutePath().toStdString());

#endif

	fields.copyDefaults("fields");
	generators.copyDefaults("generators");

	splash.showMessage("Initializing fields ...");
	a.processEvents();
	if (!fields.initialize())
	{
		QMessageBox box(QMessageBox::Icon::Critical, "Error", 
					"Could not initialize the game field manager", QMessageBox::StandardButton::Ok);
		box.exec();
		return -1;
	}
	std::this_thread::sleep_for(delay);

	splash.showMessage("Initializing generators ...");
	a.processEvents();
	if (!generators.initialize())
	{
		QMessageBox box(QMessageBox::Icon::Critical, "Error", 
					"Could not initialize the generator manager", QMessageBox::StandardButton::Ok);
		box.exec();
		return -1;
	}
	std::this_thread::sleep_for(delay);

	splash.showMessage("Initializing robots ...");
	a.processEvents();
	if (!robots.initialize())
	{
		QMessageBox box(QMessageBox::Icon::Critical, "Error",
			"Could not initialize the robots manager", QMessageBox::StandardButton::Ok);
		box.exec();
		return -1;
	}
	std::this_thread::sleep_for(delay);

	fields.dumpSearchPath("Fields");
	generators.dumpSearchPath("Generators");
	robots.dumpSearchPath("Robots");

	try {
		XeroPathGen w(fields, generators, robots, logfilestream, log2stream);
		w.show();
		splash.finish(&w);
		return a.exec();
	}
	catch(const std::exception &ex)
	{
		qDebug() << "Exception Caught: " << ex.what();
		return -1;
	}

	return 0;
}
