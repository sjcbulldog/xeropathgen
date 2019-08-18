/********************************************************************************
** Form generated from reading UI file 'PlotVariableSelector.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLOTVARIABLESELECTOR_H
#define UI_PLOTVARIABLESELECTOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_PlotVariableSelector
{
public:
    QVBoxLayout *verticalLayout;
    QTreeWidget *tree_;
    QDialogButtonBox *buttons_;

    void setupUi(QDialog *PlotVariableSelector)
    {
        if (PlotVariableSelector->objectName().isEmpty())
            PlotVariableSelector->setObjectName(QString::fromUtf8("PlotVariableSelector"));
        PlotVariableSelector->resize(400, 300);
        verticalLayout = new QVBoxLayout(PlotVariableSelector);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tree_ = new QTreeWidget(PlotVariableSelector);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        tree_->setHeaderItem(__qtreewidgetitem);
        tree_->setObjectName(QString::fromUtf8("tree_"));

        verticalLayout->addWidget(tree_);

        buttons_ = new QDialogButtonBox(PlotVariableSelector);
        buttons_->setObjectName(QString::fromUtf8("buttons_"));
        buttons_->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttons_);


        retranslateUi(PlotVariableSelector);

        QMetaObject::connectSlotsByName(PlotVariableSelector);
    } // setupUi

    void retranslateUi(QDialog *PlotVariableSelector)
    {
        PlotVariableSelector->setWindowTitle(QCoreApplication::translate("PlotVariableSelector", "Trajectory Profile Variables", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PlotVariableSelector: public Ui_PlotVariableSelector {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLOTVARIABLESELECTOR_H
