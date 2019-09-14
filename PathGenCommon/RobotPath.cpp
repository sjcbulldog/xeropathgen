#include "RobotPath.h"
#include "Rotation2d.h"
#include "SplinePair.h"
#include "TrajectoryNames.h"
#include <numeric>
#include <cmath>

namespace xero
{
	namespace paths
	{
		std::shared_ptr<SplinePair> RobotPath::getSplineAtTime(double time)
		{
			if (times_.size() == 0)
				return nullptr;

			if (times_.size() != points_.size())
				return nullptr;

			for (size_t i = 0; i < points_.size(); i++)
			{
				if (time < times_[i])
					return splines_[i - 1];
			}

			return nullptr;
		}

		double RobotPath::getTime()
		{
			auto traj = getTrajectory(TrajectoryName::Main);
			if (traj == nullptr || traj->size() == 0)
				return 0.0;

			return (*traj)[traj->size() - 1].time();
		}

		bool RobotPath::getDistance(double time, double &dist)
		{
			Pose2dWithTrajectory pt;

			if (!getPoseAtTime(time, pt))
				return false;

			dist = pt.position();
			return true;
		}

		bool RobotPath::getPoseAtTime(double time, Pose2dWithTrajectory& value)
		{
			auto traj = getTrajectory(TrajectoryName::Main);
			if (traj == nullptr || traj->size() == 0)
				return false;

			if (time > getTime())
			{
				value = (*traj)[traj->size() - 1];
				return true;
			}

			size_t low = 0;
			size_t high = traj->size() - 1;
			while (high - low > 1)
			{
				size_t mid = (high + low) / 2;
				double chktime = (*traj)[mid].time();
				if (time > chktime)
					low = mid;
				else
					high = mid;
			}

			const Pose2dWithTrajectory& lowpt = (*traj)[low];
			const Pose2dWithTrajectory& highpt = (*traj)[high];
			double percent = (time - lowpt.time()) / (highpt.time() - lowpt.time());
			value = lowpt.interpolate(highpt, percent);
			return true;
		}

		double RobotPath::getDistance()
		{
			int steps = 1000;

			if (!hasSplines())
				generateSplines();

			if (distances_.size() == 0)
			{
				for (size_t i = 0; i < splines_.size(); i++)
				{
					double dist = 0;
					auto pair = splines_[i];
					bool first = true;
					Translation2d pos, prevpos;
					for (float t = 0.0f; t <= 1.0f; t += 1.0f / steps)
					{
						pos = pair->evalPosition(t);
						if (first)
							first = false;
						else
						{
							dist += pos.distance(prevpos);
						}

						prevpos = pos;
					}

					distances_.push_back(dist);
				}
			}
			return std::accumulate(distances_.begin(), distances_.end(), 0.0);
		}

		void RobotPath::generateSplines()
		{
			splines_.clear();

			for (size_t i = 0; i < points_.size() - 1; i++)
			{
				const Pose2d p0 = points_[i];
				const Pose2d p1 = points_[i + 1];
				std::shared_ptr<SplinePair> pair = std::make_shared<SplinePair>(p0, p1);
				splines_.push_back(pair);
			}

			optimize();
		}

		double RobotPath::sumDCurvature2() {
			double sum = 0;
			for (auto pair : splines_)
				sum += pair->sumDCurvature2();

			return sum;
		}

		double RobotPath::optimize()
		{
			int count = 0;
			double prev = sumDCurvature2();
			while (count < kMaxIterations) {
				runOptimizationIteration();
				double current = sumDCurvature2();
				if (prev - current < kMinDelta)
					return current;
				prev = current;
				count++;
			}
			return prev;
		}

		double RobotPath::fitParabola(Translation2d p1, Translation2d p2, Translation2d p3)
		{
			double A = (p3.getX() * (p2.getY() - p1.getY()) + p2.getX() * (p1.getY() - p3.getY()) + p1.getX() * (p3.getY() - p2.getY()));
			double B = (p3.getX() * p3.getX() * (p1.getY() - p2.getY()) + p2.getX() * p2.getX() * (p3.getY() - p1.getY()) + p1.getX() * p1.getX() *
				(p2.getY() - p3.getY()));
			return -B / (2 * A);
		}

		void RobotPath::runOptimizationIteration() {
			if (splines_.size() < 2)
				return;

			std::vector<ControlPoint> controlPoints(splines_.size() - 1);
			double magnitude = 0;

			for (int i = 0; i < splines_.size() - 1; ++i) {
				auto spline = splines_[i];
				auto splinenext = splines_[i + 1];
				if (spline->getStartPose().isColinear(splinenext->getStartPose()) || spline->getEndPose().isColinear(splinenext->getEndPose()))
					continue;

				double original = sumDCurvature2();
				controlPoints[i] = ControlPoint();

				QuinticHermiteSpline sx(spline->x0(), spline->x1(), spline->dx0(), spline->dx1(), spline->ddx0(), spline->ddx1() + kEpsilon);
				QuinticHermiteSpline sy(spline->y0(), spline->y1(), spline->dy0(), spline->dy1(), spline->ddy0(), spline->ddy1());
				auto p = std::make_shared<SplinePair>(sx, sy);
				splines_[i] = p;

				sx = QuinticHermiteSpline(splinenext->x0(), splinenext->x1(), splinenext->dx0(), splinenext->dx1(), splinenext->ddx0() + kEpsilon, splinenext->ddx0());
				sy = QuinticHermiteSpline(splinenext->y0(), splinenext->y1(), splinenext->dy0(), splinenext->dy1(), splinenext->ddy0(), splinenext->ddy1());
				p = std::make_shared<SplinePair>(sx, sy);
				splines_[i + 1] = p;

				controlPoints[i].ddx = (sumDCurvature2() - original) / kEpsilon;

				sx = QuinticHermiteSpline(spline->x0(), spline->x1(), spline->dx0(), spline->dx1(), spline->ddx0(), spline->ddx1());
				sy = QuinticHermiteSpline(spline->y0(), spline->y1(), spline->dy0(), spline->dy1(), spline->ddy0(), spline->ddy1() + kEpsilon);
				p = std::make_shared<SplinePair>(sx, sy);
				splines_[i] = p;

				sx = QuinticHermiteSpline(splinenext->x0(), splinenext->x1(), splinenext->dx0(), splinenext->dx1(), splinenext->ddx0(), splinenext->ddx0());
				sy = QuinticHermiteSpline(splinenext->y0(), splinenext->y1(), splinenext->dy0(), splinenext->dy1(), splinenext->ddy0() + kEpsilon, splinenext->ddy1());
				p = std::make_shared<SplinePair>(sx, sy);
				splines_[i + 1] = p;
				controlPoints[i].ddy = (sumDCurvature2() - original) / kEpsilon;

				splines_[i] = spline;
				splines_[i + 1] = splinenext;
				magnitude += controlPoints[i].ddx * controlPoints[i].ddx + controlPoints[i].ddy * controlPoints[i].ddy;
			}

			magnitude = std::sqrt(magnitude);

			Translation2d p1, p2, p3;
			p2 = Translation2d(0, sumDCurvature2()); //middle point is at the current location

			for (int i = 0; i < splines_.size() - 1; ++i) {
				auto spline = splines_[i];
				auto splinenext = splines_[i + 1];
				if (spline->getStartPose().isColinear(splinenext->getStartPose()) || spline->getEndPose().isColinear(splinenext->getEndPose()))
					continue;

				//normalize to step size
				controlPoints[i].ddx *= kStepSize / magnitude;
				controlPoints[i].ddy *= kStepSize / magnitude;

				//move opposite the gradient by step size amount
				spline->ddxy1(spline->ddx1() - controlPoints[i].ddx, spline->ddy1() - controlPoints[i].ddy);
				splinenext->ddxy0(splinenext->ddx0() - controlPoints[i].ddx, splinenext->ddy0() - controlPoints[i].ddy);
			}

			p1 = Translation2d(-kStepSize, sumDCurvature2());

			for (int i = 0; i < splines_.size() - 1; ++i) { //last point is offset from the middle location by +stepSize
				auto spline = splines_[i];
				auto splinenext = splines_[i + 1];
				if (spline->getStartPose().isColinear(splinenext->getStartPose()) || spline->getEndPose().isColinear(splinenext->getEndPose()))
					continue;

				//move along the gradient by 2 times the step size amount (to return to original location and move by 1
				// step)
				spline->ddxy1(spline->ddx1() + 2 * controlPoints[i].ddx, spline->ddy1() + 2 * controlPoints[i].ddy);
				splinenext->ddxy0(splinenext->ddx0() + 2 * controlPoints[i].ddx, splinenext->ddy0() + 2 * controlPoints[i].ddy);
			}

			p3 = Translation2d(kStepSize, sumDCurvature2());

			double stepSize = fitParabola(p1, p2, p3);

			for (int i = 0; i < splines_.size() - 1; ++i) {
				auto spline = splines_[i];
				auto splinenext = splines_[i + 1];
				if (spline->getStartPose().isColinear(splinenext->getStartPose()) || spline->getEndPose().isColinear(splinenext->getEndPose()))
					continue;

				controlPoints[i].ddx *= 1 + stepSize / kStepSize;
				controlPoints[i].ddy *= 1 + stepSize / kStepSize;

				spline->ddxy1(spline->ddx1() + controlPoints[i].ddx, spline->ddy1() + controlPoints[i].ddy);
				splinenext->ddxy0(splinenext->ddx0() + controlPoints[i].ddx, splinenext->ddy0() + controlPoints[i].ddy);
			}
		}

		void RobotPath::addTrajectory(std::shared_ptr<PathTrajectory> newtraj)
		{
			trajectory_lock_.lock();
			auto it = std::find_if(trajectories_.begin(), trajectories_.end(), [newtraj](std::shared_ptr<PathTrajectory> traj) { return traj->name() == newtraj->name(); });
			if (it != trajectories_.end())
				trajectories_.erase(it);
			trajectories_.push_back(newtraj);


			if (newtraj->name() == TrajectoryName::Main)
			{
				size_t i = 0;
				size_t current = 1;
				times_.clear();
				times_.push_back(0.0);
				double curdist = std::numeric_limits<double>::max();

				while (i < newtraj->size() && current < points_.size())
				{
					const Pose2dWithTrajectory& pt = (*newtraj)[i];
					double dist = points_[current].distance(pt.pose());
					if (dist <= curdist || std::fabs(dist - curdist) < 0.1)
					{
						//
						// Still approaching
						//
						curdist = dist;
					}
					else
					{
						//
						// We found it, it was the previous point
						//
						if (i == 0)
							times_.push_back(0.0);
						else
							times_.push_back((*newtraj)[i - 1].time());

						curdist = std::numeric_limits<double>::max();
						current++;
					}

					i++;
				}
				times_.push_back((*newtraj)[newtraj->size() - 1].time());
			}

			trajectory_lock_.unlock();
		}
	}
}