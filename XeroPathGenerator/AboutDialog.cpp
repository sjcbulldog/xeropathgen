#include "AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent)
{
	QString exedir = QCoreApplication::applicationDirPath();
	QString imagepath = exedir + "/ErrorCodeXero.png";
	QPixmap image(imagepath);

	ui.setupUi(this);
	ui.image_->setPixmap(image);
	ui.text_->setReadOnly(true);

	QFont font = ui.text_->font();
	font.setPointSize(16);
	QTextCharFormat fmt;

	fmt.setFont(font);
	ui.text_->setCurrentCharFormat(fmt);
	ui.text_->append("XeroPathGenerator\r\n\r\n");

	font.setPointSize(12);
	fmt.setFont(font);
	ui.text_->setCurrentCharFormat(fmt);
	ui.text_->append("Brought to you by 1425\r\nError Code Xero\r\n\r\n");
	ui.text_->append("Icons made by Dave Gandy from www.flaticon.com\r\n\r\n");
	ui.text_->append("Ideas, algorithms, concepts, taking from Cheesy Poofs with much appreciation.\r\n\r\n");

	connect(ui.buttons_, &QDialogButtonBox::accepted, this, &AboutDialog::accept);
}

AboutDialog::~AboutDialog()
{
}
