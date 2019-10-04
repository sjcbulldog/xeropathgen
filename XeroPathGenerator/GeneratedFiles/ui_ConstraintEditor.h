/********************************************************************************
** Form generated from reading UI file 'ConstraintEditor.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONSTRAINTEDITOR_H
#define UI_CONSTRAINTEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ConstraintEditor
{
public:
    QHBoxLayout *horizontalLayout;
    QTreeView *tree_;
    QVBoxLayout *verticalLayout;
    QPushButton *add_;
    QPushButton *delete_;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *ConstraintEditor)
    {
        if (ConstraintEditor->objectName().isEmpty())
            ConstraintEditor->setObjectName(QString::fromUtf8("ConstraintEditor"));
        ConstraintEditor->resize(487, 300);
        horizontalLayout = new QHBoxLayout(ConstraintEditor);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        tree_ = new QTreeView(ConstraintEditor);
        tree_->setObjectName(QString::fromUtf8("tree_"));

        horizontalLayout->addWidget(tree_);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        add_ = new QPushButton(ConstraintEditor);
        add_->setObjectName(QString::fromUtf8("add_"));
        add_->setMaximumSize(QSize(50, 16777215));

        verticalLayout->addWidget(add_);

        delete_ = new QPushButton(ConstraintEditor);
        delete_->setObjectName(QString::fromUtf8("delete_"));
        delete_->setMaximumSize(QSize(50, 16777215));

        verticalLayout->addWidget(delete_);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout->addLayout(verticalLayout);


        retranslateUi(ConstraintEditor);

        QMetaObject::connectSlotsByName(ConstraintEditor);
    } // setupUi

    void retranslateUi(QWidget *ConstraintEditor)
    {
        ConstraintEditor->setWindowTitle(QCoreApplication::translate("ConstraintEditor", "ConstraintEditor", nullptr));
        add_->setText(QCoreApplication::translate("ConstraintEditor", "Add", nullptr));
        delete_->setText(QCoreApplication::translate("ConstraintEditor", "Delete", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ConstraintEditor: public Ui_ConstraintEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONSTRAINTEDITOR_H
