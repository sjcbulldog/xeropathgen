#pragma once
#pragma once

#include "GameFieldManager.h"
#include "GeneratorManager.h"
#include "RobotManager.h"
#include "PathFieldView.h"
#include "PathFileTreeModel.h"
#include "WaypointTreeModel.h"
#include "PathParamTreeModel.h"
#include "PathGenerationEngine.h"
#include "PathPlotView.h"
#include "RecentFiles.h"
#include "ConstraintTreeModel.h"
#include "ConstraintEditor.h"
#include "ShowRobotWindow.h"
#include "PlotMainWindow.h"
#include "PathFileTree.h"

#include <QProgressBar>
#include <QMainWindow>
#include <QSplitter>
#include <QMenuBar>
#include <QMenu>
#include <QGroupBox>
#include <QListView>
#include <QTreeView>
#include <QLabel>
#include <QTabWidget>
#include <QTextEdit>
#include <QChartView>
#include <QSettings>
#include <QCheckBox>
#include <QRadioButton>

#include <fstream>
#include <sstream>

class XeroPathGenerator : public QMainWindow
{
	friend class RecentFiles;

	Q_OBJECT

public:
	XeroPathGenerator(GameFieldManager &fields, GeneratorManager &generators, RobotManager &robots, std::ofstream &ostrm, std::stringstream &sstrm, QWidget *parent = Q_NULLPTR);
	virtual ~XeroPathGenerator();

private:
	void setFileName(QString name);
	void setXeroWindowTitle();

	bool createMenus();
	bool createWindows();
	bool createLeftSide();
	bool createRightSide();
	bool createToolbar();

	void populateFieldsMenu();
	void populateGeneratorsMenu();
	void populateRobotsMenu();

	void setDefaultField();
	void setDefaultGenerator();
	void setDefaultRobot();

	void setField(const std::string& field);
	void setRobot(const std::string& robot);
	void setGenerator(const std::string& generator);
	void setPath(std::shared_ptr<xero::paths::RobotPath> path);

	//
	// Setup related items
	//
	void newFieldSelected(std::shared_ptr<GameField> field);
	void newGeneratorSelected(std::shared_ptr<Generator> field);
	void newRobotSelected(std::shared_ptr<xero::paths::RobotParams> field);

	//
	// File menu actions
	//
	void fileNew();
	void fileOpen();
	void fileOpenWithName(QString filename);
	void fileSave();
	void fileSaveAs();
	void fileClose();
	void fileGenerate();
	void fileGenerateAs();
	void filePublish();
	void generate();
	void showFileMenu();

	//
	// Edit menu items
	//
	void addPathGroupAction();
	void addRobotPathAction();
	void deletePathOrGroup();
	void editPreferences();
	void showEditMenu();

	//
	// View menu
	//
	void viewPlotEdit();
	void viewTrajectoryData();

	//
	// Generator menu related
	//
	void editGeneratorParameters();

	//
	// Demo modes
	//
	void demoPathModeChanged();
	void nextOneInDemo();
	void setupDemoMode(bool active);

	//
	// Robot menu related
	//
	void newRobotAction();
	void editRobotAction();
	void showRobotMenu();
	void deleteRobotAction();

	//
	// Help menu related
	//
	void toggleDebugLogging();
	void toggleInfoLogging();
	void toggleWarningLogging();
	void showAbout();
	void showDocumentation();

	void pathTreeSelectionChanged(const QModelIndex& current, const QModelIndex& prev);

	void closeEvent(QCloseEvent* ev);
	void showEvent(QShowEvent* ev);

	void pathWindowMouseMoved(xero::paths::Translation2d pos);
	void scrollBarChanged();

private:
	static constexpr int timerTickMS = 100;

	//
	// Names of images
	//
	static constexpr const char* PlayButtonImage = "play.png";
	static constexpr const char* StopButtonImage = "stop.png";
	static constexpr const char* ForwardOneButtonImage = "forward1.png";
	static constexpr const char* ForwardTenButtonImage = "forward10.png";
	static constexpr const char* RobotButtonImage = "gear.png";


	//
	// Names for settings values
	//
	static constexpr const char* GeometrySetting = "geometry";
	static constexpr const char* WindowStateSetting = "windowState";
	static constexpr const char* LeftSplitterSetting = "leftSplitter";
	static constexpr const char* RightSplitterSetting = "rightSplitter";
	static constexpr const char* LeftRightSplitterSetting = "leftRightSplitter";
	static constexpr const char* UnitsSetting = "units";
	static constexpr const char* LastPathDirSetting = "lastpathdir";
	static constexpr const char* OutputTypeSetting = "outputtype";
	static constexpr const char* JsonOutputType = "JSON";
	static constexpr const char* CSVOutputType = "CSV";

	static const char* RobotDialogName;
	static const char* RobotDialogWidth;
	static const char* RobotDialogLength;
	static const char* RobotDialogMaxVelocity;
	static const char* RobotDialogMaxAcceleration;
	static const char* RobotDialogMaxJerk;
	static const char* RobotDialogUnits ;
	static const char* RobotDialogDriveType ;
	static const char* RobotDialogTimeStep ;

private:
	enum OutputType
	{
		OutputCSV,
		OutputJSON,
	};

	enum DemoMode
	{
		ModeNone,
		ModeFile,
		ModeGroup,
		ModePath
	};

private:
	void setUnits(const std::string& units);
	void insertWaypoint();
	void deleteWaypoint();

	void writeMessageTypes();
	void readMessageTypes();

	void readGeneratorParams(const std::string& name, GeneratorParameterStore& store);
	void writeGeneratorParams(const std::string& name, const GeneratorParameterStore& store);

	void insertedWaypointProc();
	void deletedWaypointProc();
	void movingWaypointProc(size_t index);
	void movedWaypointProc(size_t index);
	void selectedWaypointProc(size_t index);

	void demoRobot();
	void demoPlay();
	void demoStop();
	void demoForwardOne();
	void demoForwardTen();

	void waypointTreeDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
	void pathParamTreeDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
	void pathTreeDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
	
	bool save();

	void createEditRobot(std::shared_ptr<xero::paths::RobotParams> robot);
	static void messageLogger(QtMsgType type, const QMessageLogContext& context, const QString& msg);
	void messageLoggerWin(QtMsgType type, const QMessageLogContext& context, const QString& msg);
	void timerProc();

	void allPathsDirty();
	void initRecentFiles();
	void initPlotVars();
	void savePlotVars();

	void constraintAddedRemoved();
	void currentPathChanged();

private:
	QSettings settings_;

	QSplitter* m_left_right_;
	QSplitter* m_left_top_bottom_;
	QSplitter* m_right_stack_;

	QLabel* xpos_text_;
	QLabel* ypos_text_;
	QLabel* status_text_;
	QLabel* time_dist_;
	QProgressBar* prog_bar_;

	//
	// Left side
	//
	PathFieldView* path_view_;
	QTabWidget* plots_log_tab_;
	PathPlotView* plots_;
	PlotMainWindow* plot_main_;
	QTextEdit* log_;
	ConstraintEditor* constraints_;

	//
	// Right side
	//
	QGroupBox* paths_group_;
	PathFileTree* paths_;

	QGroupBox* path_parameters_group_;
	QTreeView* path_parameters_;

	QGroupBox* waypoint_parameters_group_;
	QTreeView* waypoint_parameters_;

	//
	// Menus
	//
	QMenu* file_;
	QAction* file_save_;
	QMenu* recent_menu_;
	QMenu* edit_;
	QAction* insert_waypoint_action_;
	QAction* delete_waypoint_action_;
	QAction* delete_path_or_group_action_;
	QAction* add_path_action_;
	QAction* add_group_action_;
	QMenu* view_;
	QAction* view_plot_action_;
	QAction* view_trajectory_data_;

	QMenu* games_;
	QActionGroup* games_group_;
	QMenu* generators_;
	QActionGroup* gen_group_;
	QMenu* robots_;
	QActionGroup* robots_group_;
	QAction* edit_robot_action_;
	QMenu* help_;
	QAction* robot_seperator_;
	QMenu* logmenu_;
	QAction* debug_action_;
	QAction* info_action_;
	QAction* warning_action_;

	//
	// Toolbar
	//
	QToolBar* toolbar_;
	QAction* play_action_;
	QAction* stop_action_;
	QAction* forward_one_action_;
	QAction* forward_ten_action_;
	QAction* show_robot_action_;
	QRadioButton* demo_mode_off_;
	QRadioButton* demo_mode_on_;
	ShowRobotWindow* robot_window_;

	//
	// The field, generator, and robot managers
	//
	GameFieldManager &fields_mgr_;
	GeneratorManager &generators_mgr_;
	RobotManager& robot_mgr_;
	PathGenerationEngine path_engine_;

	//
	// The currently selected field, generator, and robot
	//
	std::shared_ptr<GameField> current_field_;
	std::shared_ptr<Generator> current_generator_;

	std::shared_ptr<xero::paths::RobotParams> current_robot_;

	//
	// The set of loaded paths
	//
	PathFileTreeModel paths_model_;
	std::string path_file_name_;
	std::shared_ptr<xero::paths::RobotPath> current_path_;

	WaypointTreeModel waypoint_model_;
	PathParamTreeModel path_param_model_;

	std::string last_path_dir_;

	QTimer* path_timer_;

	//
	// The units we are using
	//
	std::string units_;

	bool write_messages_;
	std::ofstream& logstream_;
	std::stringstream& strstream_;

	std::list<QtMsgType> messages_;
	static XeroPathGenerator* theOne;

	RecentFiles* recents_;
	OutputType output_type_;

	DemoMode demo_mode_;
	size_t group_index_;
	size_t path_index_;
	size_t step_count_;

	std::thread::id gui_thread_;
	std::mutex log_messages_lock_;
	QList<QString> log_messages_;
};
