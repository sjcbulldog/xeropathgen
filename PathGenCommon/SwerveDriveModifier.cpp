#include "SwerveDriveModifier.h"
#include "MathUtils.h"
#include "TrapezoidalProfile.h"
#include "TrajectoryNames.h"
#include <cmath>
#include <fstream>

namespace xero
{
	namespace paths
	{
		SwerveDriveModifier::SwerveDriveModifier()
		{
		}

		SwerveDriveModifier::~SwerveDriveModifier()
		{
		}

		bool SwerveDriveModifier::modify(const RobotParams& robot, std::shared_ptr<RobotPath> path, const std::string &units)
		{
			(void)units;

			std::shared_ptr<PathTrajectory> main = path->getTrajectory(TrajectoryName::Main);
			if (main == nullptr)
				return false;

			double duration = (*main)[main->size() - 1].time();
			double rtime = duration - path->getStartAngleDelay() - path->getEndAngleDelay();
			auto tp = createRotationProfile(robot, path);

			//
			// Cannot rotate the robot from start angle to end angle in the time given for the
			// translation of the robot from the start position to the end position.
			//
			if (tp->getTotalTime() > rtime)
				return false;

			std::vector<Pose2dWithTrajectory> flpts;
			std::vector<Pose2dWithTrajectory> frpts;
			std::vector<Pose2dWithTrajectory> blpts;
			std::vector<Pose2dWithTrajectory> brpts;

			bool first = true;
			Translation2d prevfl, prevfr, prevbl, prevbr;
			double fldist = 0, frdist = 0, bldist = 0, brdist = 0;

			for (size_t i = 0; i < main->size(); i++)
			{
				const Pose2dWithTrajectory& pt = (*main)[i];
				double time = pt.time();
				Rotation2d angle = Rotation2d::fromDegrees(MathUtils::boundDegrees(path->getStartAngle() + tp->getDistance(time - path->getStartAngleDelay())));

				//
				// This is the linear velocity needed to rotate the robot per the
				// rotational speed profile.  This needs to be combined with the
				// translational velocity to set the final velocity for each wheel
				//
				double rv = RotationalToGround(robot, tp->getVelocity(time - path->getStartAngleDelay()));

				//
				// This is the linear acceleration needed to rotate the robot per the
				// rotational speed profile.  This needs to be combined with the
				// translational acceleration to set the final acceleration for each wheel.
				//
				double ra = RotationalToGround(robot, tp->getAccel(time - path->getStartAngleDelay()));

				Translation2d rotflvel = GetWheelPerpendicularVector(robot, Wheel::FL, rv).rotateBy(angle);
				Translation2d rotfrvel = GetWheelPerpendicularVector(robot, Wheel::FR, rv).rotateBy(angle);
				Translation2d rotblvel = GetWheelPerpendicularVector(robot, Wheel::BL, rv).rotateBy(angle);
				Translation2d rotbrvel = GetWheelPerpendicularVector(robot, Wheel::BR, rv).rotateBy(angle);

				Translation2d rotflacc = GetWheelPerpendicularVector(robot, Wheel::FL, ra).rotateBy(angle);
				Translation2d rotfracc = GetWheelPerpendicularVector(robot, Wheel::FR, ra).rotateBy(angle);
				Translation2d rotblacc = GetWheelPerpendicularVector(robot, Wheel::BL, ra).rotateBy(angle);
				Translation2d rotbracc = GetWheelPerpendicularVector(robot, Wheel::BR, ra).rotateBy(angle);

				Rotation2d heading = pt.rotation();
				Translation2d pathvel = Translation2d(heading, pt.velocity()).rotateBy(Rotation2d::fromDegrees(-angle.toDegrees()));
				Translation2d pathacc = Translation2d(heading, pt.acceleration()).rotateBy(Rotation2d::fromDegrees(-angle.toDegrees()));

				Translation2d flv = rotflvel + pathvel;
				Translation2d frv = rotfrvel + pathvel;
				Translation2d blv = rotblvel + pathvel;
				Translation2d brv = rotbrvel + pathvel;

				Translation2d fla = rotflacc + pathacc;
				Translation2d fra = rotfracc + pathacc;
				Translation2d bla = rotblacc + pathacc;
				Translation2d bra = rotbracc + pathacc;

				Translation2d flpos = Translation2d(robot.getLength() / 2.0, robot.getWidth() / 2.0).rotateBy(angle).translateBy(pt.translation());
				Translation2d frpos = Translation2d(robot.getLength() / 2.0, -robot.getWidth() / 2.0).rotateBy(angle).translateBy(pt.translation());
				Translation2d blpos = Translation2d(-robot.getLength() / 2.0, robot.getWidth() / 2.0).rotateBy(angle).translateBy(pt.translation());
				Translation2d brpos = Translation2d(-robot.getLength() / 2.0, -robot.getWidth() / 2.0).rotateBy(angle).translateBy(pt.translation());

				if (!first)
				{
					fldist += flpos.distance(prevfl);
					frdist += frpos.distance(prevfr);
					bldist += blpos.distance(prevbl);
					brdist += brpos.distance(prevbr);
				}
				else
				{
					first = false;
				}

				Pose2d flpose(flpos, flv.toRotation());
				Pose2dWithTrajectory fltraj(flpose, time, fldist, flv.normalize(), fla.normalize(), 0.0);
				flpts.push_back(fltraj);

				Pose2d frpose(frpos, frv.toRotation());
				Pose2dWithTrajectory frtraj(frpose, time, frdist, frv.normalize(), fra.normalize(), 0.0);
				frpts.push_back(frtraj);

				Pose2d blpose(blpos, blv.toRotation());
				Pose2dWithTrajectory bltraj(blpose, time, bldist, blv.normalize(), bla.normalize(), 0.0);
				blpts.push_back(bltraj);

				Pose2d brpose(brpos, brv.toRotation());
				Pose2dWithTrajectory brtraj(brpose, time, brdist, brv.normalize(), bra.normalize(), 0.0);
				brpts.push_back(brtraj);

				prevfl = flpos;
				prevfr = frpos;
				prevbl = blpos;
				prevbr = brpos;
			}

			std::shared_ptr<PathTrajectory> fl = std::make_shared<PathTrajectory>(TrajectoryName::FL, flpts);
			std::shared_ptr<PathTrajectory> fr = std::make_shared<PathTrajectory>(TrajectoryName::FR, frpts);
			std::shared_ptr<PathTrajectory> bl = std::make_shared<PathTrajectory>(TrajectoryName::BL, blpts);
			std::shared_ptr<PathTrajectory> br = std::make_shared<PathTrajectory>(TrajectoryName::BR, brpts);

			path->addTrajectory(fl);
			path->addTrajectory(fr);
			path->addTrajectory(bl);
			path->addTrajectory(br);

			return true;
		}

		Translation2d SwerveDriveModifier::GetWheelPerpendicularVector(const RobotParams &robot, Wheel w, double magnitude)
		{
			double dx = 0.0, dy = 0.0;

			switch (w)
			{
			case Wheel::FL:
				dx = robot.getLength() / 2.0;
				dy = robot.getWidth() / 2.0;
				break;

			case Wheel::FR:
				dx = robot.getLength() / 2.0;
				dy = -robot.getWidth() / 2.0;
				break;

			case Wheel::BL:
				dx = -robot.getLength() / 2.0;
				dy = robot.getWidth() / 2.0;
				break;

			case Wheel::BR:
				dx = -robot.getLength() / 2.0;
				dy = -robot.getWidth() / 2.0;
				break;
			}

			double dist = std::sqrt(dx * dx + dy * dy);
			Translation2d pt(-dy / dist * magnitude, dx / dist * magnitude);
			return pt;
		}

		std::shared_ptr<TrapezoidalProfile> SwerveDriveModifier::createRotationProfile(const RobotParams& params, std::shared_ptr<RobotPath> path)
		{
			double accel = GroundToRotational(params, path->getMaxAccel() * percent_);
			double maxvel = GroundToRotational(params, path->getMaxVelocity() * percent_);

			double diff = path->getEndAngle() - path->getStartAngle();
			if (diff > 180.0)
				diff -= 360.0;
			else if (diff <= -180.0)
				diff += 360.0;

			std::shared_ptr<TrapezoidalProfile> tp = std::make_shared<TrapezoidalProfile>(accel, -accel, maxvel);
			tp->update(diff, 0.0, 0.0);
			return tp;
		}

		double SwerveDriveModifier::RotationalToGround(const RobotParams robot, double rv)
		{
			double diameter = std::sqrt(robot.getWidth() * robot.getWidth() +robot.getLength() * robot.getLength());
			double circum = diameter * MathUtils::kPI;
			return rv * circum / 360.0;
		}

		double SwerveDriveModifier::GroundToRotational(RobotParams robot, double gr)
		{
			double diameter = std::sqrt(robot.getWidth() * robot.getWidth() + robot.getLength() * robot.getLength());
			double circum = diameter * MathUtils::kPI;
			return gr * 360.0 / circum;
		}
	}
}
