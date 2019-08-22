/********************************************************************************
** Form generated from reading UI file 'SelectRobotDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SELECTROBOTDIALOG_H
#define UI_SELECTROBOTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QListView>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SelectRobotDialog
{
public:
    QVBoxLayout *verticalLayout;
    QListView *robots_;
    QDialogButtonBox *buttons_;

    void setupUi(QDialog *SelectRobotDialog)
    {
        if (SelectRobotDialog->objectName().isEmpty())
            SelectRobotDialog->setObjectName(QString::fromUtf8("SelectRobotDialog"));
        SelectRobotDialog->resize(329, 261);
        verticalLayout = new QVBoxLayout(SelectRobotDialog);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        robots_ = new QListView(SelectRobotDialog);
        robots_->setObjectName(QString::fromUtf8("robots_"));

        verticalLayout->addWidget(robots_);

        buttons_ = new QDialogButtonBox(SelectRobotDialog);
        buttons_->setObjectName(QString::fromUtf8("buttons_"));
        buttons_->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttons_);


        retranslateUi(SelectRobotDialog);

        QMetaObject::connectSlotsByName(SelectRobotDialog);
    } // setupUi

    void retranslateUi(QDialog *SelectRobotDialog)
    {
        SelectRobotDialog->setWindowTitle(QCoreApplication::translate("SelectRobotDialog", "SelectRobotDialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SelectRobotDialog: public Ui_SelectRobotDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SELECTROBOTDIALOG_H
