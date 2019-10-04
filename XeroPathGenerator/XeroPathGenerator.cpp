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
#include "XeroPathGenerator.h"
#include "XeroPathGenerator.h"
#include "PathFileTreeModel.h"
#include "GameField.h"
#include "Generator.h"
#include "PathCollectionIO.h"
#include "PropertyEditor.h"
#include "TankDriveModifier.h"
#include "SwerveDriveModifier.h"
#include "SelectRobotDialog.h"
#include "PlotVariableSelector.h"
#include "AboutDialog.h"
#include "WaypointChangedUndo.h"
#include "PrintController.h"

#include <DriveBaseData.h>
#include <TrajectoryNames.h>
#include <JSONWriter.h>
#include <PathGroup.h>
#include <CSVWriter.h>
#include <RobotPath.h>
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QTimer>
#include <QDebug>
#include <QLineSeries>
#include <QToolBar>
#include <QCheckBox>
#include <QDesktopServices>
#include <QUrl>
#include <QLatin1String>
#include <QTextBrowser>
#include <QProcess>
#include <QPrinter>
#include <QPrintDialog>

#include <cstdio>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4100 4458)
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#endif

#include <networktables/NetworkTableInstance.h>

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifdef _MSVC
#pragma warning(pop)
#endif

using namespace xero::paths;
using namespace QtCharts;

const char* XeroPathGenerator::RobotDialogName = "Name";
const char* XeroPathGenerator::RobotDialogEWidth = "Effective Width";
const char* XeroPathGenerator::RobotDialogELength = "Effective Length";
const char* XeroPathGenerator::RobotDialogRWidth = "Physical Width";
const char* XeroPathGenerator::RobotDialogRLength = "Physical Length";
const char* XeroPathGenerator::RobotDialogMaxVelocity = "Max Velocity";
const char* XeroPathGenerator::RobotDialogMaxAcceleration = "Max Acceleration";
const char* XeroPathGenerator::RobotDialogMaxJerk = "Max Jerk";
const char* XeroPathGenerator::RobotDialogUnits = "Units";
const char* XeroPathGenerator::RobotDialogDriveType = "Drive Type";
const char* XeroPathGenerator::RobotDialogTimeStep = "Time Step";

const char* XeroPathGenerator::PrefDialogUnits = "Units";
const char* XeroPathGenerator::PrefDialogOutputFormat = "Output Format";
const char* XeroPathGenerator::PrefDialogNTServer = "NT Server Addr";
const char* XeroPathGenerator::PrefDialogNTTableName = "NT Table Name";

XeroPathGenerator* XeroPathGenerator::theOne = nullptr;

XeroPathGenerator::XeroPathGenerator(GameFieldManager& fields, GeneratorManager& generators, RobotManager &robots, 
										std::ofstream& ostrm, std::stringstream& sstrm, QWidget* parent)
	: QMainWindow(parent), fields_mgr_(fields), generators_mgr_(generators), robot_mgr_(robots), 
	  path_param_model_(paths_model_), logstream_(ostrm), strstream_(sstrm), download_mgr_(fields, generators)
{
	assert(theOne == nullptr);
	theOne = this;
	write_messages_ = false;

	traj_window_ = nullptr;

	QString exedir = QCoreApplication::applicationDirPath();
	QString imagepath = exedir + "/images/icon.png";
	QPixmap image(imagepath);
	QIcon icon(image);
	setWindowIcon(icon);

	resize(1024, 768);
	gui_thread_ = std::this_thread::get_id();

	if (!createWindows())
	{
		QMessageBox box(QMessageBox::Icon::Critical,
			"Error", "Could not initialize application windows", QMessageBox::StandardButton::Ok);
		box.exec();
		throw new std::runtime_error("Error - startup aborted");
		return;
	}

	if (!createMenus())
	{
		QMessageBox box(QMessageBox::Icon::Critical,
			"Error", "Could not initialize application menus", QMessageBox::StandardButton::Ok);
		box.exec();
		throw new std::runtime_error("Error - startup aborted");
		return;
	}

	if (!createToolbar())
	{
		QMessageBox box(QMessageBox::Icon::Critical,
			"Error", "Could not initialize application toolbar", QMessageBox::StandardButton::Ok);
		box.exec();
		throw new std::runtime_error("Error - startup aborted");
		return;
	}

	xpos_text_ = new QLabel("");
	xpos_text_->setFixedWidth(100);
	statusBar()->insertWidget(0, xpos_text_);

	ypos_text_ = new QLabel("");
	ypos_text_->setFixedWidth(100);
	statusBar()->insertWidget(1, ypos_text_);

	status_text_ = new QLabel("");
	status_text_->setFixedWidth(200);
	statusBar()->insertWidget(2, status_text_);

	time_dist_ = new QLabel("");
	time_dist_->setFixedWidth(200);
	statusBar()->insertWidget(3, time_dist_);

	prog_bar_ = new QProgressBar();
	statusBar()->insertWidget(4, prog_bar_);
	prog_bar_->setVisible(false);

	populateFieldsMenu();
	populateGeneratorsMenu();
	populateRobotsMenu();

	setDefaultField();
	setDefaultGenerator();
	setDefaultRobot();
	setPath(nullptr);

	if (settings_.contains(GeometrySetting))
		restoreGeometry(settings_.value(GeometrySetting).toByteArray());

	if (settings_.contains(WindowStateSetting))
		restoreState(settings_.value(WindowStateSetting).toByteArray());

	if (settings_.contains(LeftRightSplitterSetting))
	{
		QList<QVariant> stored = settings_.value(LeftRightSplitterSetting).toList();
		QList<int> sizes;
		for (const QVariant& v : stored)
			sizes.push_back(v.toInt());
		m_left_right_->setSizes(sizes);
	}

	if (settings_.contains(LeftSplitterSetting))
	{
		QList<QVariant> stored = settings_.value(LeftSplitterSetting).toList();
		QList<int> sizes;
		for (const QVariant& v : stored)
			sizes.push_back(v.toInt());
		m_left_top_bottom_->setSizes(sizes);
	}

	if (path_view_->height() < 20)
	{
		QList<int> sizes;
		sizes << m_left_top_bottom_->height() / 2;
		sizes << m_left_top_bottom_->height() / 2;
		m_left_top_bottom_->setSizes(sizes);
	}

	if (settings_.contains(RightSplitterSetting))
	{
		QList<QVariant> stored = settings_.value(RightSplitterSetting).toList();
		QList<int> sizes;
		for (const QVariant& v : stored)
			sizes.push_back(v.toInt());
		m_right_stack_->setSizes(sizes);
	}

	if (settings_.contains(UnitsSetting))
		units_ = settings_.value(UnitsSetting).toString().toStdString();
	else
	{
		units_ = "in";
		settings_.setValue(UnitsSetting, units_.c_str());
	}
	setUnits(units_);

	if (settings_.contains(LastPathDirSetting))
	{
		last_path_dir_ = settings_.value(LastPathDirSetting).toString().toStdString();
	}

	if (settings_.contains(OutputTypeSetting))
	{
		QString val = settings_.value(OutputTypeSetting).toString();
		if (val == JsonOutputType)
			output_type_ = OutputType::OutputJSON;
		else
			output_type_ = OutputType::OutputCSV;
	}

	if (settings_.contains(NTServerIPAddress))
	{
		ntserver_ = settings_.value(NTServerIPAddress).toString().toStdString();
	}

	if (ntserver_.length() == 0)
	{
		ntserver_ = "127.0.0.1";
	}

	if (settings_.contains(NTServerTableName))
	{
		nttable_name_ = settings_.value(NTServerTableName).toString().toStdString();
	}
	else
	{
		nttable_name_ = "XeroPaths";
	}

	if (settings_.contains(GridEnabled))
	{
		bool b = settings_.value(GridEnabled).toBool();
		path_view_->enableGrid(b);
	}

	if (settings_.contains(GridComplete))
	{
		bool b = settings_.value(GridComplete).toBool();
		path_view_->completeGrid(b);
	}

	if (settings_.contains(ShowEquation))
	{
		bool b = settings_.value(ShowEquation).toBool();
		path_view_->showEquation(b);
	}

	if (settings_.contains(EquationsStacked))
	{
		bool b = settings_.value(EquationsStacked).toBool();
		path_view_->equationsStacked(b);
	}

	if (settings_.contains(DownloadSite) && settings_.contains(DownloadLocation))
	{
		download_mgr_.setSite(settings_.value(DownloadSite).toString());
		download_mgr_.setLocation(settings_.value(DownloadLocation).toString());
	}
	else
	{
		download_mgr_.setSite(DefaultDownloadSite);
		download_mgr_.setLocation(DefaultDownloadLocation);
	}

	auto inst = nt::NetworkTableInstance::GetDefault();
	inst.StartClient(ntserver_.c_str());

	(void)connect(path_view_, &PathFieldView::mouseMoved, this, &XeroPathGenerator::pathWindowMouseMoved);

	path_timer_ = new QTimer(this);
	(void)connect(path_timer_, &QTimer::timeout, this, &XeroPathGenerator::timerProc);

	path_timer_->start(timerTickMS);

	initRecentFiles();

	demo_mode_ = DemoMode::ModeNone;
	robot_window_ = nullptr;
	traj_window_ = nullptr;

	(void)connect(&paths_model_, &PathFileTreeModel::pathAdded, this, &XeroPathGenerator::pathAdded);
	(void)connect(&paths_model_, &PathFileTreeModel::pathDeleted, this, &XeroPathGenerator::pathDeleted);
	(void)connect(&paths_model_, &PathFileTreeModel::groupAdded, this, &XeroPathGenerator::groupAdded);
	(void)connect(&paths_model_, &PathFileTreeModel::groupDeleted, this, &XeroPathGenerator::groupDeleted);
}

XeroPathGenerator::~XeroPathGenerator()
{
	qInstallMessageHandler(0);
	assert(theOne == this);
	theOne = nullptr;

}

//////////////////////////////////////////////////////////////////////////////////////////
//
// This section of code is all about building up the UI.  The interactions of the various
// splitters, windows, panes etc. was complex enough I chose to build this via code versus
// using their UI builder.  This was my first QT application so maybe as I get more familiar
// with their tools I would have done it differently.
//
/////////////////////////////////////////////////////////////////////////////////////////

bool XeroPathGenerator::createWindows()
{
	//
	// Top most splitter window, divides the client window area left to 
	// right between the graph/plot windows and the data stacks on the right
	//
	m_left_right_ = new QSplitter();
	setCentralWidget(m_left_right_);

	//
	// Left side of the screen splitter that splits between the top that contains
	// path drawing, and the bottom that contains graphs and logs
	//
	m_left_top_bottom_ = new QSplitter(m_left_right_);
	m_left_top_bottom_->setOrientation(Qt::Orientation::Vertical);
	if (!createLeftSide())
		return false;

	//
	// Right side of the screen that contains stacks of list views for editing data
	//
	m_right_stack_ = new QSplitter(m_left_right_);
	m_right_stack_->setOrientation(Qt::Orientation::Vertical);
	if (!createRightSide())
		return false;

	return true;
}

bool XeroPathGenerator::createLeftSide()
{
	path_view_ = new PathFieldView(paths_model_, m_left_top_bottom_);
	path_view_->setVisible(true);
	(void)connect(path_view_, &PathFieldView::waypointStartMoving, this, &XeroPathGenerator::startMovingWaypointProc);
	(void)connect(path_view_, &PathFieldView::waypointEndMoving, this, &XeroPathGenerator::endMovingWaypointProc);
	(void)connect(path_view_, &PathFieldView::waypointMoving, this, &XeroPathGenerator::movingWaypointProc);
	(void)connect(path_view_, &PathFieldView::waypointInserted, this, &XeroPathGenerator::insertedWaypointProc);
	(void)connect(path_view_, &PathFieldView::waypointDeleted, this, &XeroPathGenerator::deletedWaypointProc);
	(void)connect(path_view_, &PathFieldView::waypointSelected, this, &XeroPathGenerator::selectedWaypointProc);
	plots_log_tab_ = new QTabWidget(m_left_top_bottom_);

	plot_main_ = new PlotMainWindow(this);
	plots_ = plot_main_->plotview();
	(void)connect(plot_main_->bar(), &QAbstractSlider::valueChanged, this, &XeroPathGenerator::scrollBarChanged);
	log_ = new QTextEdit();

	plots_log_tab_->insertTab(0, plot_main_, tr("Trajectory"));
	plots_log_tab_->insertTab(1, log_, tr("Log Messages"));

	log_->setText("Welcome to Error Code Xero, path generator");

	return true;
}

bool XeroPathGenerator::createRightSide()
{
	QHBoxLayout* layout;

	paths_group_ = new QGroupBox(tr("Paths"), m_right_stack_);
	layout = new QHBoxLayout();
	paths_ = new PathFileTree();
	layout->addWidget(paths_);
	paths_group_->setLayout(layout);
	paths_->setHeaderHidden(true);
	paths_->setModel(&paths_model_);
	paths_->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	paths_->setEditTriggers(QAbstractItemView::EditTrigger::DoubleClicked);
	(void)connect(paths_->selectionModel(), &QItemSelectionModel::currentChanged, this, &XeroPathGenerator::pathTreeSelectionChanged);

	path_parameters_group_ = new QGroupBox(tr("Path Parameters"), m_right_stack_);
	layout = new QHBoxLayout();
	path_parameters_ = new QTreeView();
	path_parameters_->setModel(&path_param_model_);
	layout->addWidget(path_parameters_);
	path_parameters_group_->setLayout(layout);
	(void)connect(&path_param_model_, &PathParamTreeModel::dataChanged, this, &XeroPathGenerator::pathParamTreeDataChanged);

	QGroupBox *gr = new QGroupBox(tr("Path Constraints"), m_right_stack_);
	layout = new QHBoxLayout();
	constraints_ = new ConstraintEditor(this);
	(void)connect(constraints_, &ConstraintEditor::constraintAdded, this, &XeroPathGenerator::constraintAddedRemoved);
	(void)connect(constraints_, &ConstraintEditor::constraintRemoved, this, &XeroPathGenerator::constraintAddedRemoved);
	(void)connect(constraints_, &ConstraintEditor::constraintChanged, this, &XeroPathGenerator::constraintAddedRemoved);
	layout->addWidget(constraints_);
	gr->setLayout(layout);

	waypoint_parameters_group_ = new QGroupBox(tr("Waypoint Parameters"), m_right_stack_);
	layout = new QHBoxLayout();
	waypoint_parameters_ = new QTreeView();
	waypoint_parameters_->setModel(&waypoint_model_);
	(void)connect(&waypoint_model_, &WaypointTreeModel::dataChanged, this, &XeroPathGenerator::waypointTreeDataChanged);

	layout->addWidget(waypoint_parameters_);
	waypoint_parameters_group_->setLayout(layout);

	return true;
}

bool XeroPathGenerator::createMenus()
{
	QAction* action;

	//
	// File menu
	//
	file_ = new QMenu(tr("&File"));
	(void)connect(file_, &QMenu::aboutToShow, this, &XeroPathGenerator::showEditMenu);
	menuBar()->addMenu(file_);
	action = file_->addAction(tr("New"));
	(void)connect(action, &QAction::triggered, this, &XeroPathGenerator::fileNew);
	action = file_->addAction(tr("Open ..."));
	(void)connect(action, &QAction::triggered, this, &XeroPathGenerator::fileOpen);
	file_->addSeparator();
	file_save_ = file_->addAction(tr("Save"));
	(void)connect(file_save_, &QAction::triggered, this, &XeroPathGenerator::fileSave);
	action = file_->addAction(tr("Save As ..."));
	(void)connect(action, &QAction::triggered, this, &XeroPathGenerator::fileSaveAs);
	file_->addSeparator();
	action = file_->addAction(tr("Close"));
	(void)connect(action, &QAction::triggered, this, &XeroPathGenerator::fileClose);
	file_->addSeparator();
	action = file_->addAction(tr("Generate Paths As ..."));
	(void)connect(action, &QAction::triggered, this, &XeroPathGenerator::fileGenerateAs);
	action = file_->addAction(tr("Generate Paths"));
	(void)connect(action, &QAction::triggered, this, &XeroPathGenerator::fileGenerate);
	file_->addSeparator();
	file_publish_ = file_->addAction(tr("Publish path"));
	(void)connect(file_publish_, &QAction::triggered, this, &XeroPathGenerator::filePublish);
	file_->addSeparator();
	action = file_->addAction(tr("Print"));
	(void)connect(action, &QAction::triggered, this, &XeroPathGenerator::filePrint);
	file_->addSeparator();
	recent_menu_ = file_->addMenu("Recent Files");

	//
	// Edit menu
	//
	edit_ = new QMenu(tr("&Edit"));
	(void)connect(edit_, &QMenu::aboutToShow, this, &XeroPathGenerator::showEditMenu);

	insert_waypoint_action_ = edit_->addAction(tr("Insert Waypoint"));
	(void)connect(insert_waypoint_action_, &QAction::triggered, this, &XeroPathGenerator::insertWaypoint);

	delete_waypoint_action_ = edit_->addAction(tr("Delete Waypoint"));
	(void)connect(delete_waypoint_action_, &QAction::triggered, this, &XeroPathGenerator::deleteWaypoint);
	edit_->addSeparator();

	delete_path_or_group_action_ = edit_->addAction(tr("Delete"));
	(void)connect(delete_path_or_group_action_, &QAction::triggered, this, &XeroPathGenerator::deletePathOrGroup);
	edit_->addSeparator();

	add_group_action_ = edit_->addAction(tr("Add Path Group"));
	(void)connect(add_group_action_, &QAction::triggered, this, &XeroPathGenerator::addPathGroupAction);

	add_path_action_ = edit_->addAction(tr("Add Path"));
	(void)connect(add_path_action_, &QAction::triggered, this, &XeroPathGenerator::addRobotPathAction);

	edit_->addSeparator();
	action = edit_->addAction(tr("Preferences ..."));
	(void)connect(action, &QAction::triggered, this, &XeroPathGenerator::editPreferences);

	edit_->addSeparator();
	undo_action_ = edit_->addAction(tr("Undo"));
	(void)connect(undo_action_, &QAction::triggered, &undo_mgr_, &UndoManager::undo);

	menuBar()->addMenu(edit_);

	//
	// View menu
	//
	view_ = new QMenu(tr("&View"));
	menuBar()->addMenu(view_);

	view_plot_action_ = view_->addAction(tr("Plot Variables ..."));
	(void)connect(view_plot_action_, &QAction::triggered, this, &XeroPathGenerator::viewPlotEdit);

	view_trajectory_data_ = view_->addAction(tr("Trajectory Data ..."));
	(void)connect(view_trajectory_data_, &QAction::triggered, this, &XeroPathGenerator::viewTrajectoryData);

	//
	// Robot menu - gets filled in the populateRobotsMenu() method
	//
	robots_ = new QMenu(tr("Robots"));
	robots_group_ = new QActionGroup(this);
	robots_group_->setExclusive(true);
	menuBar()->addMenu(robots_);

	//
	// Games menu - gets filled in the populateGamesMenu() method
	//
	games_ = new QMenu(tr("&Games"));
	games_group_ = new QActionGroup(this);
	games_group_->setExclusive(true);
	menuBar()->addMenu(games_);

	//
	// Generators menu - gets fill in the populateGeneratorsMenu() method
	//
	generators_ = new QMenu(tr("Generator"));
	gen_group_ = new QActionGroup(this);
	gen_group_->setExclusive(true);
	menuBar()->addMenu(generators_);

	help_ = new QMenu(tr("&Help"));
	action = help_->addAction(tr("About"));
	(void)connect(action, &QAction::triggered, this, &XeroPathGenerator::showAbout);

	action = help_->addAction(tr("Documentation"));
	(void)connect(action, &QAction::triggered, this, &XeroPathGenerator::showDocumentation);

	help_->addSeparator();

	logmenu_ = help_->addMenu("Logging");

	debug_action_ = logmenu_->addAction("Debug");
	debug_action_->setCheckable(true);
	(void)connect(debug_action_, &QAction::triggered, this, &XeroPathGenerator::toggleDebugLogging);

	info_action_ = logmenu_->addAction("Info");
	info_action_->setCheckable(true);
	(void)connect(info_action_, &QAction::triggered, this, &XeroPathGenerator::toggleInfoLogging);

	warning_action_ = logmenu_->addAction("Warning");
	warning_action_->setCheckable(true);
	(void)connect(warning_action_, &QAction::triggered, this, &XeroPathGenerator::toggleWarningLogging);

	menuBar()->addMenu(help_);

	return true;
}

bool XeroPathGenerator::createToolbar()
{
	QPixmap image;
	QIcon icon;
	QString imagepath;
	QPushButton* button;

	toolbar_ = addToolBar("Demo");

	QString exedir = QCoreApplication::applicationDirPath();

	imagepath = exedir + "/images/" + PlayButtonImage;
	image = QPixmap(imagepath);
	icon = QIcon(image);
	button = new QPushButton(icon, "", toolbar_);
	play_action_ = toolbar_->addWidget(button);
	play_action_->setToolTip("Play Demo");
	play_action_->setDisabled(true);
	(void)connect(button, &QPushButton::pressed, this, &XeroPathGenerator::demoPlay);

	imagepath = exedir + "/images/" + StopButtonImage;
	image = QPixmap(imagepath);
	icon = QIcon(image);
	button = new QPushButton(icon, "", toolbar_);
	stop_action_ = toolbar_->addWidget(button);
	stop_action_->setToolTip("Stop Demo");
	stop_action_->setDisabled(true);
	(void)connect(button, &QPushButton::pressed, this, &XeroPathGenerator::demoStop);

	toolbar_->addSeparator();

	imagepath = exedir + "/images/" + ForwardOneButtonImage;
	image = QPixmap(imagepath);
	icon = QIcon(image);
	button = new QPushButton(icon, "", toolbar_);
	forward_one_action_ = toolbar_->addWidget(button);
	forward_one_action_->setToolTip("Move Demo forward one timestep");
	forward_one_action_->setDisabled(true);
	(void)connect(button, &QPushButton::pressed, this, &XeroPathGenerator::demoForwardOne);

	imagepath = exedir + "/images/" + ForwardTenButtonImage;
	image = QPixmap(imagepath);
	icon = QIcon(image);
	button = new QPushButton(icon, "", toolbar_);
	forward_ten_action_ = toolbar_->addWidget(button);
	forward_ten_action_->setToolTip("Move demo forward ten timesteps");
	forward_ten_action_->setDisabled(true);
	(void)connect(button, &QPushButton::pressed, this, &XeroPathGenerator::demoForwardTen);

	toolbar_->addSeparator();

	imagepath = exedir + "/images/" + RobotButtonImage;
	image = QPixmap(imagepath);
	icon = QIcon(image);
	button = new QPushButton(icon, "", toolbar_);
	show_robot_action_ = toolbar_->addWidget(button);
	show_robot_action_->setToolTip("Move demo forward ten timesteps");
	show_robot_action_->setDisabled(true);
	(void)connect(button, &QPushButton::pressed, this, &XeroPathGenerator::demoRobot);

	toolbar_->addSeparator();

	demo_mode_off_ = new QRadioButton("Demo Off");
	demo_mode_off_->setChecked(true);
	demo_mode_off_->setDisabled(true);
	(void)connect(demo_mode_off_, &QRadioButton::clicked, this, &XeroPathGenerator::demoPathModeChanged);
	toolbar_->addWidget(demo_mode_off_);

	demo_mode_on_ = new QRadioButton("Demo On");
	demo_mode_on_->setDisabled(true);
	(void)connect(demo_mode_on_, &QRadioButton::clicked, this, &XeroPathGenerator::demoPathModeChanged);
	toolbar_->addWidget(demo_mode_on_);

	toolbar_->setVisible(true);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// This section of code is all about initializing the system when it is started
//
//////////////////////////////////////////////////////////////////////////////////////////

void XeroPathGenerator::readMessageTypes()
{
	QVariant v = settings_.value("messages");
	QList<QVariant> vl = v.toList();
	for (const QVariant& m : vl)
	{
		if (m.canConvert<int>())
		{
			int mt = m.toInt();
			QtMsgType qmt = static_cast<QtMsgType>(mt);
			//
			// Since we rely on the messages_ array to hold state and the only way to update this
			// array is to toggle menu items, we do a filter here just in case this gets extra data
			// in it at some point.
			//
			if (std::find(messages_.begin(), messages_.end(), qmt) == messages_.end())
				messages_.push_back(qmt);
		}
	}
}

void XeroPathGenerator::writeMessageTypes()
{
	QList<QVariant> messages;
	for (QtMsgType t : messages_)
	{
		int v = static_cast<int>(t);
		messages.push_back(QVariant(v));
	}
	settings_.setValue("messages", messages);
}

void XeroPathGenerator::readGeneratorParams(const std::string& name, GeneratorParameterStore& store)
{
	QString keyname = QString("generator/") + name.c_str();
	QVariant value = settings_.value(keyname);
	store.assignValues(value.toHash());
}

void XeroPathGenerator::writeGeneratorParams(const std::string& name, const GeneratorParameterStore& store)
{
	QString keyname = QString("generator/") + name.c_str();
	QVariant value(store);
	settings_.setValue(keyname, value);
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// This section of code is all about setting the basic entities we are using to edit.  This
// includes paths, robots, units, etc.
//
//////////////////////////////////////////////////////////////////////////////////////////

void XeroPathGenerator::setUnits(const std::string& units)
{
	std::string oldunits = units_;
	units_ = units;

	fields_mgr_.convert(units_);
	robot_mgr_.convert(units_);

	path_view_->setUnits(units_);
	path_engine_.setUnits(units_);
	plots_->setUnits(units.c_str());
	if (oldunits.length() > 0)
		paths_model_.convert(oldunits, units_);

	allPathsDirty();
}

void XeroPathGenerator::setDefaultField()
{
	QVariant field = settings_.value("field");
	QString str = field.toString();
	std::string name = str.toStdString();
	if (name.length() == 0 || !fields_mgr_.exists(name))
	{
		auto deffield = fields_mgr_.getDefaultField();
		if (deffield != nullptr)
			name = deffield->getName();
	}

	if (name.length() > 0)
		setField(name);
}

void XeroPathGenerator::setDefaultGenerator()
{
	QVariant gen = settings_.value("generator");
	QString str = gen.toString();
	std::string name = str.toStdString();
	if (name.length() == 0 || !generators_mgr_.exists(name))
	{
		auto defgen = generators_mgr_.getDefaultGenerator();
		if (defgen != nullptr)
			name = defgen->getName();
	}

	if (name.length() > 0)
		setGenerator(name);
}

void XeroPathGenerator::setDefaultRobot()
{
	QVariant robot = settings_.value("robot");
	QString robotstr = robot.toString();
	std::string name = robotstr.toStdString();
	if (name.length() == 0 || !robot_mgr_.exists(name))
	{
		auto defrobot = robot_mgr_.getDefaultRobot();
		if (defrobot != nullptr)
			name = defrobot->getName();
	}
	if (name.length())
		setRobot(name);
}

void XeroPathGenerator::setField(const std::string& name)
{
	//
	// Set the current field
	//
	current_field_ = fields_mgr_.getFieldByName(name);
	auto actions = games_->actions();
	for (auto action : actions)
	{
		action->setCheckable(true);
		action->setChecked(false);
	}
	path_view_->setField(current_field_);

	if (current_field_ != nullptr)
	{
		for (auto action : actions)
		{
			if (action->text() == name.c_str())
				action->setChecked(true);
		}
	}
}

void XeroPathGenerator::setGenerator(const std::string& name)
{
	current_generator_ = generators_mgr_.getGeneratorByName(name);
	auto actions = generators_->actions();
	for (auto action : actions)
	{
		if (action->isSeparator())
			break;
		action->setCheckable(true);
		action->setChecked(false);
	}

	path_engine_.setGenerator(current_generator_);
	path_param_model_.setGenerator(current_generator_);
	constraints_->setGenerator(current_generator_);

	allPathsDirty();

	GeneratorParameterStore store;
	readGeneratorParams(current_generator_->getName(), store);
	path_engine_.setGeneratorStore(store);

	if (current_generator_ != nullptr)
	{
		for (auto action : actions)
		{
			if (action->text() == name.c_str())
				action->setChecked(true);
		}
	}
}

void XeroPathGenerator::setRobot(const std::string& name)
{
	current_robot_ = robot_mgr_.getRobotByName(name);
	paths_model_.setRobot(current_robot_);
	path_view_->setRobot(current_robot_);
	path_engine_.setRobot(current_robot_);
	if (current_robot_ != nullptr && current_robot_->getDriveType() == RobotParams::DriveType::SwerveDrive)
		path_param_model_.setAnglesRequired(true);
	else
		path_param_model_.setAnglesRequired(false);

	allPathsDirty();

	//
	// Now check the right robot in the menu
	//
	for (auto action : robots_->actions())
	{
		if (action->isSeparator())
			break;

		if (action->text() == name.c_str())
		{
			action->setChecked(true);
			break;
		}
	}
}

void XeroPathGenerator::setPath(std::shared_ptr<RobotPath> path)
{
	current_path_ = path;
	path_view_->setPath(current_path_);
	path_param_model_.setPath(current_path_);
	plots_->setPath(current_path_);
	waypoint_model_.enable(false);
	path_parameters_->resizeColumnToContents(0);
	constraints_->setPath(current_path_);
	plot_main_->setPath(current_path_);

	if (current_path_ != nullptr && !current_path_->hasTrajectory(TrajectoryName::Main))
		currentPathChanged();

	scrollBarChanged();
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// This section of code is all about responding to events from the system.
//
//////////////////////////////////////////////////////////////////////////////////////////

void XeroPathGenerator::timerProc()
{
	log_messages_lock_.lock();
	if (log_messages_.size() > 0)
	{
		while (log_messages_.size() > 0)
		{
			QString str = log_messages_.front();
			log_messages_.pop_front();
			log_->append(str);
		}
	}
	log_messages_lock_.unlock();

	std::shared_ptr<RobotPath> path = path_engine_.getComplete();
	if (path == current_path_ && path != nullptr)
	{
		plot_main_->update();
		path_param_model_.reset();
		if (traj_window_ != nullptr && traj_window_->isShowingPath(path))
			traj_window_->update();
	}

	if (demo_mode_ != DemoMode::ModeNone)
	{
		size_t numticks = static_cast<size_t>(timerTickMS / (current_robot_->getTimestep() * 1000));
		for (size_t i = 0; i < numticks; i++)
		{
			if (step_count_ > 0)
			{
				if (path_view_->isDemoPathDone())
				{
					nextOneInDemo();
					path_view_->resetDemo();
				}
				else
				{
					path_view_->demoStep();
					step_count_--;
				}

				play_action_->setDisabled(true);
				forward_one_action_->setDisabled(true);
				forward_ten_action_->setDisabled(true);
				stop_action_->setDisabled(false);
				show_robot_action_->setDisabled(true);
			}
			else
			{
				if (robot_window_ != nullptr)
					robot_window_->update();

				play_action_->setDisabled(false);
				forward_one_action_->setDisabled(false);
				forward_ten_action_->setDisabled(false);
				stop_action_->setDisabled(true);
				show_robot_action_->setDisabled(false);
				break;
			}
		}
		plots_->setCursorTime(path_view_->getDemoTime());
		if (traj_window_ != nullptr)
			traj_window_->setCurrentTime(path_view_->getDemoTime());
		QString timestr = QString::number(path_view_->getDemoTime(), 'f', 2);
		status_text_->setText(timestr);
		path_view_->repaint();
	}

	download_mgr_.tick();

	if (download_mgr_.newFields())
	{
		QMessageBox box(QMessageBox::Icon::Information,
			"Information", "New game fields have been downloaded, restart program to make these available", QMessageBox::StandardButton::Ok);
		box.exec();
	}
}

void XeroPathGenerator::nextOneInDemo()
{
	if (demo_mode_ == DemoMode::ModeGroup)
	{
		auto group = paths_->getSelectedGroup();
		size_t which = group->getIndexFromPath(path_view_->getPath()) + 1;
		if (which == group->getPathCount())
			which = 0;

		auto path = group->getPathByIndex(which);
		path_view_->setPath(path);
		plots_->setPath(path);
	}
}

void XeroPathGenerator::pathWindowMouseMoved(xero::paths::Translation2d pos)
{
	char buf[64];

	snprintf(buf, sizeof(buf), "X: %2.1f %s", pos.getX(), units_.c_str());
	xpos_text_->setText(buf);

	snprintf(buf, sizeof(buf), "Y: %2.1f %s", pos.getY(), units_.c_str());
	ypos_text_->setText(buf);
}

void XeroPathGenerator::showEvent(QShowEvent* ev)
{
	QMainWindow::showEvent(ev);

	qInstallMessageHandler(messageLogger);

	//
	// Copy all previous log messages to the log window
	//
	std::string line;
	while (std::getline(strstream_, line))
	{
		log_->append(line.c_str());
	}
	qDebug() << "Message logging transferred to application";

	readMessageTypes();
	std::list<QtMsgType> turnon = messages_;
	messages_.clear();

	for (QtMsgType t : turnon)
	{
		switch (t)
		{
		case QtMsgType::QtDebugMsg:
			toggleDebugLogging();
			break;
		case QtMsgType::QtInfoMsg:
			toggleInfoLogging();
			break;
		case QtMsgType::QtWarningMsg:
			toggleWarningLogging();
			break;
		default:
		  	break; 
		}
	}

	write_messages_ = true;

	if (robot_mgr_.getRobots().size() == 0)
	{
		QMessageBox box(QMessageBox::Icon::Information,
			"Information", "There are no robots defined.  Please define a robot now", QMessageBox::StandardButton::Ok);
		box.exec();
		newRobotAction();
	}

	initPlotVars();
}

void XeroPathGenerator::closeEvent(QCloseEvent* event)
{
	if (paths_model_.isDirty())
	{
		QMessageBox::StandardButton reply;

		reply = QMessageBox::question(this, "Question?", 
			"You have unsaved changes, are you sure you want to quit?", QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::No)
		{
			event->ignore();
			return;
		}
	}
	settings_.setValue(GeometrySetting, saveGeometry());
	settings_.setValue(WindowStateSetting, saveState());

	QList<QVariant> stored;
	for (int size : m_left_top_bottom_->sizes())
		stored.push_back(QVariant(size));
	settings_.setValue(LeftSplitterSetting, stored);

	stored.clear();
	for (int size : m_right_stack_->sizes())
		stored.push_back(QVariant(size));
	settings_.setValue(RightSplitterSetting, stored);

	stored.clear();
	for (int size : m_left_right_->sizes())
		stored.push_back(QVariant(size));
	settings_.setValue(LeftRightSplitterSetting, stored);

	path_engine_.stopAll();
	QMainWindow::closeEvent(event);
}

void XeroPathGenerator::waypointTreeDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
  	(void)topLeft ;
	(void)bottomRight ;
	(void)roles ;

	const Pose2d& current = current_path_->getPoints()[path_view_->getSelected()];
	std::shared_ptr<WaypointChangedUndo> entry =
		std::make_shared<WaypointChangedUndo>(*this, current_path_->getParent()->getName(), 
			current_path_->getName(), path_view_->getSelected(), current);
	UndoManager::getUndoManager().pushUndoStack(entry);
	
	current_path_->replacePoint(path_view_->getSelected(), waypoint_model_.getWaypoint());
	currentPathChanged();
	path_view_->repaint();
}

void XeroPathGenerator::pathParamTreeDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
  	(void)topLeft ;
	(void)bottomRight ;
	(void)roles ;	

	paths_model_.setDirty();
	currentPathChanged();
	setXeroWindowTitle();
}

void XeroPathGenerator::pathTreeDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
  	(void)topLeft ;
	(void)bottomRight ;
	(void)roles ;	

	currentPathChanged();
	path_view_->repaint();
}

void XeroPathGenerator::pathTreeSelectionChanged(const QModelIndex& current, const QModelIndex& prev)
{
  	(void)current ;
	(void)prev ;
  
	setPath(paths_->getSelectedPath());

	if (paths_->isPathSelected() || (paths_->isGroupSelected() && paths_->getSelectedGroup()->size() > 0))
	{
		demo_mode_off_->setDisabled(false);
		demo_mode_on_->setDisabled(false);
	}
	else
	{
		demo_mode_on_->setDisabled(true);
	}
}

void XeroPathGenerator::insertedWaypointProc()
{
	paths_model_.setDirty();
	currentPathChanged();
	setXeroWindowTitle();
}

void XeroPathGenerator::deletedWaypointProc()
{
	paths_model_.setDirty();
	currentPathChanged();
	setXeroWindowTitle();
}

void XeroPathGenerator::startMovingWaypointProc(size_t index)
{
  	(void)index ;
	orig_point_ = current_path_->getPoints()[path_view_->getSelected()];
	std::shared_ptr<WaypointChangedUndo> entry = 
		std::make_shared<WaypointChangedUndo>(*this, current_path_->getParent()->getName(), 
			current_path_->getName(), path_view_->getSelected(), orig_point_);
	UndoManager::getUndoManager().pushUndoStack(entry);
}

void XeroPathGenerator::endMovingWaypointProc(size_t index)
{
	auto &moved = current_path_->getPoints()[path_view_->getSelected()];
	if (!moved.espilonEquals(orig_point_, 0.1))
	{
		paths_model_.setDirty();
		selectedWaypointProc(index);
		currentPathChanged();
		setXeroWindowTitle();
	}
	else
	{
		//
		// The point did not really move
		//
		// We put an entry on the todo stack that should not be there.
		//
		UndoManager::getUndoManager().undo();
	}
}

void XeroPathGenerator::movingWaypointProc(size_t index)
{
	paths_model_.setDirty();
	selectedWaypointProc(index);
	setXeroWindowTitle();
}

void XeroPathGenerator::selectedWaypointProc(size_t index)
{
	if (index >= current_path_->size())
		waypoint_model_.enable(false);
	else
	{
		waypoint_model_.enable(true);
		waypoint_model_.setWaypoint(current_path_->getPoints()[index], index, current_path_->getDistance(index));
		waypoint_parameters_->resizeColumnToContents(0);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// This section of code is about connecting the QT message logger to the log window in
// the GUI.  Any log messages generated before the GUI is avaialble are preserved and are
// copied to the log window.
//
//////////////////////////////////////////////////////////////////////////////////////////

void XeroPathGenerator::messageLogger(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	theOne->messageLoggerWin(type, context, msg);
}

void XeroPathGenerator::messageLoggerWin(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	std::stringstream log2stream;

	QByteArray localMsg = msg.toLocal8Bit();
	const char* file = context.file ? context.file : "";
	const char* function = context.function ? context.function : "";
	switch (type) {
	case QtDebugMsg:
		log2stream << "Debug: ";
		break;
	case QtInfoMsg:
		log2stream << "Info: ";
		break;
	case QtWarningMsg:
		log2stream << "Warning: ";
		break;
	case QtCriticalMsg:
		log2stream << "Critical: ";
		break;
	case QtFatalMsg:
		log2stream << "Error: ";
		break;
	}

	logstream_ << localMsg.constData() << "(" << file << ":" << context.line << ", " << function << std::endl;
	log2stream << localMsg.constData();

	//
	// For information coming from the generation thread, this crashes the program.  Need to dig
	// into this more deeply to figure out how to send log messages between threads.
	//
	if (std::find(messages_.begin(), messages_.end(), type) != messages_.end())
	{
		if (std::this_thread::get_id() == gui_thread_)
			log_->append(log2stream.str().c_str());
		else
		{
			log_messages_lock_.lock();
			log_messages_.push_back(log2stream.str().c_str());
			log_messages_lock_.unlock();
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// This section of code is all about responding to menu selections.  Technically these are
// events as well but they are specialized enough they got their own section
//
//////////////////////////////////////////////////////////////////////////////////////////

void XeroPathGenerator::demoRobot()
{
	if (robot_window_ == nullptr)
	{
		robot_window_ = new ShowRobotWindow(path_view_);
	}

	robot_window_->show();
}

void XeroPathGenerator::demoPlay() 
{
	step_count_ = std::numeric_limits<size_t>::max();
}

void XeroPathGenerator::demoStop() 
{
	step_count_ = 0;
}

void XeroPathGenerator::demoForwardOne() 
{
	step_count_ = 1;
}

void XeroPathGenerator::demoForwardTen() 
{
	step_count_ = 10;
}

void XeroPathGenerator::setupDemoMode(bool active)
{
	path_view_->setDemoMode(active);
	robots_->setDisabled(active);
	generators_->setDisabled(active);
	games_->setDisabled(active);
	file_->setDisabled(active);
	edit_->setDisabled(active);
	view_->setDisabled(active);
	paths_->setDisabled(active);
	path_parameters_->setDisabled(active);
	waypoint_parameters_->setDisabled(active);
	constraints_->setDisabled(active);

	play_action_->setDisabled(!active);
	stop_action_->setDisabled(!active);
	forward_one_action_->setDisabled(!active);
	forward_ten_action_->setDisabled(!active);
	show_robot_action_->setDisabled(!active);

	if (active)
		status_text_->setText(QString::number(0.0, 'f', 2));
}

void XeroPathGenerator::demoPathModeChanged()
{
	if (demo_mode_off_->isChecked())
	{
		demo_mode_ = DemoMode::ModeNone;
		setupDemoMode(false);
		status_text_->setText("");
	}
	else if (demo_mode_on_->isChecked())
	{
		if (paths_->isPathSelected())
		{
			demo_mode_ = DemoMode::ModePath;
			step_count_ = 0;
			setupDemoMode(true);
		}
		else if (paths_->isGroupSelected())
		{
			demo_mode_ = DemoMode::ModeGroup;
			std::shared_ptr<PathGroup> group = paths_->getSelectedGroup();
			if (group == nullptr || group->getPathCount() == 0)
				return;

			path_view_->setPath(group->getPathByIndex(0));
			plots_->setPath(group->getPathByIndex(0));

			step_count_ = 0;
			setupDemoMode(true);
		}
	}
}

void XeroPathGenerator::showFileMenu()
{
	if (paths_model_.isDirty())
		file_save_->setEnabled(true);
	else
		file_save_->setEnabled(false);

	if (paths_->isPathSelected())
		file_publish_->setEnabled(true);
	else
		file_publish_->setEnabled(false);
}

void XeroPathGenerator::fileNew()
{
	fileClose();
}

void XeroPathGenerator::fileOpen()
{
	if (paths_model_.isDirty())
	{
		QMessageBox question;
		question.setText("The paths have been modified.");
		question.setInformativeText("Do you want to save your changes?");
		question.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		question.setDefaultButton(QMessageBox::Save);
		int ret = question.exec();
		if (ret == QMessageBox::Cancel)
			return;

		if (ret == QMessageBox::Save)
		{
			if (!save())
				return;
		}
	}

	QString filename = QFileDialog::getOpenFileName(this, tr("Load Path File"), "", tr("Path File (*.path);;All Files (*)"));
	if (filename.length() == 0)
		return;

	fileOpenWithName(filename);

}

void XeroPathGenerator::fileOpenWithName(QString filename)
{
	QFileInfo info(filename);
	if (!info.exists())
	{
		std::string msg = "File'";
		msg += filename.toStdString();
		msg += "' does not exist";
		QMessageBox box(QMessageBox::Icon::Critical,
			"Error", msg.c_str(), QMessageBox::StandardButton::Ok);
		box.exec();

		recents_->removeRecentFile(this, filename);
		return;
	}

	//
	// Ok, we are about to read new paths.  Shutdown the path generation engine
	//
	paths_model_.clear();
	if (!PathCollectionIO::readPathCollection(filename.toStdString(), paths_model_.getPathCollection()))
	{
		std::string msg = "Error loading file'";
		msg += filename.toStdString();
		msg += "'";
		QMessageBox box(QMessageBox::Icon::Critical,
			"Error", msg.c_str(), QMessageBox::StandardButton::Ok);
		box.exec();

		return;
	}

	recents_->addRecentFile(this, filename);
	paths_model_.reset(true);
	setFileName(filename);
	setPath(nullptr);
	allPathsDirty();
}

void XeroPathGenerator::setFileName(QString str)
{
	path_file_name_ = str.toStdString();
	setXeroWindowTitle();
}

void XeroPathGenerator::setXeroWindowTitle()
{
	QString str = QString("Xero Path Generator - ") + path_file_name_.c_str();
	if (paths_model_.isDirty())
		str += " [ Modified ]";
	setWindowTitle(str);
}

void XeroPathGenerator::setPathDirty(std::shared_ptr<xero::paths::RobotPath> path)
{
	path->clearTrajectories();

	if (path == current_path_)
		plot_main_->update();
	if (traj_window_ != nullptr && traj_window_->isShowingPath(path))
		traj_window_->update();

	path_engine_.markPathDirty(path);
}

void XeroPathGenerator::allPathsDirty()
{
	std::list<std::shared_ptr<RobotPath>> allpaths;
	paths_model_.getAllPaths(allpaths);
	for (auto path : allpaths)
		setPathDirty(path);
}

bool XeroPathGenerator::save()
{
	if (path_file_name_.length() == 0)
	{
		QFileDialog dialog;

		QString filename = QFileDialog::getSaveFileName(this, tr("Save Path File"), "", tr("Path File (*.path);;All Files (*)"));
		if (filename.length() == 0)
			return false;

		setFileName(filename);
	}

	QFile file(path_file_name_.c_str());
	if (!PathCollectionIO::writePathCollection(file, paths_model_.getPathCollection()))
	{
		std::string msg = "Save to file '";
		msg += path_file_name_;
		msg += "' failed";
		QMessageBox box(QMessageBox::Icon::Critical,
			"Error", msg.c_str(), QMessageBox::StandardButton::Ok);
		box.exec();

		return false;
	}

	paths_model_.setClean();
	setXeroWindowTitle();

	recents_->addRecentFile(this, file.fileName());

	QString str;
	str = "Saved ";
	str += file.fileName();
	QMessageBox box(QMessageBox::Icon::Information, "Success", str, QMessageBox::StandardButton::Ok);
	box.exec();
	return true;
}

void XeroPathGenerator::fileSave()
{
	save();
}

void XeroPathGenerator::fileSaveAs()
{
	QFileDialog dialog;

	QString filename = QFileDialog::getSaveFileName(this, tr("Save Path File"), "", tr("Path File (*.path);;All Files (*)"));
	if (filename.length() == 0)
		return;
	setFileName(filename);
	save();
}

void XeroPathGenerator::fileClose()
{
	if (paths_model_.isDirty())
	{
		QMessageBox question;
		question.setText("The paths have been modified.");
		question.setInformativeText("Do you want to save your changes?");
		question.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		question.setDefaultButton(QMessageBox::Save);
		int ret = question.exec();
		if (ret == QMessageBox::Cancel)
			return;

		if (ret == QMessageBox::Save)
		{
			if (!save())
				return;
		}
	}
	paths_model_.clear();
	paths_model_.setClean();
	setFileName("");
	setPath(nullptr);
}

void XeroPathGenerator::fileGenerate()
{
	if (last_path_dir_.length() == 0)
		fileGenerateAs();
	else
		generate();
}

void XeroPathGenerator::fileGenerateAs()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (dir.length() == 0)
		return;

	last_path_dir_ = dir.toStdString();
	settings_.setValue(LastPathDirSetting, last_path_dir_.c_str());

	generate();
}

void XeroPathGenerator::generateOnePath(std::shared_ptr<RobotPath> path, const std::string &trajname, std::ostream& outfile)
{
	std::vector<std::string> headers =
	{
		RobotPath::TimeTag,
		RobotPath::XTag,
		RobotPath::YTag,
		RobotPath::PositionTag,
		RobotPath::VelocityTag,
		RobotPath::AccelerationTag,
		RobotPath::JerkTag,
		RobotPath::HeadingTag
	};

	auto t = path->getTrajectory(trajname);
	if (output_type_ == OutputType::OutputCSV)
	{
		CSVWriter::write<std::vector<Pose2dWithTrajectory>::const_iterator>(outfile, headers, t->begin(), t->end());
	}
	else
	{
		JSONWriter::write<std::vector<Pose2dWithTrajectory>::const_iterator>(outfile, headers, t->begin(), t->end());
	}
}

void XeroPathGenerator::generate()
{
	std::chrono::milliseconds delay(100);

	int count = 0;
	auto total = static_cast<int>(paths_model_.getPathCollection().getPathCount()) ;
	int pending;

	prog_bar_->setVisible(true);
	prog_bar_->setRange(0, total);
	allPathsDirty();

	setCursor(Qt::WaitCursor);
	status_text_->setText("Generating Paths");

	do {
		pending = static_cast<int>(path_engine_.waitingPaths());
		prog_bar_->setValue(total - pending);
		std::this_thread::sleep_for(delay);
	} while (pending > 0);


	status_text_->setText("Writing Paths");
	count = 0;
	std::list<std::shared_ptr<RobotPath>> pathlist;
	paths_model_.getAllPaths(pathlist);
	std::string outfile;
	for (auto path : pathlist)
	{
		prog_bar_->setValue(count);

		std::vector<std::string> names = path->getTrajectoryNames();

		for (const std::string& trajname : names)
		{
			//
			// Writre the paths to the last_path_dir directory as
			// CSV files
			//
			if (output_type_ == OutputType::OutputCSV)
			{
				outfile = last_path_dir_ + "/" + path->getParent()->getName() + "_" + path->getName() + "_" + trajname + ".csv";
			}
			else
			{
				outfile = last_path_dir_ + "/"  + path->getParent()->getName() + "_" + path->getName() + "_" + trajname + ".json";
			}

			std::ofstream outstrm(outfile);
			generateOnePath(path, trajname, outstrm);
		}
		count++;
	}

	prog_bar_->setVisible(false);
	status_text_->setText("Complete");
	setCursor(Qt::ArrowCursor);
}

void XeroPathGenerator::filePrint()
{
	if (!paths_->isPathSelected() && !paths_->isGroupSelected())
	{
		std::string msg("Please select a path or path group to print.");
		QMessageBox box(QMessageBox::Icon::Warning,
			"Error", msg.c_str(), QMessageBox::StandardButton::Ok);
		box.exec();
		return;
	}

	QPrinter printer(QPrinter::HighResolution);
	QPrintDialog dialog(&printer, this);
	dialog.setWindowTitle(tr("Print Document"));
	if (dialog.exec() != QDialog::Accepted)
		return;

	PrintController ctrl(*path_view_, *plots_);
	if (paths_->isPathSelected())
		ctrl.printField(printer, paths_->getSelectedPath());
	else if (paths_->isGroupSelected())
		ctrl.printGroup(printer, paths_->getSelectedGroup());
}

void XeroPathGenerator::filePublish()
{
	if (paths_model_.isDirty())
	{
		std::string msg("The paths have been changed without being saved.  You must save the paths to publish them to the robot.  Do you want to save the paths?");
		QMessageBox box(QMessageBox::Icon::Warning,
			"Error", msg.c_str(), QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);
		int result = box.exec();

		if (result == QMessageBox::StandardButton::No)
			return;

		save();
	}

	if (paths_model_.isDirty())
	{
		std::string msg("You choose not to save the paths, therefore the path selected will NOT be published to the robot.");
		QMessageBox box(QMessageBox::Icon::Warning,
			"Error", msg.c_str(), QMessageBox::StandardButton::Ok);
		box.exec();

		return;
	}

	auto path = paths_->getSelectedPath();
	if (path == nullptr)
	{
		std::string msg("There is no path selected, path not published to robot.");
		QMessageBox box(QMessageBox::Icon::Warning,
			"Error", msg.c_str(), QMessageBox::StandardButton::Ok);
		box.exec();
	}
	else
	{
		nt::NetworkTableInstance inst = nt::NetworkTableInstance::GetDefault();
		auto pubtable = inst.GetTable(nttable_name_.c_str());

		const std::vector<std::string>& trajnames = DriveBaseData::getTrajectories(current_robot_->getDriveType());

		for (const std::string& trajname : trajnames)
		{
			std::string name = path->getParent()->getName() + "_" + path->getName() + "_" + trajname;
			std::stringstream sstrm;

			generateOnePath(path, trajname, sstrm);
			pubtable->PutString(name, sstrm.str());
		}
	}
}

void XeroPathGenerator::viewPlotEdit()
{
	if (current_robot_ == nullptr)
		return;

	PlotVariableSelector dialog(current_robot_->getDriveType(), plots_->getPlotVariables(), this);

	if (dialog.exec() == QDialog::Accepted)
	{
		plots_->clearPlotVariables();
		plots_->addPlotVariables(dialog.getPlotVariables());
		plots_->setPath(current_path_);
		savePlotVars();
	}
}

void XeroPathGenerator::viewTrajectoryData()
{
	if (current_robot_ == nullptr || current_path_ == nullptr)
		return;

	if (traj_window_ == nullptr)
		traj_window_ = new TrajectoryViewWindow();

	if (current_path_->hasTrajectory(TrajectoryName::Main))
		traj_window_->setPath(current_path_);

	traj_window_->show();
}

void XeroPathGenerator::showEditMenu()
{
	if (paths_->isGroupSelected())
	{
		delete_path_or_group_action_->setText(tr("Delete Path Group"));
		delete_path_or_group_action_->setEnabled(true);
	}
	else if (paths_->isPathSelected()) {
		delete_path_or_group_action_->setText(tr("Delete Path"));
		delete_path_or_group_action_->setEnabled(true);
	}
	else {
		delete_path_or_group_action_->setEnabled(false);
		delete_path_or_group_action_->setText(tr("Delete"));
	}

	insert_waypoint_action_->setEnabled(path_view_->isInsertWaypointValid());
	delete_waypoint_action_->setEnabled(path_view_->isDeleteWaypointValid());

	undo_action_->setEnabled(undo_mgr_.hasUndoInfo());
}

void XeroPathGenerator::insertWaypoint()
{
	path_view_->insertWaypoint();
}

void XeroPathGenerator::deleteWaypoint()
{
	path_view_->deleteWaypoint();
}

void XeroPathGenerator::populateFieldsMenu()
{
	for (auto field : fields_mgr_.getFields())
	{
		QAction* act = games_->addAction(field->getName().c_str());
		games_group_->addAction(act);
		(void)connect(act, &QAction::triggered, this, [this, field] { newFieldSelected(field); });
	}
}

void XeroPathGenerator::populateGeneratorsMenu()
{
	QAction* action;

	for (auto generator : generators_mgr_.getGenerators())
	{
		QAction* act = generators_->addAction(generator->getName().c_str());
		gen_group_->addAction(act);
		(void)connect(act, &QAction::triggered, this, [this, generator] { newGeneratorSelected(generator); });
	}

	generators_->addSeparator();
	action = generators_->addAction(tr("Edit Generator Parameters ..."));
	(void)connect(action, &QAction::triggered, this, &XeroPathGenerator::editGeneratorParameters);
}

void XeroPathGenerator::populateRobotsMenu()
{
	bool first = true;
	for (auto robot : robot_mgr_.getRobots())
	{
		QAction* act = new QAction(robot->getName().c_str());
		act->setCheckable(true);
		act->setChecked(first);
		if (first)
			first = false;
		robots_->addAction(act);
		robots_group_->addAction(act);
		(void)connect(act, &QAction::triggered, this, [this, robot] { newRobotSelected(robot); });
	}

	robot_seperator_ = robots_->addSeparator();
	QAction* action = robots_->addAction(tr("New Robot ..."));
	(void)connect(action, &QAction::triggered, this, &XeroPathGenerator::newRobotAction);

	edit_robot_action_ = robots_->addAction(tr("Edit Current Robot ..."));
	(void)connect(edit_robot_action_, &QAction::triggered, this, &XeroPathGenerator::editRobotAction);

	action = robots_->addAction(tr("Delete Robot"));
	(void)connect(action, &QAction::triggered, this, &XeroPathGenerator::deleteRobotAction);

	robots_->addSeparator();

	action = robots_->addAction(tr("Export Current Robot ..."));
	(void)connect(action, &QAction::triggered, this, &XeroPathGenerator::exportCurrentRobot);

	action = robots_->addAction(tr("Import Robot ..."));
	(void)connect(action, &QAction::triggered, this, &XeroPathGenerator::importRobot);
}

void XeroPathGenerator::toggleDebugLogging()
{
	auto it = std::find(messages_.begin(), messages_.end(), QtMsgType::QtDebugMsg);
	if (it != messages_.end())
	{
		debug_action_->setChecked(false);
		messages_.erase(it);
	}
	else
	{
		debug_action_->setChecked(true);
		auto it2 = std::find(messages_.begin(), messages_.end(), QtMsgType::QtDebugMsg);
		if (it2 == messages_.end())
			messages_.push_back(QtMsgType::QtDebugMsg);
	}
	if (write_messages_)
		writeMessageTypes();
}

void XeroPathGenerator::toggleInfoLogging()
{
	auto it = std::find(messages_.begin(), messages_.end(), QtMsgType::QtInfoMsg);
	if (it != messages_.end()) 
	{
		info_action_->setChecked(false);
		messages_.erase(it);
	}
	else
	{
		info_action_->setChecked(true);
		auto it2 = std::find(messages_.begin(), messages_.end(), QtMsgType::QtInfoMsg);
		if (it2 == messages_.end())
			messages_.push_back(QtMsgType::QtInfoMsg);
	}
	if (write_messages_)
		writeMessageTypes();
}

void XeroPathGenerator::toggleWarningLogging()
{
	auto it = std::find(messages_.begin(), messages_.end(), QtMsgType::QtWarningMsg);
	if (it != messages_.end())
	{
		warning_action_->setChecked(false);
		messages_.erase(it);
	}
	else
	{
		warning_action_->setChecked(true);
		auto it2 = std::find(messages_.begin(), messages_.end(), QtMsgType::QtWarningMsg);
		if (it2 == messages_.end())
			messages_.push_back(QtMsgType::QtWarningMsg);
	}
	if (write_messages_)
		writeMessageTypes();
}

void XeroPathGenerator::showAbout()
{
	AboutDialog about(generators_mgr_, fields_mgr_);
	about.exec();
}

void XeroPathGenerator::showDocumentation()
{
	QString exedir = QCoreApplication::applicationDirPath();

	if (help_process_ == nullptr)
		help_process_ = std::make_shared<QProcess>();


	QStringList args;
	args.push_back("-collectionFile");
	args.push_back(exedir + "/xeropath.qhc");
	args.push_back("-enableRemoteControl");
	help_process_->setWorkingDirectory(exedir);
	help_process_->start(exedir + "/assistant", args);
	help_process_->waitForStarted();

	QByteArray a;
	a.append("register " + exedir + "/xeropath.qhc;\r\n");
	help_process_->write(a);

	a.clear();
	a.append("setSource qthelp://errorcodexero.pathgen.help/doc/intro.html\n;");
	help_process_->write(a);

	a.clear();
	a.append("expandToc -1;");
	help_process_->write(a);
}

void XeroPathGenerator::checkForUpdates()
{
	download_mgr_.updateFields();
}

void XeroPathGenerator::newFieldSelected(std::shared_ptr<GameField> field)
{
	if (field != current_field_)
	{
		setField(field->getName());
		settings_.setValue("field", field->getName().c_str());
	}
}

void XeroPathGenerator::newGeneratorSelected(std::shared_ptr<Generator> generator)
{
	if (generator != current_generator_)
	{
		setGenerator(generator->getName());
		settings_.setValue("generator", generator->getName().c_str());
	}
}

void XeroPathGenerator::editGeneratorParameters()
{
	std::shared_ptr<EditableProperty> prop;

	//
	// Get the existing values
	//
	GeneratorParameterStore store = path_engine_.getParameterStore();

	//
	// Move the existing values to the property editor.  If an existing value is
	// missing, use the default.
	//
	PropertyEditor editor("Generator Parameters", this);
	for (const auto& p : current_generator_->getGeneratorParams())
	{
		QVariant value;
		if (store.hasParameterValue(p.getName().c_str()))
			value = store[(p.getName().c_str())];
		else
			value = p.getDefault();

		if (p.getType() == GeneratorParameter::DoublePropType)
			prop = std::make_shared<EditableProperty>(p.getName().c_str(), EditableProperty::PTDouble, value, p.getDescription().c_str());
		else if (p.getType() == GeneratorParameter::IntegerPropType)
			prop = std::make_shared<EditableProperty>(p.getName().c_str(), EditableProperty::PTInteger, value, p.getDescription().c_str());
		else if (p.getType() == GeneratorParameter::StringPropType)
			prop = std::make_shared<EditableProperty>(p.getName().c_str(), EditableProperty::PTString, value, p.getDescription().c_str());
		else if (p.getType() == GeneratorParameter::StringListPropType)
		{
			prop = std::make_shared<EditableProperty>(p.getName().c_str(), EditableProperty::PTStringList, value, p.getDescription().c_str());
			for (const std::string& choice : p.getChoices())
				prop->addChoice(choice.c_str());
		}
		else
		{
			qDebug() << "Generator " << current_generator_->getName().c_str() << " has unknown property type '" << p.getType().c_str();
		}

		editor.getModel().addProperty(prop);
	}
	if (editor.exec() == QDialog::Rejected)
		return;

	//
	// Now move the values from the property editor to the store
	//
	for (auto prop2 : editor.getModel().getProperties())
		store[prop2->getName()] = prop2->getValue();

	writeGeneratorParams(current_generator_->getName(), store);
	path_engine_.setGeneratorStore(store);

	allPathsDirty();
}

void XeroPathGenerator::newRobotSelected(std::shared_ptr<RobotParams> robot)
{
	if (robot != current_robot_)
	{
		setRobot(robot->getName());
		settings_.setValue("robot", robot->getName().c_str());
	}
}

void XeroPathGenerator::showRobotMenu()
{
	if (current_robot_ == nullptr)
		edit_robot_action_->setEnabled(false);
	else
		edit_robot_action_->setEnabled(true);
}

void XeroPathGenerator::newRobotAction()
{
	createEditRobot(nullptr);
}

void XeroPathGenerator::editRobotAction()
{
	createEditRobot(current_robot_);
}

void XeroPathGenerator::exportCurrentRobot()
{
	QFileDialog dialog;

	QString filename = QFileDialog::getSaveFileName(this, tr("Save Robot File"), "", tr("Path File (*.robot);;All Files (*)"));
	if (filename.length() == 0)
		return;

	QFile file(filename);
	robot_mgr_.save(current_robot_, file);
}

void XeroPathGenerator::importRobot()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Load Path File"), "", tr("Path File (*.robot);;All Files (*)"));
	if (filename.length() == 0)
		return;

	QFile file(filename);
	try
	{
		auto robot = robot_mgr_.importRobot(file);
		QAction* newRobotAction = new QAction(robot->getName().c_str());
		robots_group_->addAction(newRobotAction);
		robots_->insertAction(robot_seperator_, newRobotAction);
		newRobotAction->setCheckable(true);
		(void)connect(newRobotAction, &QAction::triggered, this, [this, robot] { newRobotSelected(robot); });
		setRobot(robot->getName());
	}
	catch(const std::runtime_error &error)
	{
		std::string msg = "Could not import robot file '";
		msg += filename.toStdString();
		msg += "' - ";
		msg += error.what();

		QMessageBox box(QMessageBox::Icon::Critical,
			"Error", msg.c_str(), QMessageBox::StandardButton::Ok);
		box.exec();
	}
}

void XeroPathGenerator::deleteRobotAction()
{
	if (robot_mgr_.getRobots().size() == 1)
	{
		std::string msg = "Only a single robot exists and it cannot be deleted";
		QMessageBox box(QMessageBox::Icon::Critical,
			"Error", msg.c_str(), QMessageBox::StandardButton::Ok);
		box.exec();
	}
	else
	{
		SelectRobotDialog dialog(robot_mgr_, current_robot_);

		if (dialog.exec() == QDialog::Accepted)
		{
			QString str = dialog.getSelectedRobot();
			if (current_robot_ != nullptr && str == current_robot_->getName().c_str())
			{
				QMessageBox box(QMessageBox::Icon::Critical,
					"Error", "You cannot delete the current robot", QMessageBox::StandardButton::Ok);
				box.exec();
				return;
			}

			if (robot_mgr_.deleteRobot(str.toStdString()))
			{
				auto actlist = robots_->actions();
				auto it = std::find_if(actlist.begin(), actlist.end(), [str](QAction* item) { return item->text() == str; });
				robots_->removeAction(*it);
			}
		}
	}
}

void XeroPathGenerator::createEditRobot(std::shared_ptr<RobotParams> robot)
{	
	double elength, ewidth, rlength, rwidth;
	double velocity, accel, jerk, timestep;
	RobotParams::DriveType drivetype;
	std::string units;
	std::string name;
	bool create = (robot == nullptr);
	QString title;

	if (robot == nullptr)
	{
		//
		// Creating a new robot, use defaules
		elength = UnitConverter::convert(RobotParams::DefaultLength, RobotParams::DefaultUnits, units_);
		ewidth = UnitConverter::convert(RobotParams::DefaultWidth, RobotParams::DefaultUnits, units_);
		rlength = UnitConverter::convert(RobotParams::DefaultLength, RobotParams::DefaultUnits, units_);
		rwidth = UnitConverter::convert(RobotParams::DefaultWidth, RobotParams::DefaultUnits, units_);
		velocity = UnitConverter::convert(RobotParams::DefaultMaxVelocity, RobotParams::DefaultUnits, units_);
		accel = UnitConverter::convert(RobotParams::DefaultMaxAcceleration, RobotParams::DefaultUnits, units_);
		jerk = UnitConverter::convert(RobotParams::DefaultMaxJerk, RobotParams::DefaultUnits, units_);
		timestep = RobotParams::DefaultTimestep;
		drivetype = RobotParams::DefaultDriveType;
		units = RobotParams::DefaultUnits;
		title = "Create Robot";
	}
	else
	{
		elength = robot->getEffectiveLength();
		ewidth = robot->getEffectiveWidth();
		rlength = robot->getRobotLength();
		rwidth = robot->getRobotWidth();
		velocity = robot->getMaxVelocity();
		accel = robot->getMaxAccel();
		jerk = robot->getMaxJerk();
		timestep = robot->getTimestep();
		drivetype = robot->getDriveType();
		name = robot->getName();
		units = robot->getUnits();
		title = "Edit Robot";
	}

	while (1)
	{
		PropertyEditor* editor = new PropertyEditor(title, this);
		PropertyEditorTreeModel& model = editor->getModel();
		std::shared_ptr<EditableProperty> prop;

		prop = std::make_shared<EditableProperty>(RobotDialogName, EditableProperty::PTString,
			name.c_str(), "The name of the robot", !create);
		model.addProperty(prop);

		prop = std::make_shared<EditableProperty>(RobotDialogUnits, EditableProperty::PTStringList,
			QVariant(units.c_str()), "The units of measurement for the robot, can differ from the paths");
		auto list = UnitConverter::getAllUnits();
		for (auto& unit : list)
			prop->addChoice(unit.c_str());
		model.addProperty(prop);

		prop = std::make_shared<EditableProperty>(RobotDialogELength, EditableProperty::PTDouble, 
			std::to_string(elength).c_str(), "The effective length of the robot");
		model.addProperty(prop);

		prop = std::make_shared<EditableProperty>(RobotDialogEWidth, EditableProperty::PTDouble,
			std::to_string(ewidth).c_str(), "The effective width of the robot");
		model.addProperty(prop);

		prop = std::make_shared<EditableProperty>(RobotDialogRLength, EditableProperty::PTDouble,
			std::to_string(rlength).c_str(), "The physical length of the robot (outside bumper to outside bumper)");
		model.addProperty(prop);

		prop = std::make_shared<EditableProperty>(RobotDialogRWidth, EditableProperty::PTDouble,
			std::to_string(rwidth).c_str(), "The physical width of the robot (outside bumper to outside bumpter)");
		model.addProperty(prop);

		prop = std::make_shared<EditableProperty>(RobotDialogMaxVelocity, EditableProperty::PTDouble,
			std::to_string(velocity).c_str(), "The maximum velocity of the robot");
		model.addProperty(prop);

		prop = std::make_shared<EditableProperty>(RobotDialogMaxAcceleration, EditableProperty::PTDouble,
			std::to_string(accel).c_str(), "The maximum acceleration of the robot");
		model.addProperty(prop);

		prop = std::make_shared<EditableProperty>(RobotDialogMaxJerk, EditableProperty::PTDouble,
			std::to_string(jerk).c_str(), "The maximum jerk of the robot");
		model.addProperty(prop);

		prop = std::make_shared<EditableProperty>(RobotDialogDriveType, EditableProperty::PTStringList,
			QVariant(DriveBaseData::typeToName(drivetype).c_str()), "The drive type for the robot");
		auto drivetypes = RobotParams::getDriveTypes();
		QList<QVariant> dtypes;
		for (auto type : drivetypes)
			prop->addChoice(DriveBaseData::typeToName(type).c_str());
		model.addProperty(prop);

		prop = std::make_shared<EditableProperty>(RobotDialogTimeStep, EditableProperty::PTDouble,
			std::to_string(timestep).c_str(), "The time interval for the drive base control loop");
		model.addProperty(prop);

		if (editor->exec() == QDialog::Rejected)
		{
			delete editor;
			return;
		}

		elength = model.getProperty(RobotDialogELength)->getValue().toDouble();
		ewidth = model.getProperty(RobotDialogEWidth)->getValue().toDouble();
		rlength = model.getProperty(RobotDialogRLength)->getValue().toDouble();
		rwidth = model.getProperty(RobotDialogRWidth)->getValue().toDouble();
		velocity = model.getProperty(RobotDialogMaxVelocity)->getValue().toDouble();
		accel = model.getProperty(RobotDialogMaxAcceleration)->getValue().toDouble();
		jerk = model.getProperty(RobotDialogMaxJerk)->getValue().toDouble();
		drivetype = DriveBaseData::nameToType(model.getProperty(RobotDialogDriveType)->getValue().toString().toStdString());
		timestep = model.getProperty(RobotDialogTimeStep)->getValue().toDouble();

		if (create && model.getProperty(RobotDialogName)->getValue().toString().length() == 0)
		{
			std::string msg("The robot name is empty, a robot name must be supplied.");
			QMessageBox box(QMessageBox::Icon::Critical,
				"Error", msg.c_str(), QMessageBox::StandardButton::Ok);
			box.exec();

			delete editor;
			continue;
		}

		if (create && robot_mgr_.exists(model.getProperty(RobotDialogName)->getValue().toString().toStdString()))
		{
			std::string msg("A robot with the name'");
			msg += model.getProperty("name")->getValue().toString().toStdString();
			msg += "' alread exists, please choose another name";
			QMessageBox box(QMessageBox::Icon::Critical,
				"Error", msg.c_str(), QMessageBox::StandardButton::Ok);
			box.exec();

			delete editor;
			continue;
		}

		if (create)
			robot = std::make_shared<RobotParams>(model.getProperty(RobotDialogName)->getValue().toString().toStdString());

		robot->setEffectiveWidth(ewidth);
		robot->setEffectiveLength(elength);
		robot->setRobotWidth(rwidth);
		robot->setRobotLength(rlength);
		robot->setMaxVelocity(velocity);
		robot->setMaxAcceleration(accel);
		robot->setMaxJerk(jerk);
		robot->setTimestep(timestep);
		robot->setDriveType(drivetype);

		if (create)
		{
			if (!robot_mgr_.add(robot))
			{
				QMessageBox box(QMessageBox::Icon::Critical,
					"Error", "Cannot save robot to robot direcotry",
					QMessageBox::StandardButton::Ok);
				return;
			}
			QAction* newRobotAction = new QAction(robot->getName().c_str());
			robots_group_->addAction(newRobotAction);
			robots_->insertAction(robot_seperator_, newRobotAction);
			newRobotAction->setCheckable(true);
			(void)connect(newRobotAction, &QAction::triggered, this, [this, robot] { newRobotSelected(robot); });
			setRobot(robot->getName());
		}
		else
		{
			if (!robot_mgr_.save(robot))
			{
				QMessageBox box(QMessageBox::Icon::Critical,
					"Error", "Cannot save robot to robot direcotry",
					QMessageBox::StandardButton::Ok);
				return;
			}
		}
		delete editor;
		break;
	}
}

void XeroPathGenerator::deletePathOrGroup()
{
	if (paths_->isGroupSelected())
	{
		paths_model_.deleteGroup(paths_->getSelectedGroup()->getName());
	}
	else if (paths_->isPathSelected())
	{
		auto path = paths_->getSelectedPath();
		paths_model_.deletePath(path->getParent()->getName(), path->getName());
	}
}

void XeroPathGenerator::addPathGroupAction()
{
	paths_model_.addPathGroup();
}

void XeroPathGenerator::addRobotPathAction()
{
	if (paths_->isGroupSelected())
	{
		auto group = paths_->getSelectedGroup();
		if (group != nullptr)
		{
			paths_model_.addNewPath(group);
		}
	}
}

void XeroPathGenerator::pathAdded(std::shared_ptr<RobotPath> path)
{
	paths_->expandAll();
	setPathDirty(path);
	setPath(path);
}

void XeroPathGenerator::groupAdded(std::shared_ptr<PathGroup> group)
{
  	(void)group ;
	paths_->expandAll();
}

void XeroPathGenerator::pathDeleted()
{
	paths_->expandAll();
}

void XeroPathGenerator::groupDeleted()
{
	paths_->expandAll();
}

void XeroPathGenerator::editPreferences()
{
	PropertyEditor dialog("Edit Preferences");
	std::shared_ptr<EditableProperty> prop;

	prop = std::make_shared<EditableProperty>(PrefDialogUnits, EditableProperty::PTStringList,
		QVariant(units_.c_str()), "The units of measurement for the robot, can differ from the paths");
	auto list = UnitConverter::getAllUnits();
	for (auto& unit : list)
		prop->addChoice(unit.c_str());
	dialog.getModel().addProperty(prop);

	QString value = JsonOutputType;
	if (output_type_ == OutputType::OutputCSV)
		value = CSVOutputType;

	prop = std::make_shared<EditableProperty>(PrefDialogOutputFormat, EditableProperty::PTStringList,
		QVariant(value), "The format for trajectory output");
	prop->addChoice(JsonOutputType);
	prop->addChoice(CSVOutputType);
	dialog.getModel().addProperty(prop);

	prop = std::make_shared<EditableProperty>(PrefDialogNTServer, EditableProperty::PTString,
		QVariant(ntserver_.c_str()), "The IP address of the Network Table server");
	dialog.getModel().addProperty(prop);

	prop = std::make_shared<EditableProperty>(PrefDialogNTTableName, EditableProperty::PTString,
		QVariant(nttable_name_.c_str()), "The name of the network table for path publishing");
	dialog.getModel().addProperty(prop);

	prop = std::make_shared<EditableProperty>(GridEnabled, EditableProperty::PTStringList,
		QVariant(path_view_->isGridEnabled() ? "true" : "false"), "If true, the grid is drawn on the field");
	prop->addChoice("true");
	prop->addChoice("false");
	dialog.getModel().addProperty(prop);

	prop = std::make_shared<EditableProperty>(GridComplete, EditableProperty::PTStringList,
		QVariant(path_view_->isCompleteGrid() ? "true" : "false"), "If the grid is drawn, and this is true, a mesh grid is drawn");
	prop->addChoice("true");
	prop->addChoice("false");
	dialog.getModel().addProperty(prop);

	prop = std::make_shared<EditableProperty>(ShowEquation, EditableProperty::PTStringList,
		QVariant(path_view_->isShowEquation() ? "true" : "false"), "If true, the Spline equations are displayed");
	prop->addChoice("true");
	prop->addChoice("false");
	dialog.getModel().addProperty(prop);

	prop = std::make_shared<EditableProperty>(EquationsStacked, EditableProperty::PTStringList,
		QVariant(path_view_->areEquationsStacked() ? "true" : "false"), "If true and equations are shown, they are stacked x then y");
	prop->addChoice("true");
	prop->addChoice("false");
	dialog.getModel().addProperty(prop);

	if (dialog.exec() == QDialog::Rejected)
		return;

	std::string units = dialog.getModel().getProperty(PrefDialogUnits)->getValue().toString().toStdString();
	setUnits(units);
	settings_.setValue(UnitsSetting, units.c_str());

	ntserver_ = dialog.getModel().getProperty(PrefDialogNTServer)->getValue().toString().toStdString();
	auto inst = nt::NetworkTableInstance::GetDefault();
	inst.StopClient();
	inst.StartClient(ntserver_.c_str());
	settings_.setValue(NTServerIPAddress, ntserver_.c_str());

	nttable_name_ = dialog.getModel().getProperty(PrefDialogNTTableName)->getValue().toString().toStdString();

	value = dialog.getModel().getProperty(PrefDialogOutputFormat)->getValue().toString();
	if (value == JsonOutputType)
	{
		output_type_ = OutputType::OutputJSON;
		settings_.setValue(OutputTypeSetting, value);
	}
	else if (value == CSVOutputType)
	{
		output_type_ = OutputType::OutputCSV;
		settings_.setValue(OutputTypeSetting, value);
	}
	else
	{
		settings_.remove(OutputTypeSetting);
	}

	value = dialog.getModel().getProperty(GridEnabled)->getValue().toString();
	if (value == "true")
	{
		settings_.setValue(GridEnabled, true);
		path_view_->enableGrid(true);
	}
	else if (value == "false")
	{
		settings_.setValue(GridEnabled, false);
		path_view_->enableGrid(false);
	}

	value = dialog.getModel().getProperty(GridComplete)->getValue().toString();
	if (value == "true")
	{
		settings_.setValue(GridComplete, true);
		path_view_->completeGrid(true);
	}
	else if (value == "false")
	{
		settings_.setValue(GridComplete, false);
		path_view_->completeGrid(false);
	}

	value = dialog.getModel().getProperty(ShowEquation)->getValue().toString();
	if (value == "true")
	{
		settings_.setValue(ShowEquation, true);
		path_view_->showEquation(true);
	}
	else if (value == "false")
	{
		settings_.setValue(ShowEquation, false);
		path_view_->showEquation(false);
	}

	value = dialog.getModel().getProperty(EquationsStacked)->getValue().toString();
	if (value == "true")
	{
		settings_.setValue(EquationsStacked, true);
		path_view_->equationsStacked(true);
	}
	else if (value == "false")
	{
		settings_.setValue(EquationsStacked, false);
		path_view_->equationsStacked(false);
	}

	path_view_->repaint();
}

void XeroPathGenerator::initRecentFiles()
{
	recents_ = new RecentFiles(settings_, *recent_menu_);
	recents_->initialize(this);
}

void XeroPathGenerator::initPlotVars()
{
	if (current_robot_ == nullptr)
		return;

	QString rtype;
	if (current_robot_->getDriveType() == RobotParams::DriveType::TankDrive)
		rtype = "plotvars/tank";
	else
		rtype = "plotvars/swerve";

	if (settings_.contains(rtype))
	{
		QVariant v = settings_.value(rtype);
		QList<QVariant> vl = v.toList();
		QList<QString> items;
		for (QVariant q : vl)
		{
			items.push_back(q.toString());
		}
		plots_->addPlotVariables(items);
	}
	else
	{
		plots_->addPlotVariable("main", PathPlotView::VarType::VTPosition);
		plots_->addPlotVariable("main", PathPlotView::VarType::VTVelocity);
		plots_->addPlotVariable("main", PathPlotView::VarType::VTAcceleration);
	}
}

void XeroPathGenerator::savePlotVars()
{
	if (current_robot_ == nullptr)
		return;

	QString rtype;
	if (current_robot_->getDriveType() == RobotParams::DriveType::TankDrive)
		rtype = "plotvars/tank";
	else
		rtype = "plotvars/swerve";

	QList<QVariant> vl;
	auto list = plots_->getPlotVariables();
	for (auto item : list)
	{
		vl.push_back(QVariant(item));
	}
	settings_.setValue(rtype, vl);
}

void XeroPathGenerator::constraintAddedRemoved()
{
	paths_model_.setDirty();
	currentPathChanged();
	setXeroWindowTitle();
}

void XeroPathGenerator::scrollBarChanged()
{
	double stime = plot_main_->getTime();
	path_view_->setCursorTime(stime);
	plots_->setCursorTime(stime);
	if (traj_window_ != nullptr)
		traj_window_->setCurrentTime(stime);

	if (current_path_ != nullptr)
	{
		double dist;
		QString timedist = "time " + QString::number(stime, 'f', 2);
		
		if (current_path_->getDistance(stime, dist))
		{
			timedist += ", dist " + QString::number(dist, 'f', 2);
			time_dist_->setText(timedist);
		}
	}
	else
	{
		time_dist_->setText("");
	}
}

void XeroPathGenerator::currentPathChanged()
{
	setPathDirty(current_path_);
	setXeroWindowTitle();
	plot_main_->setPath(current_path_);
}

void XeroPathGenerator::updateWaypoint(const std::string &path, const std::string &group, size_t index, const xero::paths::Pose2d& pt)
{
	auto p = paths_model_.findPathByName(path, group);
	if (p != nullptr)
	{
		p->replacePoint(index, pt);
		path_view_->repaint();

		if (current_path_ == p && path_view_->getSelected() == index)
			waypoint_model_.setWaypoint(current_path_->getPoints()[index], index, current_path_->getDistance(index));
	}
}
