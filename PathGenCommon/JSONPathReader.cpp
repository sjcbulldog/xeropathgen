#include "JSONPathReader.h"
#include "JSONPathReader.h"
#include "JSON.h"
#include "JSONValue.h"
#include "DistanceVelocityConstraint.h"
#include <fstream>
#include <iostream>
#include <cmath>

namespace xero
{
	namespace paths
	{
		std::wstring JSONPathReader::toWide(const std::string& str)
		{
			std::wstring result;

			for (char ch : str)
				result += (wchar_t)ch;

			return result;
		}

		std::string JSONPathReader::toNarrow(const std::wstring& str)
		{
			std::string result;

			for (wchar_t ch : str)
				result += (char)ch;

			return result;
		}

		bool JSONPathReader::readFile(const std::string& filename, std::wstring& data)
		{
			std::string line;

			std::ifstream strm(filename);
			if (!strm.is_open()) 
			{
				std::cerr << "pathgen: cannot open path file '" << filename << "'' for reading" << std::endl;
				return false;
			}

			while (getline(strm, line))
			{
				data += toWide(line);
				data += (wchar_t)'\n';
			}

			return true;
		}

		bool JSONPathReader::readJSONRobotFile(const std::string& filename, RobotParams& robot)
		{
			std::wstring data;

			if (!readFile(filename, data))
				return false;

			JSONValue* value = JSON::Parse(data.c_str());
			if (value == nullptr)
			{
				std::cerr << "pathgen: cannot parse file '" << filename << "' as a robot file" << std::endl;
				return false;
			}

			if (!value->IsObject())
			{
				std::cerr << "pathgen: robot file '" << filename << "' is not a valid robot file" << std::endl;
				std::cerr << "         top level entity in file is not a JSON object" << std::endl;
				return false;
			}

			JSONObject root = value->AsObject();

			if (root.find(RobotParams::NameTagW) == root.end())
			{
				std::cerr << "pathgen: robot file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         group object does not contain 'name' field" << std::endl;
				return false;
			}

			if (!root[RobotParams::NameTagW]->IsString())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid robot file" << std::endl;
				std::cerr << "         group object 'name' field is not a string" << std::endl;
				return false;
			}
			robot.setName(toNarrow(root[RobotParams::NameTagW]->AsString()));

			if (root.find(RobotParams::DriveTypeTagW) == root.end())
			{
				std::cerr << "pathgen: robot file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         group object does not contain 'drivetype' field" << std::endl;
				return false;
			}

			if (!root[RobotParams::DriveTypeTagW]->IsNumber())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid robot file" << std::endl;
				std::cerr << "         group object 'drivetype' field is not a number" << std::endl;
				return false;
			}
			int dt = static_cast<int>(root[RobotParams::DriveTypeTagW]->AsNumber());
			robot.setDriveType(static_cast<RobotParams::DriveType>(dt));

			if (root.find(RobotParams::TimeStepTagW) == root.end())
			{
				std::cerr << "pathgen: robot file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         group object does not contain 'timestep' field" << std::endl;
				return false;
			}

			if (!root[RobotParams::TimeStepTagW]->IsNumber())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid robot file" << std::endl;
				std::cerr << "         group object 'timestep' field is not a number" << std::endl;
				return false;
			}
			robot.setTimestep(root[RobotParams::TimeStepTagW]->AsNumber());

			if (root.find(RobotParams::EffectiveLengthTagW) == root.end())
			{
				std::cerr << "pathgen: robot file '" << filename << "' is not a valid robot file" << std::endl;
				std::cerr << "         group object does not contain 'efflength' field" << std::endl;
				return false;
			}

			if (!root[RobotParams::EffectiveLengthTagW]->IsNumber())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid robot file" << std::endl;
				std::cerr << "         group object 'efflength' field is not a number" << std::endl;
				return false;
			}
			robot.setEffectiveLength(root[RobotParams::EffectiveLengthTagW]->AsNumber());

			if (root.find(RobotParams::EffectiveWidthTagW) == root.end())
			{
				std::cerr << "pathgen: robot file '" << filename << "' is not a valid robot file" << std::endl;
				std::cerr << "         group object does not contain 'effwidth' field" << std::endl;
				return false;
			}

			if (!root[RobotParams::EffectiveWidthTagW]->IsNumber())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid robot file" << std::endl;
				std::cerr << "         group object 'effwidth' field is not a number" << std::endl;
				return false;
			}
			robot.setEffectiveWidth(root[RobotParams::EffectiveWidthTagW]->AsNumber());

			if (root.find(RobotParams::RobotLengthTagW) == root.end())
			{
				std::cerr << "pathgen: robot file '" << filename << "' is not a valid robot file" << std::endl;
				std::cerr << "         group object does not contain 'robotlength' field" << std::endl;
				return false;
			}

			if (!root[RobotParams::RobotLengthTagW]->IsNumber())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid robot file" << std::endl;
				std::cerr << "         group object 'robotlength' field is not a number" << std::endl;
				return false;
			}
			robot.setRobotLength(root[RobotParams::RobotLengthTagW]->AsNumber());

			if (root.find(RobotParams::RobotWidthTagW) == root.end())
			{
				std::cerr << "pathgen: robot file '" << filename << "' is not a valid robot file" << std::endl;
				std::cerr << "         group object does not contain 'robotwidth' field" << std::endl;
				return false;
			}

			if (!root[RobotParams::RobotWidthTagW]->IsNumber())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid robot file" << std::endl;
				std::cerr << "         group object 'robotwidth' field is not a number" << std::endl;
				return false;
			}
			robot.setRobotWidth(root[RobotParams::RobotWidthTagW]->AsNumber());

			if (root.find(RobotParams::MaxVelocityTagW) == root.end())
			{
				std::cerr << "pathgen: robot file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         group object does not contain 'maxvelocity' field" << std::endl;
				return false;
			}

			if (!root[RobotParams::MaxVelocityTagW]->IsNumber())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid robot file" << std::endl;
				std::cerr << "         group object 'maxvelocity' field is not a number" << std::endl;
				return false;
			}
			robot.setMaxVelocity(root[RobotParams::MaxVelocityTagW]->AsNumber());

			if (root.find(RobotParams::MaxAccelerationTagW) == root.end())
			{
				std::cerr << "pathgen: robot file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         group object does not contain 'maxacceleration' field" << std::endl;
				return false;
			}

			if (!root[RobotParams::MaxAccelerationTagW]->IsNumber())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid robot file" << std::endl;
				std::cerr << "         group object 'maxacceleration' field is not a number" << std::endl;
				return false;
			}
			robot.setMaxAcceleration(root[RobotParams::MaxAccelerationTagW]->AsNumber());

			if (root.find(RobotParams::MaxJerkTagW) == root.end())
			{
				std::cerr << "pathgen: robot file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         group object does not contain 'maxjerk' field" << std::endl;
				return false;
			}

			if (!root[RobotParams::MaxJerkTagW]->IsNumber())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid robot file" << std::endl;
				std::cerr << "         group object 'maxjerk' field is not a number" << std::endl;
				return false;
			}
			robot.setMaxJerk(root[RobotParams::MaxJerkTagW]->AsNumber());
			return true;
		}

		bool JSONPathReader::readJSONPathFile(const std::string& filename, const RobotParams& robot, PathCollection& paths)
		{
			std::wstring data;
			bool ret = true;

			if (!readFile(filename, data))
				return false;

			JSONValue* value = JSON::Parse(data.c_str());
			if (value == nullptr) 
			{
				std::cerr << "pathgen: cannot parse file '" << filename << "' as a JSON file" << std::endl;
				return false;
			}

			if (!value->IsObject()) 
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         top level entity in file is not a JSON object" << std::endl;
				return false;
			}

			JSONObject root = value->AsObject();

			if (root.find(RobotPath::GroupsTagW) == root.end())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         top level object in file does not contains the entry 'groups'" << std::endl;
				return false;
			}

			if (!root[RobotPath::GroupsTagW]->IsArray())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         the 'groups' entry in top level object is not an JSON array" << std::endl;
				return false;
			}

			JSONArray groups = root[RobotPath::GroupsTagW]->AsArray();
			for (size_t i = 0; i < groups.size(); i++)
			{
				if (!parseGroup(filename, robot, paths, groups[i]))
					ret = false;
			}

			return ret;
		}

		bool JSONPathReader::parseGroup(const std::string& filename, const RobotParams& robot, PathCollection& paths, JSONValue* obj)
		{
			std::string name;
			bool ret = true;

			if (!obj->IsObject())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         an element of the 'groups' entry is not an object" << std::endl;
				return false;
			}

			JSONObject group = obj->AsObject();

			if (group.find(RobotPath::NameTagW) == group.end())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         group object does not contain 'name' field" << std::endl;
				return false;
			}

			if (!group[RobotPath::NameTagW]->IsString())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         group object 'name' field is not a string" << std::endl;
				return false;
			}

			name = toNarrow(group[RobotPath::NameTagW]->AsString());

			if (group.find(RobotPath::PathsTagW)== group.end())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         group object does not contain 'paths' field" << std::endl;
				return false;
			}

			if (!group[RobotPath::PathsTagW]->IsArray())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         group object 'paths' field is not an array" << std::endl;
				return false;
			}

			JSONArray parray = group[RobotPath::PathsTagW]->AsArray();
			for (size_t i = 0; i < parray.size(); i++)
			{
				JSONValue* path = parray[i];
				if (!parsePath(filename, robot, paths, name, path))
					ret = false;
			}

			return ret;
		}

		bool JSONPathReader::parsePath(const std::string& filename, const RobotParams& robot, PathCollection& paths, const std::string& group, JSONValue* obj)
		{
			std::string name;
			std::vector<Pose2d> points;
			std::shared_ptr<RobotPath> onepath;

			if (!obj->IsObject())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         an element of the 'paths' entry in a 'group' is not an object" << std::endl;
				return false;
			}

			JSONObject path = obj->AsObject();

			if (path.find(RobotPath::NameTagW) == path.end())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         path object does not contain 'name' field" << std::endl;
				return false;
			}
			if (!path[RobotPath::NameTagW]->IsString())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         path object 'name' field is not a string" << std::endl;
				return false;
			}

			name = toNarrow(path[RobotPath::NameTagW]->AsString());
			onepath = paths.addPath(group, name);

			if (path.find(RobotPath::StartVelocityTagW) == path.end())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         path '" << name << "' is missing 'startvelocity' field" << std::endl;
				return false;
			}

			if (!path[RobotPath::StartVelocityTagW]->IsNumber())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         path has 'startvelocity' field that is not a number" << std::endl;
				return false;
			}
			onepath->setStartVelocity(path[RobotPath::StartVelocityTagW]->AsNumber());

			if (path.find(RobotPath::EndVelocityTagW) == path.end())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         path is missing 'endvelocity' field" << std::endl;
				return false;
			}

			if (!path[RobotPath::EndVelocityTagW]->IsNumber())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         path has 'endvelocity' field that is not a number" << std::endl;
				return false;
			}
			onepath->setEndVelocity(path[RobotPath::EndVelocityTagW]->AsNumber());

			if (path.find(RobotPath::MaxVelocityTagW) == path.end())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         path is missing 'maxvelocity' field" << std::endl;
				return false;
			}

			if (!path[RobotPath::MaxVelocityTagW]->IsNumber())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         path has 'maxvelocity' field that is not a number" << std::endl;
				return false;
			}
			double v = path[RobotPath::MaxVelocityTagW]->AsNumber();
			if (std::fabs(v) < 0.001 || v > robot.getMaxVelocity())
			{
				//
				// Zero max velocity was supplied, get it from the robot
				//
				v = robot.getMaxVelocity();
			}
			onepath->setMaxVelocity(v);

			if (path.find(RobotPath::MaxAccelerationTagW) == path.end())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         path is missing 'maxacceleration' field" << std::endl;
				return false;
			}

			if (!path[RobotPath::MaxAccelerationTagW]->IsNumber())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         path has 'maxacceleration' field that is not a number" << std::endl;
				return false;
			}
			v = path[RobotPath::MaxAccelerationTagW]->AsNumber();
			if (std::fabs(v) < 0.001 || v > robot.getMaxAccel()) 
			{
				//
				// Zero max velocity was supplied, get it from the robot
				//
				v = robot.getMaxAccel();
			}
			onepath->setMaxAccel(v);

			if (path.find(RobotPath::MaxJerkTagW) == path.end())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         path is missing 'maxjerk' field" << std::endl;
				return false;
			}

			if (!path[RobotPath::MaxJerkTagW]->IsNumber())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         path has 'maxjerk' field that is not a number" << std::endl;
				return false;
			}
			v = path[RobotPath::MaxJerkTagW]->AsNumber();
			if (std::fabs(v) < 0.001)
			{
				//
				// Zero max velocity was supplied, get it from the robot
				//
				v = robot.getMaxJerk();
			}
			onepath->setMaxJerk(v);

			if (path.find(RobotPath::ConstraintsTagW) != path.end() && !path[RobotPath::ConstraintsTagW]->IsNull())
			{
				if (!path[RobotPath::ConstraintsTagW]->IsArray())
				{
					std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
					std::cerr << "         path object 'constraints' field is not an array" << std::endl;
					return false;
				}

				JSONArray conarr = path[RobotPath::ConstraintsTagW]->AsArray();
				for (size_t i = 0; i < conarr.size(); i++)
				{
					JSONValue* conobj = conarr[i];
					if (!conobj->IsObject())
					{
						std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
						std::cerr << "         path object 'constraints' array element is not an object" << std::endl;
						return false;
					}

					JSONObject constr = conobj->AsObject();

					if (constr.find(RobotPath::TypeTagW) == constr.end())
					{
						std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
						std::cerr << "         path object constraint does not contain 'type' field" << std::endl;
						return false;
					}

					if (!constr[RobotPath::TypeTagW]->IsString())
					{
						std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
						std::cerr << "         path object constraint has 'type' field but it is not a string" << std::endl;
						return false;
					}

					std::string type = toNarrow(constr[RobotPath::TypeTagW]->AsString());

					if (type == RobotPath::DistanceVelocityTag)
					{
						double before, after;
						double velocity;

						if (constr.find(RobotPath::BeforeTagW) == constr.end())
						{
							std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
							std::cerr << "         constraint 'distance_velocity' missing 'before' field" << std::endl;
							return false;
						}

						if (!constr[RobotPath::BeforeTagW]->IsNumber())
						{
							std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
							std::cerr << "         constraint 'distance_velocity' has 'before' field that is not a number" << std::endl;
							return false;
						}
						before = constr[RobotPath::BeforeTagW]->AsNumber();

						if (constr.find(RobotPath::AfterTagW) == constr.end())
						{
							std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
							std::cerr << "         constraint 'distance_velocity' missing 'after' field" << std::endl;
							return false;
						}

						if (!constr[RobotPath::AfterTagW]->IsNumber())
						{
							std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
							std::cerr << "         constraint 'distance_velocity' has 'after' field that is not a number" << std::endl;
							return false;
						}
						after = constr[RobotPath::AfterTagW]->AsNumber();

						if (constr.find(RobotPath::VelocityTagW) == constr.end())
						{
							std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
							std::cerr << "         constraint 'distance_velocity' missing 'velocity' field" << std::endl;
							return false;
						}

						if (!constr[RobotPath::VelocityTagW]->IsNumber())
						{
							std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
							std::cerr << "         constraint 'distance_velocity' has 'velocity' field that is not a number" << std::endl;
							return false;
						}
						velocity = constr[RobotPath::VelocityTagW]->AsNumber();
						onepath->addTimingConstraint(std::make_shared<DistanceVelocityConstraint>(after, before, velocity));
					}
					else 
					{
						std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
						std::cerr << "         path object constraint with unknown type '" << type << "'" << std::endl;
						return false;
					}
				}
			}

			if (path.find(RobotPath::PointsTagW) == path.end())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         path does not contain 'points' field" << std::endl;
				return false;
			}

			if (!path[RobotPath::PointsTagW]->IsArray())
			{
				std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
				std::cerr << "         path contains 'point' field but it is not an array" << std::endl;
				return false;
			}

			JSONArray ptarr = path[RobotPath::PointsTagW]->AsArray();
			for (size_t i = 0; i < ptarr.size(); i++)
			{
				JSONValue* vt = ptarr[i];
				if (!vt->IsObject())
				{
					std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
					std::cerr << "         element in points array is not an object" << std::endl;
					return false;
				}

				JSONObject pt = vt->AsObject();
				double x, y;
				double heading;

				if (pt.find(RobotPath::XTagW) == pt.end())
				{
					std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
					std::cerr << "         point missing 'x' field" << std::endl;
					return false;
				}

				if (!pt[RobotPath::XTagW]->IsNumber())
				{
					std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
					std::cerr << "         point has 'x' field that is not a number" << std::endl;
					return false;
				}
				x = pt[RobotPath::XTagW]->AsNumber();

				if (pt.find(RobotPath::YTagW) == pt.end())
				{
					std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
					std::cerr << "         point missing 'y' field" << std::endl;
					return false;
				}

				if (!pt[RobotPath::YTagW]->IsNumber())
				{
					std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
					std::cerr << "         point has 'y' field that is not a number" << std::endl;
					return false;
				}
				y = pt[RobotPath::YTagW]->AsNumber();

				if (pt.find(RobotPath::HeadingTagW) == pt.end())
				{
					std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
					std::cerr << "         point missing 'heading' field" << std::endl;
					return false;
				}

				if (!pt[RobotPath::HeadingTagW]->IsNumber())
				{
					std::cerr << "pathgen: file '" << filename << "' is not a valid path file" << std::endl;
					std::cerr << "         point has 'heading' field that is not a number" << std::endl;
					return false;
				}
				heading = pt[RobotPath::HeadingTagW]->AsNumber();

				Rotation2d rot = Rotation2d::fromDegrees(heading);
				Translation2d trans(x, y);
				Pose2d pose(trans, rot);
				onepath->addPoint(pose);
			}
			return true;
		}
	}
}
