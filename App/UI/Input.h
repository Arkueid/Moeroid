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
    QSpacerItem *horizontalSpacer_2;

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
        Input->setStyleSheet(QString::fromUtf8("/* \350\256\276\347\275\256\346\225\264\344\275\223\350\203\214\346\231\257\351\242\234\350\211\262 */\n"
"QWidget {\n"
"    background-color: #FFD9E8; /* \347\262\211\350\211\262\350\203\214\346\231\257 */\n"
"}\n"
"\n"
"/* QLineEdit \346\240\267\345\274\217 */\n"
"QLineEdit {\n"
"    border: 2px solid #F4A6B7; /* \350\276\271\346\241\206\351\242\234\350\211\262 */\n"
"    border-radius: 10px; /* \345\234\206\350\247\222\350\276\271\346\241\206 */\n"
"    padding: 5px; /* \345\206\205\350\276\271\350\267\235 */\n"
"    background-color: #FFF3F8; /* \350\276\223\345\205\245\346\241\206\350\203\214\346\231\257\351\242\234\350\211\262 */\n"
"    color: #8C2C60; /* \346\226\207\345\255\227\351\242\234\350\211\262 */\n"
"}\n"
"\n"
"/* \345\275\223\350\276\223\345\205\245\346\241\206\350\242\253\351\200\211\344\270\255\346\227\266\347\232\204\346\240\267\345\274\217 */\n"
"QLineEdit:focus {\n"
"    border-color: #FF6CC9; /* \346\233\264\351\262\234\350\211\263\347\232\204\351\242\234\350\211\262\347\252\201\345\207"
                        "\272\346\230\276\347\244\272 */\n"
"    background-color: #FFECF3;\n"
"}\n"
"\n"
"/* QPushButton \345\237\272\347\241\200\346\240\267\345\274\217 */\n"
"QPushButton {\n"
"    border: 2px solid #F4A6B7; /* \350\276\271\346\241\206\351\242\234\350\211\262 */\n"
"    border-radius: 10px; /* \345\234\206\350\247\222\350\276\271\346\241\206 */\n"
"    padding: 5px 15px; /* \345\206\205\350\276\271\350\267\235 */\n"
"    background-color: #FFBBD7; /* \346\214\211\351\222\256\350\203\214\346\231\257\351\242\234\350\211\262 */\n"
"    color: #FFFFFF; /* \346\226\207\345\255\227\351\242\234\350\211\262 */\n"
"    font-weight: bold; /* \345\212\240\347\262\227\345\255\227\344\275\223 */\n"
"}\n"
"\n"
"/* \345\275\223\351\274\240\346\240\207\346\202\254\345\201\234\345\234\250\346\214\211\351\222\256\344\270\212\346\227\266 */\n"
"QPushButton:hover {\n"
"    background-color: #FF6CC9; /* \346\224\271\345\217\230\350\203\214\346\231\257\351\242\234\350\211\262 */\n"
"}\n"
"\n"
"/* \345\275\223\346\214\211\351\222\256\350\242"
                        "\253\346\214\211\344\270\213\346\227\266 */\n"
"QPushButton:pressed {\n"
"    background-color: #D957A3; /* \346\233\264\346\267\261\347\232\204\351\242\234\350\211\262\350\241\250\347\244\272\346\214\211\344\270\213\347\212\266\346\200\201 */\n"
"}\n"
"\n"
"/* \346\267\273\345\212\240\345\233\276\346\240\207\346\210\226\345\205\266\344\273\226\350\243\205\351\245\260\345\205\203\347\264\240\357\274\210\345\246\202\346\236\234\351\234\200\350\246\201\357\274\211\345\217\257\344\273\245\345\234\250\350\277\231\351\207\214\350\277\233\350\241\214\350\260\203\346\225\264 */"));
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

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 16);
        horizontalLayout->setStretch(2, 4);
        horizontalLayout->setStretch(3, 4);
        horizontalLayout->setStretch(4, 1);

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
