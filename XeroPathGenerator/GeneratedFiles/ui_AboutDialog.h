/********************************************************************************
** Form generated from reading UI file 'AboutDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUTDIALOG_H
#define UI_ABOUTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_AboutDialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *image_;
    QTextEdit *text_;
    QDialogButtonBox *buttons_;

    void setupUi(QDialog *AboutDialog)
    {
        if (AboutDialog->objectName().isEmpty())
            AboutDialog->setObjectName(QString::fromUtf8("AboutDialog"));
        AboutDialog->resize(820, 499);
        verticalLayout = new QVBoxLayout(AboutDialog);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        image_ = new QLabel(AboutDialog);
        image_->setObjectName(QString::fromUtf8("image_"));
        image_->setMinimumSize(QSize(250, 0));

        horizontalLayout->addWidget(image_);

        text_ = new QTextEdit(AboutDialog);
        text_->setObjectName(QString::fromUtf8("text_"));

        horizontalLayout->addWidget(text_);


        verticalLayout->addLayout(horizontalLayout);

        buttons_ = new QDialogButtonBox(AboutDialog);
        buttons_->setObjectName(QString::fromUtf8("buttons_"));
        buttons_->setStandardButtons(QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttons_);


        retranslateUi(AboutDialog);

        QMetaObject::connectSlotsByName(AboutDialog);
    } // setupUi

    void retranslateUi(QDialog *AboutDialog)
    {
        AboutDialog->setWindowTitle(QCoreApplication::translate("AboutDialog", "About XeroPathGenerator", nullptr));
        image_->setText(QString());
        text_->setDocumentTitle(QCoreApplication::translate("AboutDialog", "About ", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AboutDialog: public Ui_AboutDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTDIALOG_H
