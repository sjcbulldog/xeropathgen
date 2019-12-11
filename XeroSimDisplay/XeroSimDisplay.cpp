#include "XeroSimDisplay.h"
#include "ConfigDialog.h"
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QProcess>
#include <QMessageBox>
#include <QTextCodec>
#include <QProcessEnvironment>

XeroSimDisplay::XeroSimDisplay(GameFieldManager &fields, QWidget *parent) : QMainWindow(parent), fields_(fields)
{
	fields_.initialize();

	createWindows();
	createMenus();

	std::string name = "Empty Field";
	if (settings_.contains("FIELD"))
	{
		QString field = settings_.value("FIELD").toString();
		name = field.toStdString();
	}
	setField(name);
	field_view_->setUnits("in");

	if (settings_.contains("SRCDIR"))
		src_dir_ = settings_.value("SRCDIR").toString();

	if (settings_.contains("ROBOT"))
		robot_ = settings_.value("ROBOT").toString();

	if (settings_.contains("STIMULUS"))
		stimulus_ = settings_.value("STIMULUS").toString();

	if (settings_.contains("AUTO"))
		auto_ = settings_.value("AUTO").toDouble();
	else
		auto_ = 0.0;

	if (settings_.contains("OPER"))
		oper_ = settings_.value("OPER").toDouble();
	else
		oper_ = 0.0;

	if (settings_.contains("TEST"))
		test_ = settings_.value("TEST").toDouble();
	else
		test_ = 0.0;

	start_ = 0.0;

	timer_ = new QTimer(this);
	(void)connect(timer_, &QTimer::timeout, this, &XeroSimDisplay::timerProc);
	timer_->start(1);

	sim_proc_ = nullptr;

	last_robot_update_ = std::chrono::high_resolution_clock::now();
	last_subsystem_update_ = std::chrono::high_resolution_clock::now();
}

XeroSimDisplay::~XeroSimDisplay()
{
}

void XeroSimDisplay::closeEvent(QCloseEvent* ev)
{
	settings_.setValue("GEOMETERY", saveGeometry());
	settings_.setValue("STATE", saveState());
	QMainWindow::closeEvent(ev);
}

void XeroSimDisplay::setField(const std::string& name)
{
	//
	// Set the current field
	//
	auto field = fields_.getFieldByName(name);
	field->convert("in");
	field_view_->setField(field);
}

void XeroSimDisplay::createWindows()
{
	left_right_splitter_ = new QSplitter();
	left_right_splitter_->setOrientation(Qt::Orientation::Horizontal);
	setCentralWidget(left_right_splitter_);

	top_bottom_spliter_ = new QSplitter();
	top_bottom_spliter_->setOrientation(Qt::Orientation::Vertical);

	field_view_ = new PathFieldView(top_bottom_spliter_);
	top_bottom_spliter_->addWidget(field_view_);

	logger_ = new QTextEdit(top_bottom_spliter_);
	logger_->setReadOnly(true);
	top_bottom_spliter_->addWidget(logger_);

	left_right_splitter_->addWidget(top_bottom_spliter_);
	subsystems_ = new QTreeWidget();
	subsystems_->setColumnCount(2);
	QStringList header;
	header.push_back("Param");
	header.push_back("Value");
	subsystems_->setHeaderLabels(header);
	left_right_splitter_->addWidget(subsystems_);

	if (settings_.contains("GEOMETRY"))
		restoreGeometry(settings_.value("GEOMETRY").toByteArray());

	if (settings_.contains("STATE"))
		restoreState(settings_.value("STATE").toByteArray());
}

void XeroSimDisplay::createMenus()
{
	QAction *action;

	run_ = new QMenu(tr("&Run"));
	menuBar()->addMenu(run_);
	action = run_->addAction(tr("Configure ..."));
	(void)connect(action, &QAction::triggered, this, &XeroSimDisplay::runConfig);
	action = run_->addAction(tr("Run"));
	(void)connect(action, &QAction::triggered, this, &XeroSimDisplay::runRun);
}

void XeroSimDisplay::runConfig()
{
	ConfigDialog dialog(src_dir_, robot_, stimulus_, auto_, oper_, test_);

	if (dialog.exec() == QDialog::Accepted)
	{
		src_dir_ = dialog.getSrcDir();
		robot_ = dialog.getRobot();
		stimulus_ = dialog.getStimulusFile();
		auto_ = dialog.getAuto();
		oper_ = dialog.getOper();
		test_ = dialog.getTest();

		settings_.setValue("SRCDIR", src_dir_);
		settings_.setValue("ROBOT", robot_);
		settings_.setValue("STIMULUS", stimulus_);
		settings_.setValue("AUTO", auto_);
		settings_.setValue("OPER", oper_);
		settings_.setValue("TEST", test_);
	}
}

void XeroSimDisplay::runRun()
{
	if (sim_proc_ != nullptr)
	{
		QMessageBox box(QMessageBox::Icon::Critical,
			"Error", "A simulation is ready running", QMessageBox::StandardButton::Ok);
		box.exec();
		return;
	}

	QString stim = "robots/" + robot_ + "/sims/" + stimulus_;
	QString sim = "makebuild/SIMULATOR/Debug/" + robot_ + "/" + robot_ + ".exe";

	if (!QFile::exists(src_dir_ + "/" + stim))
	{
		QString msg = "The stimulus file '" + stim + "' does not exist";
		QMessageBox box(QMessageBox::Icon::Critical,
			"Error", msg, QMessageBox::StandardButton::Ok);
		box.exec();
		return;
	}

	if (!QFile::exists(src_dir_ + "/" + sim))
	{
		QString msg = "The simlator file '" + sim + "' does not exist (do you need to build it?)";
		QMessageBox box(QMessageBox::Icon::Critical,
			"Error", msg, QMessageBox::StandardButton::Ok);
		box.exec();
		return;
	}

	stdout_.reserve(20 * 1024 * 1024);
	runSimulation();
}

void XeroSimDisplay::runSimulation()
{
	QStringList args;
	bool looping = true;

	QString stim = "robots/" + robot_ + "/sims/" + stimulus_;
	QString sim = "makebuild/SIMULATOR/Debug/" + robot_ + "/" + robot_ + ".exe";

	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	QString str = env.value("PATH", "");
	str = "C:\\cygwin64\\bin;" + str;
	env.insert("PATH", str);

	args.push_back("--simgraph");
	args.push_back("--auto");
	args.push_back(QString::number(auto_));
	args.push_back("--oper");
	args.push_back(QString::number(oper_));
	args.push_back("--test");
	args.push_back(QString::number(test_));
	args.push_back("--start");
	args.push_back("1.0");
	args.push_back("--events");
	args.push_back("robots/" + robot_ + "/sims/" + stimulus_);

	{
		std::lock_guard<std::mutex> guard(data_lock_);
		stdout_ += "Running: ";
		for (const QString& arg : args)
			stdout_ += std::string(" ") + arg.toStdString();
		stdout_ += "\n";
	}

	sim_start_failed_ = false;

	sim_proc_ = new QProcess();
	sim_proc_->setWorkingDirectory("C:/cygwin64/home/ButchGriffin/rtools/master");			// TODO - add this to setup some how
	sim_proc_->setProcessEnvironment(env);

	rdstdout = connect(sim_proc_, &QProcess::readyReadStandardOutput, this, &XeroSimDisplay::readStdOut);
	rdstderr = connect(sim_proc_, &QProcess::readyReadStandardError, this, &XeroSimDisplay::readStdErr);
	procstarted = connect(sim_proc_, &QProcess::started, this, &XeroSimDisplay::procStarted);
	erroroccurred = connect(sim_proc_, &QProcess::errorOccurred, this, &XeroSimDisplay::errorOccurred);
	procfinished = connect(sim_proc_, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &XeroSimDisplay::procFinished);

	sim_proc_->start(src_dir_ + "/" + sim, args);

	if (sim_start_failed_)
		unhookProc();
}

void XeroSimDisplay::unhookProc()
{
	disconnect(rdstdout);
	disconnect(rdstderr);
	disconnect(procstarted);
	disconnect(erroroccurred);
	disconnect(procfinished);
	delete sim_proc_;
	sim_proc_ = nullptr;
}

void XeroSimDisplay::errorOccurred(QProcess::ProcessError error)
{
	qDebug() << "Error";

	if (error == QProcess::ProcessError::FailedToStart)
	{
		sim_start_failed_ = true;
	}
}

void XeroSimDisplay::procStarted()
{
	qDebug() << "Started";
}

void XeroSimDisplay::procFinished(int exitcode, QProcess::ExitStatus st)
{
	unhookProc();
	delete sim_proc_;
	sim_proc_ = nullptr;
}

void XeroSimDisplay::readStdOut()
{
	if (sim_proc_ != nullptr)
	{
		auto data = sim_proc_->readAllStandardOutput();
		if (data.size() > 0)
		{
			QString str = QTextCodec::codecForMib(106)->toUnicode(data);
			{
				std::lock_guard<std::mutex> guard(data_lock_);
				stdout_ += str.toStdString();
			}
		}
	}
}

void XeroSimDisplay::readStdErr()
{
	if (sim_proc_ != nullptr)
	{
		auto data = sim_proc_->readAllStandardError();
		if (data.size() > 0)
		{
			QString str = QTextCodec::codecForMib(106)->toUnicode(data);
			{
				std::lock_guard<std::mutex> guard(data_lock_);
				stderr_ += str.toStdString();
			}
		}
	}
}

void XeroSimDisplay::timerProc()
{
	int pos;
	int loopcnt = 0;
	int lines;

	std::lock_guard<std::mutex> guard(data_lock_);

	while (stdout_.length() > 0 && (pos = stdout_.find_first_of('\n')) != std::string::npos)
	{
		std::string line = stdout_.substr(0, pos);
		stdout_ = stdout_.substr(pos + 1);

		if ((pos = line.find_first_of("%%%%")) != std::string::npos)
		{
			processSimulatorLine(line.substr(pos + 4));
		}
		else
		{
			logger_->append(line.c_str());
		}

		if (loopcnt++ == 100)
		{
			//
			// We only process so much data before we retur, so that the GUI still functions.
			//
			break;
		}
	}

	auto dur = std::chrono::high_resolution_clock::now() - last_robot_update_;
	auto durms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
	if (durms.count() > 50)
	{
		field_view_->setRobotPosition(robot_x_, robot_y_, robot_angle_);
		last_robot_update_ = std::chrono::high_resolution_clock::now();
	}

	dur = std::chrono::high_resolution_clock::now() - last_subsystem_update_;
	durms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
	if (durms.count() > 500)
	{
		//
		// Every 250 ms we update the value in the tree.  Updating the actual value seems to be
		// expensive, so we only do it occasionally
		//
		updateAllSubsystems();
		last_subsystem_update_ = std::chrono::high_resolution_clock::now();
	}
}

bool XeroSimDisplay::extractNameValue(const std::string& line, std::string& name, std::string& value)
{
	int index = 0;

	while (index < line.length() && std::isspace(line[index]))
		index++;

	while (index < line.length() && line[index] != ':')
		name += line[index++];

	if (index == line.length())
		return false;

	index++;

	while (index < line.length() && std::isspace(line[index]))
		index++;

	while (index < line.length() && !std::isspace(line[index]))
		value += line[index++];

	return true;
}

std::shared_ptr<SubsystemData> XeroSimDisplay::getSubsystemData(const std::string& subname)
{
	auto it = subsystem_data_.find(subname);
	if (it == subsystem_data_.end())
	{
		std::shared_ptr<SubsystemData> sub = std::make_shared<SubsystemData>(subname);
		subsystem_data_[subname] = sub;
		return sub;
	}

	return it->second;
}

void XeroSimDisplay::processSimulatorLine(const std::string& line)
{
	int comma = line.find_first_of(',');
	if (comma == std::string::npos)
		return;

	std::string subname = line.substr(0, comma);
	std::string remaining = line.substr(comma + 1);
	std::string name, value;
	if (extractNameValue(remaining, name, value))
	{
		std::shared_ptr<SubsystemData> data = getSubsystemData(subname);
		if (data->addData(name, value))
		{
			createItem(data, name);
		}

		if (subname == "tankdrive")
		{
			if (name == "X")
				robot_x_ = QString(value.c_str()).toDouble();
			else if (name == "Y")
				robot_y_ = QString(value.c_str()).toDouble();
			else if (name == "Angle")
				robot_angle_ = QString(value.c_str()).toDouble();
		}
	}
}

void XeroSimDisplay::updateAllSubsystems()
{
	for (int i = 0; i < subsystems_->topLevelItemCount(); i++)
	{
		QTreeWidgetItem* subitem = subsystems_->topLevelItem(i);
		auto subdata = getSubsystemData(subitem->text(0).toStdString());

		for (int i = 0; i < subitem->childCount(); i++)
		{
			QTreeWidgetItem* vitem = subitem->child(i);
			std::string value = subdata->getValue(vitem->text(0).toStdString());
			vitem->setText(1, value.c_str());
		}
	}
}

void XeroSimDisplay::createItem(std::shared_ptr<SubsystemData> data, const std::string& name)
{
	QTreeWidgetItem* subitem = nullptr;;

	for (int i = 0; i < subsystems_->topLevelItemCount(); i++)
	{
		auto item = subsystems_->topLevelItem(i);
		if (item->text(0).toStdString() == data->getName())
		{
			subitem = item;
			break;
		}
	}

	if (subitem == nullptr)
	{
		subitem = new QTreeWidgetItem(subsystems_);
		subitem->setText(0, data->getName().c_str());
		subsystems_->addTopLevelItem(subitem);
	}

	QString keystr(name.c_str());
	QTreeWidgetItem* vitem = nullptr;
	for (int i = 0; i < subitem->childCount(); i++)
	{
		auto item = subitem->child(i);
		if (item->text(0) == keystr)
		{
			vitem = item;
			break;
		}
	}

	if (vitem == nullptr)
	{
		vitem = new QTreeWidgetItem(subitem);
		vitem->setText(0, keystr);
		subitem->addChild(vitem);
	}
}