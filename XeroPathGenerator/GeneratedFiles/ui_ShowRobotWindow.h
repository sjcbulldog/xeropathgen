/********************************************************************************
** Form generated from reading UI file 'ShowRobotWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.13.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SHOWROBOTWINDOW_H
#define UI_SHOWROBOTWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ShowRobotWindow
{
public:

    void setupUi(QWidget *ShowRobotWindow)
    {
        if (ShowRobotWindow->objectName().isEmpty())
            ShowRobotWindow->setObjectName(QString::fromUtf8("ShowRobotWindow"));
        ShowRobotWindow->resize(400, 300);

        retranslateUi(ShowRobotWindow);

        QMetaObject::connectSlotsByName(ShowRobotWindow);
    } // setupUi

    void retranslateUi(QWidget *ShowRobotWindow)
    {
        ShowRobotWindow->setWindowTitle(QCoreApplication::translate("ShowRobotWindow", "ShowRobotWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ShowRobotWindow: public Ui_ShowRobotWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SHOWROBOTWINDOW_H
