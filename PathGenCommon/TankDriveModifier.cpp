#include "TankDriveModifier.h"
#include "TrajectoryNames.h"
#include "Pose2DWithCurvature.h"
#include <cmath>

namespace xero
{
	namespace paths
	{
		TankDriveModifier::TankDriveModifier()
		{
		}

		TankDriveModifier::~TankDriveModifier()
		{
		}

		bool TankDriveModifier::modify(const RobotParams& robot, std::shared_ptr<RobotPath> path, const std::string &units)
		{
			std::shared_ptr<PathTrajectory> main = path->getTrajectory(TrajectoryName::Main);
			if (main == nullptr)
				return false;

			//
			// Get the width of the robot in the same units used by the paths
			//
			double width = UnitConverter::convert(robot.getEffectiveWidth(), robot.getLengthUnits(), units);

			std::vector<Pose2dWithTrajectory> leftpts;
			std::vector<Pose2dWithTrajectory> rightpts;

			double lcurv = 0, rcurv = 0;
			double lvel = 0, lacc = 0, lpos = 0, ljerk = 0;
			double rvel = 0, racc = 0, rpos = 0, rjerk = 0;
			double plx = 0, ply = 0, prx = 0, pry = 0;
			double plvel = 0, prvel = 0;
			double placc = 0, pracc = 0;

			for (size_t i = 0; i < main->size(); i++)
			{
				const Pose2dWithTrajectory& pt = (*main)[i];
				double time = pt.time();
				double px = pt.x();
				double py = pt.y();

				double lx = px - width * pt.rotation().getSin() / 2.0;
				double ly = py + width * pt.rotation().getCos() / 2.0;
				double rx = px + width * pt.rotation().getSin() / 2.0;
				double ry = py - width * pt.rotation().getCos() / 2.0;

				if (i == 0)
				{
					lvel = 0.0;
					lacc = 0.0;
					lpos = 0.0;
					ljerk = 0.0;

					rvel = 0.0;
					racc = 0.0;
					rpos = 0.0;
					rjerk = 0.0;
				}
				else
				{
					double dt = time - (*main)[i - 1].time();
					double ldist = std::sqrt((lx - plx) * (lx - plx) + (ly - ply) * (ly - ply));
					double rdist = std::sqrt((rx - prx) * (rx - prx) + (ry - pry) * (ry - pry));

					lvel = ldist / dt;
					rvel = rdist / dt;

					lacc = (lvel - plvel) / dt;
					racc = (rvel - prvel) / dt;

					ljerk = (lacc - placc) / dt;
					rjerk = (racc - pracc) / dt;

					lpos += ldist;
					rpos += rdist;
				}

				Translation2d lpt(lx, ly);
				Pose2d l2d(lpt, pt.rotation());
				Pose2dWithTrajectory ltraj(l2d, time, lpos, lvel, lacc, ljerk, lcurv, 0.0);
				leftpts.push_back(ltraj);

				Translation2d rpt(rx, ry);
				Pose2d r2d(rpt, pt.rotation());
				Pose2dWithTrajectory rtraj(r2d, time, rpos, rvel, racc, rjerk, rcurv, 0.0);
				rightpts.push_back(rtraj);

				plx = lx;
				ply = ly;
				prx = rx;
				pry = ry;
				plvel = lvel;
				prvel = rvel;
				placc = lacc;
				pracc = racc;
			}

			assert(leftpts.size() == rightpts.size());
			for (int i = 0; i < leftpts.size(); i++) {
				if (i == 0 || i == leftpts.size() - 1)
				{
					lcurv = 0.0;
					rcurv = 0.0;
				}
				else
				{
					leftpts[i].setCurvature(xero::paths::Pose2dWithCurvature::curvature(leftpts[i - 1].pose(), leftpts[i].pose(), leftpts[i + 1].pose()));
					rightpts[i].setCurvature(xero::paths::Pose2dWithCurvature::curvature(rightpts[i - 1].pose(), rightpts[i].pose(), rightpts[i + 1].pose()));
				}
			}

			std::shared_ptr<PathTrajectory> left = std::make_shared<PathTrajectory>(TrajectoryName::Left, leftpts);
			std::shared_ptr<PathTrajectory> right = std::make_shared<PathTrajectory>(TrajectoryName::Right, rightpts);

			path->addTrajectory(left);
			path->addTrajectory(right);

			return true;
		}
	}
}