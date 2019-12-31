#pragma once

#include <QtWidgets/QMainWindow>
#include "GameFieldManager.h"
#include "PathFieldView.h"
#include "SubsystemData.h"
#include <QSplitter>
#include <QTextEdit>
#include <QMenu>
#include <QSettings>
#include <QProcess>
#include <QTimer>
#include <QTreeWidget>
#include <memory>
#include <thread>
#include <map>
#include <mutex>


class XeroSimDisplay : public QMainWindow
{
	Q_OBJECT

public:
	XeroSimDisplay(GameFieldManager &field, QWidget *parent = Q_NULLPTR);
	virtual ~XeroSimDisplay();

protected:
	void setField(const std::string& field);
	void closeEvent(QCloseEvent* ev);

private:
	// Menu delegates
	void runConfig();
	void runRun();

private:
	void createWindows();
	void createMenus();

	void runSimulation();
	void readStdOut();
	void readStdErr();
	void procStarted();
	void procFinished(int exitcode, QProcess::ExitStatus st);
	void errorOccurred(QProcess::ProcessError error);

	void unhookProc();

	void timerProc();
	void processSimulatorLine(const std::string& line);

	bool extractNameValue(const std::string& line, std::string& name, std::string& value);
	std::shared_ptr<SubsystemData> getSubsystemData(const std::string& subname);
	void updateAllSubsystems();
	void createItem(std::shared_ptr<SubsystemData> data, const std::string& item);

	void updateRobot();

private:
	GameFieldManager& fields_;
	QSettings settings_;

	double robot_x_;
	double robot_y_;
	double robot_angle_;
	QString robot_text_;

	QSplitter* top_bottom_spliter_;
	QSplitter* left_right_splitter_;
	PathFieldView* field_view_;
	QTextEdit* logger_;
	QTreeWidget* subsystems_;

	QString src_dir_;
	QString robot_;
	QString stimulus_;
	double auto_;
	double oper_;
	double test_;
	double start_;

	QMenu* run_;

	QProcess* sim_proc_;

	std::string stdout_;
	std::string stderr_;

	QTimer *timer_;

	QMetaObject::Connection rdstdout;
	QMetaObject::Connection rdstderr;
	QMetaObject::Connection procstarted;
	QMetaObject::Connection procfinished;
	QMetaObject::Connection erroroccurred;

	std::map<std::string, std::shared_ptr<SubsystemData>> subsystem_data_;

	std::mutex data_lock_;

	std::chrono::high_resolution_clock::time_point last_robot_update_;
	std::chrono::high_resolution_clock::time_point last_subsystem_update_;

	bool sim_start_failed_;
};

