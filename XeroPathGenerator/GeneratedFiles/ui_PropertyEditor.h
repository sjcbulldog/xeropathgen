/********************************************************************************
** Form generated from reading UI file 'PropertyEditor.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROPERTYEDITOR_H
#define UI_PROPERTYEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PropertyEditor
{
public:
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout;
    QTreeView *tree_;
    QDialogButtonBox *buttons_;
    QHBoxLayout *horizontalLayout;

    void setupUi(QWidget *PropertyEditor)
    {
        if (PropertyEditor->objectName().isEmpty())
            PropertyEditor->setObjectName(QString::fromUtf8("PropertyEditor"));
        PropertyEditor->resize(573, 397);
        horizontalLayout_2 = new QHBoxLayout(PropertyEditor);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        tree_ = new QTreeView(PropertyEditor);
        tree_->setObjectName(QString::fromUtf8("tree_"));

        verticalLayout->addWidget(tree_);

        buttons_ = new QDialogButtonBox(PropertyEditor);
        buttons_->setObjectName(QString::fromUtf8("buttons_"));
        buttons_->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttons_);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));

        verticalLayout->addLayout(horizontalLayout);


        horizontalLayout_2->addLayout(verticalLayout);


        retranslateUi(PropertyEditor);

        QMetaObject::connectSlotsByName(PropertyEditor);
    } // setupUi

    void retranslateUi(QWidget *PropertyEditor)
    {
        PropertyEditor->setWindowTitle(QCoreApplication::translate("PropertyEditor", "PropertyEditor", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PropertyEditor: public Ui_PropertyEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROPERTYEDITOR_H
