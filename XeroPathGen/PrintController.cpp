#include "PrintController.h"
#include "PathFieldView.h"
#include "PathPlotView.h"
#include <QStyleOptionGraphicsItem>

PrintController::PrintController(PathFieldView& field, PathPlotView& plot) : field_(field), plot_(plot)
{
}

PrintController::~PrintController()
{
}

void PrintController::mapToPaperRect(QPainter& paint, const QRect &loc, const QSize &size)
{
	double xscale = loc.width() / size.width();
	double yscale = loc.height() / size.height();
	double scale = qMin(xscale, yscale);
	paint.translate(loc.x() + loc.width() / 2, loc.y() + loc.height() / 2);
	paint.scale(scale, scale);
	paint.translate(-size.width() / 2, -size.height() / 2);
}

void PrintController::printField(QPrinter& printer, std::shared_ptr<xero::paths::RobotPath> path)
{
	QRectF paper = printer.paperRect(QPrinter::Inch);
	QRect r;
	QPainter paint;

	paint.begin(&printer);

	QString title = QString(path->getParent()->getName().c_str()) + "/" + QString(path->getName().c_str());
	QTextOption opt;
	opt.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

	QFont f = paint.font();
	f.setPointSizeF(18.0);
	paint.setFont(f);
	QFontMetricsF metrics(f);
	double h = metrics.height();
	QRectF rf(0, 100, paper.width(), h);
	paint.drawText(rf, title, opt);

	r = QRect(0, 0, paper.width(), paper.height() / 2);
	paint.save();
	mapToPaperRect(paint, r, QSize(field_.width(), field_.height()));
	field_.doPaint(paint, true);
	paint.restore();

	QRectF target(0, paper.height() / 2, paper.width() , paper.height() / 2);
	plot_.render(&paint, target, QRect(), Qt::IgnoreAspectRatio);

	paint.end();
}

void PrintController::printGroup(QPrinter& printer, std::shared_ptr<xero::paths::PathGroup> group)
{
	QRectF paper = printer.paperRect(QPrinter::Inch);
	QRect r;
	QPainter paint;


	paint.begin(&printer);

	QString title(group->getName().c_str());
	QTextOption opt;
	opt.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

	QFont f = paint.font();
	f.setPointSizeF(18.0);
	paint.setFont(f);
	QFontMetricsF metrics(f);
	double fh = metrics.height();
	QRectF rf(0, 100, paper.width(), fh);
	paint.drawText(rf, title, opt);

	int h = static_cast<int>((paper.height() - fh) / group->getPaths().size());
	int i = 0;
	for (auto p : group->getPaths())
	{
		field_.setPath(p);
		r = QRect(0, i * h + fh, paper.width(), h);
		paint.save();
		mapToPaperRect(paint, r, QSize(field_.width(), field_.height()));
		field_.doPaint(paint, true);
		paint.restore();
		field_.setPath(nullptr);

		i++;
	}
}