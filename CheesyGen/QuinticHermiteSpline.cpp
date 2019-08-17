#include "QuinticHermiteSpline.h"
#include "Pose2d.h"
#include <cmath>

namespace xero {
	namespace paths
	{
		QuinticHermiteSpline::QuinticHermiteSpline(const Pose2d& p0, const Pose2d& p1)
		{
			double scale = 1.2 * p0.getTranslation().distance(p1.getTranslation());
			x0 = p0.getTranslation().getX();
			x1 = p1.getTranslation().getX();
			dx0 = p0.getRotation().getCos() * scale;
			dx1 = p1.getRotation().getCos() * scale;
			ddx0 = 0.0;
			ddx1 = 0.0;

			y0 = p0.getTranslation().getY();
			y1 = p1.getTranslation().getY();
			dy0 = p0.getRotation().getSin() * scale;
			dy1 = p1.getRotation().getSin() * scale;
			ddy0 = 0.0;
			ddy1 = 0.0;

			compute();
		}

		QuinticHermiteSpline::QuinticHermiteSpline(double ax0, double ax1, double adx0, double adx1, double addx0, double addx1,
													double ay0, double ay1, double ady0, double ady1, double addy0, double addy1)
		{
			x0 = ax0;
			x1 = ax1;
			dx0 = adx0;
			dx1 = adx1;
			ddx0 = addx0;
			ddx1 = addx1;

			y0 = ay0;
			y1 = ay1;
			dy0 = ady0;
			dy1 = ady1;
			ddy0 = addy0;
			ddy1 = addy1;

			compute();
		}

		QuinticHermiteSpline::QuinticHermiteSpline(const QuinticHermiteSpline& other)
		{
			x0 = other.x0;
			x1 = other.x1;
			dx0 = other.dx0;
			dx1 = other.dx1;
			ddx0 = other.ddx0;
			ddx1 = other.ddx1;
			
			y0 = other.y0;
			y1 = other.y1;
			dy0 = other.dy0;
			dy1 = other.dy1;
			ddy0 = other.ddy0;
			ddy1 = other.ddy1;

			ax = other.ax;
			bx = other.bx;
			cx = other.cx;
			dx = other.dx;
			ex = other.ex;
			fx = other.fx;

			ay = other.ay;
			by = other.by;
			cy = other.cy;
			dy = other.dy;
			ey = other.ey;
			fy = other.fy;
		}

		QuinticHermiteSpline::~QuinticHermiteSpline()
		{
		}

		void QuinticHermiteSpline::compute()
		{
			ax = -6 * x0 - 3 * dx0 - 0.5 * ddx0 + 0.5 * ddx1 - 3 * dx1 + 6 * x1;
			bx = 15 * x0 + 8 * dx0 + 1.5 * ddx0 - ddx1 + 7 * dx1 - 15 * x1;
			cx = -10 * x0 - 6 * dx0 - 1.5 * ddx0 + 0.5 * ddx1 - 4 * dx1 + 10 * x1;
			dx = 0.5 * ddx0;
			ex = dx0;
			fx = x0;

			ay = -6 * y0 - 3 * dy0 - 0.5 * ddy0 + 0.5 * ddy1 - 3 * dy1 + 6 * y1;
			by = 15 * y0 + 8 * dy0 + 1.5 * ddy0 - ddy1 + 7 * dy1 - 15 * y1;
			cy = -10 * y0 - 6 * dy0 - 1.5 * ddy0 + 0.5 * ddy1 - 4 * dy1 + 10 * y1;
			dy = 0.5 * ddy0;
			ey = dy0;
			fy = y0;
		}

		Pose2d QuinticHermiteSpline::getStartPose() const
		{
			Translation2d trans(x0, y0);
			Rotation2d rot(dx0, dy0, true);
			return Pose2d(trans, rot);
		}

		Pose2d QuinticHermiteSpline::getEndPose() const
		{
			Translation2d trans(x1, y1);
			Rotation2d rot(dx1, dy1, true);
			return Pose2d(trans, rot);
		}

		Translation2d QuinticHermiteSpline::getPoint(double t) const
		{
			double x = ax * t * t * t * t * t + bx * t * t * t * t + cx * t * t * t + dx * t * t + ex * t + fx;
			double y = ay * t * t * t * t * t + by * t * t * t * t + cy * t * t * t + dy * t * t + ey * t + fy;
			return Translation2d(x, y);
		}

		Rotation2d QuinticHermiteSpline::getHeading(double t) const
		{
			return Rotation2d(fdx(t), fdy(t), true);
		}

		double QuinticHermiteSpline::getVelocity(double t) const
		{
			return std::hypot(fdx(t), fdy(t));
		}
		double QuinticHermiteSpline::getCurvature(double t) const
		{
			return (fdx(t) * ddy(t) - ddx(t) * fdy(t)) / 
				((fdx(t) * fdx(t) + fdy(t) * fdy(t)) * std::sqrt((fdx(t) * fdx(t) + fdy(t) * fdy(t))));
		}

		double QuinticHermiteSpline::getDCurvature(double t) const
		{
			double dx2dy2 = (fdx(t) * fdx(t) + fdy(t) * fdy(t));
			double num = (fdx(t) * dddy(t) - dddx(t) * fdy(t)) * dx2dy2 - 3 * (fdx(t) * ddy(t) - ddx(t) * fdy(t)) * (fdx(t) * ddx(t) + fdy(t) * ddy(t));
			return num / (dx2dy2 * dx2dy2 * std::sqrt(dx2dy2));
		}

		double QuinticHermiteSpline::sumDCurvature2(std::vector<std::shared_ptr<QuinticHermiteSpline>>& splines)
		{
			double sum = 0;
			for (auto s : splines) {
				sum += s->sumDCurvature2();
			}
			return sum;
		}

		double QuinticHermiteSpline::getDCurvature2(double t)
		{
			double dx2dy2 = (fdx(t) * fdx(t) + fdy(t) * fdy(t));
			double num = (fdx(t) * dddy(t) - dddx(t) * fdy(t)) * dx2dy2 - 3 * (fdx(t) * ddy(t) - ddx(t) * fdy(t)) * (fdx(t) * ddx(t) + fdy(t) * ddy(t));
			return num * num / (dx2dy2 * dx2dy2 * dx2dy2 * dx2dy2 * dx2dy2);
		}

		double QuinticHermiteSpline::sumDCurvature2()
		{
			double dt = 1.0 / kSamples;
			double sum = 0;
			for (double t = 0; t < 1.0; t += dt) {
				sum += (dt * getDCurvature2(t));
			}
			return sum;
		}
		double QuinticHermiteSpline::fitParabola(const Translation2d& p1, const Translation2d& p2, const Translation2d& p3)
		{
			double A = (p3.getX() * (p2.getY() - p1.getY()) + p2.getX() * (p1.getY() - p3.getY()) + p1.getX() * (p3.getY() - p2.getY()));
			double B = (p3.getX() * p3.getX() * (p1.getY() - p2.getY()) + p2.getX() * p2.getX() * (p3.getY() - p1.getY()) + p1.getX() * p1.getX() *
				(p2.getY() - p3.getY()));
			return -B / (2 * A);
		}

		void QuinticHermiteSpline::runOptimizationIteration(std::vector<std::shared_ptr<QuinticHermiteSpline>>& splines)
		{
			//can't optimize anything with less than 2 splines
			if (splines.size() <= 1) {
				return;
			}

			std::vector<ControlPoint> controlPoints(splines.size() - 1);
			double magnitude = 0;

			for (size_t i = 0; i < splines.size() - 1; ++i) {
				std::shared_ptr<QuinticHermiteSpline> spline = splines[i];
				std::shared_ptr<QuinticHermiteSpline> nextspline = splines[i + 1];

				if (spline->getStartPose().isColinear(nextspline->getStartPose()) || spline->getEndPose().isColinear(nextspline->getEndPose()))
					continue;

				double original = sumDCurvature2(splines);
				std::shared_ptr<QuinticHermiteSpline> temp = spline;
				std::shared_ptr<QuinticHermiteSpline> temp1 = nextspline;

				controlPoints[i] = ControlPoint(0, 0);

				//calculate partial derivatives of sumDCurvature2
				splines[i] = std::make_shared<QuinticHermiteSpline>(temp->x0, temp->x1, temp->dx0, temp->dx1, temp->ddx0, temp->ddx1 + MathUtils::kEpsilon,
					temp->y0, temp->y1, temp->dy0, temp->dy1, temp->ddy0, temp->ddy1);
				splines[i + 1] = std::make_shared<QuinticHermiteSpline>(temp1->x0, temp1->x1, temp1->dx0, temp1->dx1, temp1->ddx0 +
					MathUtils::kEpsilon, temp1->ddx1, temp1->y0, temp1->y1, temp1->dy0, temp1->dy1, temp1->ddy0, temp1->ddy1);

				controlPoints[i].ddx = (sumDCurvature2(splines) - original) / MathUtils::kEpsilon;

				splines[i] = std::make_shared<QuinticHermiteSpline>(temp->x0, temp->x1, temp->dx0, temp->dx1, temp->ddx0, temp->ddx1,
					temp->y0, temp->y1, temp->dy0, temp->dy1, temp->ddy0, temp->ddy1 + MathUtils::kEpsilon);
				splines[i + 1] = std::make_shared<QuinticHermiteSpline>(temp1->x0, temp1->x1, temp1->dx0, temp1->dx1, temp1->ddx0, temp1->ddx1,
					temp1->y0, temp1->y1, temp1->dy0, temp1->dy1, temp1->ddy0 + MathUtils::kEpsilon, temp1->ddy1);

				controlPoints[i].ddy = (sumDCurvature2(splines) - original) / MathUtils::kEpsilon;

				splines[i] = temp;
				splines[i + 1] = temp1;
				magnitude += controlPoints[i].ddx * controlPoints[i].ddx + controlPoints[i].ddy * controlPoints[i].ddy;
			}

			magnitude = std::sqrt(magnitude);

			//minimize along the direction of the gradient
			//first calculate 3 points along the direction of the gradient
			Translation2d p1, p2, p3;
			p2 = Translation2d(0, sumDCurvature2(splines)); //middle point is at the current location

			for (size_t i = 0; i < splines.size() - 1; ++i) { //first point is offset from the middle location by -stepSize
				std::shared_ptr<QuinticHermiteSpline> spline = splines[i];
				std::shared_ptr<QuinticHermiteSpline> nextspline = splines[i + 1];

				if (spline->getStartPose().isColinear(nextspline->getStartPose()) || spline->getEndPose().isColinear(nextspline->getEndPose()))
					continue;

				//normalize to step size
				controlPoints[i].ddx *= kStepSize / magnitude;
				controlPoints[i].ddy *= kStepSize / magnitude;

				//move opposite the gradient by step size amount
				splines[i]->ddx1 -= controlPoints[i].ddx;
				splines[i]->ddy1 -= controlPoints[i].ddy;
				splines[i+1]->ddx0 -= controlPoints[i].ddx;
				splines[i+1]->ddy0 -= controlPoints[i].ddy;

				//recompute the spline's coefficients to account for new second derivatives
				splines[i]->compute();
				splines[i + 1]->compute();
			}

			p1 = Translation2d(-kStepSize, sumDCurvature2(splines));

			for (size_t i = 0; i < splines.size() - 1; ++i) { //last point is offset from the middle location by +stepSize
				std::shared_ptr<QuinticHermiteSpline> spline = splines[i];
				std::shared_ptr<QuinticHermiteSpline> nextspline = splines[i + 1];

				if (spline->getStartPose().isColinear(nextspline->getStartPose()) || spline->getEndPose().isColinear(nextspline->getEndPose()))
					continue;

				//move along the gradient by 2 times the step size amount (to return to original location and move by 1
				// step)
				splines[i]->ddx1 += 2 * controlPoints[i].ddx;
				splines[i]->ddy1 += 2 * controlPoints[i].ddy;
				splines[i+1]->ddx0 += 2 * controlPoints[i].ddx;
				splines[i+1]->ddy0 += 2 * controlPoints[i].ddy;

				//recompute the spline's coefficients to account for new second derivatives
				splines[i]->compute();
				splines[i + 1]->compute();
			}

			p3 = Translation2d(kStepSize, sumDCurvature2(splines));
			double stepSize = fitParabola(p1, p2, p3); //approximate step size to minimize sumDCurvature2 along the gradient

			for (size_t i = 0; i < splines.size() - 1; ++i) {
				std::shared_ptr<QuinticHermiteSpline> spline = splines[i];
				std::shared_ptr<QuinticHermiteSpline> nextspline = splines[i + 1];

				if (spline->getStartPose().isColinear(nextspline->getStartPose()) || spline->getEndPose().isColinear(nextspline->getEndPose()))
					continue;

				//move by the step size calculated by the parabola fit (+1 to offset for the final transformation to find
				// p3)
				controlPoints[i].ddx *= 1 + stepSize / kStepSize;
				controlPoints[i].ddy *= 1 + stepSize / kStepSize;

				splines[i]->ddx1 += controlPoints[i].ddx;
				splines[i]->ddy1 += controlPoints[i].ddy;
				splines[i + 1]->ddx0 += controlPoints[i].ddx;
				splines[i + 1]->ddy0 += controlPoints[i].ddy;

				//recompute the spline's coefficients to account for new second derivatives
				splines[i]->compute();
				splines[i + 1]->compute();
			}
		}

		double QuinticHermiteSpline::optimizeSpline(std::vector<std::shared_ptr<QuinticHermiteSpline>>& splines)
		{
			int count = 0;
			double prev = sumDCurvature2(splines);
			while (count < kMaxIterations) {
				runOptimizationIteration(splines);
				double current = sumDCurvature2(splines);
				if (prev - current < kMinDelta)
					return current;
				prev = current;
				count++;
			}
			return prev;
		}
	}
}
