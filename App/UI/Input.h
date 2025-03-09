/********************************************************************************
** Form generated from reading UI file 'Input.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef INPUT_H
#define INPUT_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Input
{
public:
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QLineEdit *editor;
    QPushButton *sendBtn;
    QPushButton *closeBtn;

    void setupUi(QWidget *Input)
    {
        if (Input->objectName().isEmpty())
            Input->setObjectName("Input");
        Input->resize(317, 43);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Input->sizePolicy().hasHeightForWidth());
        Input->setSizePolicy(sizePolicy);
        Input->setAutoFillBackground(false);
        Input->setStyleSheet(QString::fromUtf8("border-radius: 8pt"));
        horizontalLayout = new QHBoxLayout(Input);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(2, 6, 2, 6);
        horizontalSpacer = new QSpacerItem(9, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        editor = new QLineEdit(Input);
        editor->setObjectName("editor");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(editor->sizePolicy().hasHeightForWidth());
        editor->setSizePolicy(sizePolicy1);
        editor->setClearButtonEnabled(true);

        horizontalLayout->addWidget(editor);

        sendBtn = new QPushButton(Input);
        sendBtn->setObjectName("sendBtn");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(sendBtn->sizePolicy().hasHeightForWidth());
        sendBtn->setSizePolicy(sizePolicy2);
        QIcon icon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSend));
        sendBtn->setIcon(icon);
        sendBtn->setFlat(true);

        horizontalLayout->addWidget(sendBtn);

        closeBtn = new QPushButton(Input);
        closeBtn->setObjectName("closeBtn");
        sizePolicy2.setHeightForWidth(closeBtn->sizePolicy().hasHeightForWidth());
        closeBtn->setSizePolicy(sizePolicy2);
        QIcon icon1(QIcon::fromTheme(QIcon::ThemeIcon::WindowClose));
        closeBtn->setIcon(icon1);
        closeBtn->setFlat(true);

        horizontalLayout->addWidget(closeBtn);

        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 16);
        horizontalLayout->setStretch(2, 4);
        horizontalLayout->setStretch(3, 4);

        retranslateUi(Input);

        QMetaObject::connectSlotsByName(Input);
    } // setupUi

    void retranslateUi(QWidget *Input)
    {
        Input->setWindowTitle(QCoreApplication::translate("Input", "Input", nullptr));
        sendBtn->setText(QString());
#if QT_CONFIG(shortcut)
        sendBtn->setShortcut(QCoreApplication::translate("Input", "Ctrl+Return", nullptr));
#endif // QT_CONFIG(shortcut)
        closeBtn->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Input: public Ui_Input {};
} // namespace Ui

QT_END_NAMESPACE

#endif // INPUT_H
