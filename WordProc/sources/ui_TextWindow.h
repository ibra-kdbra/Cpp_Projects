/********************************************************************************
** Form generated from reading UI file 'TextWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEXTWINDOW_H
#define UI_TEXTWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFontComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TextWindow
{
public:
    QVBoxLayout *lay_textwindow;
    QWidget *wid_header;
    QHBoxLayout *lay_header;
    QFontComboBox *btn_font;
    QSpinBox *btn_fontsize;
    QFrame *lin_header1;
    QPushButton *btn_font_bold;
    QPushButton *btn_font_italic;
    QPushButton *btn_font_underline;
    QFrame *lin_header2;
    QPushButton *btn_align_left;
    QPushButton *btn_align_center;
    QPushButton *btn_align_right;
    QPushButton *btn_align_justify;
    QSpacerItem *spa_header;
    QTextEdit *wid_text;
    QWidget *wid_footer;

    void setupUi(QWidget *TextWindow)
    {
        if (TextWindow->objectName().isEmpty())
            TextWindow->setObjectName(QString::fromUtf8("TextWindow"));
        TextWindow->resize(640, 480);
        lay_textwindow = new QVBoxLayout(TextWindow);
        lay_textwindow->setSpacing(2);
        lay_textwindow->setObjectName(QString::fromUtf8("lay_textwindow"));
        lay_textwindow->setContentsMargins(2, 2, 2, 2);
        wid_header = new QWidget(TextWindow);
        wid_header->setObjectName(QString::fromUtf8("wid_header"));
        lay_header = new QHBoxLayout(wid_header);
        lay_header->setSpacing(2);
        lay_header->setObjectName(QString::fromUtf8("lay_header"));
        lay_header->setContentsMargins(2, 2, 2, 2);
        btn_font = new QFontComboBox(wid_header);
        btn_font->setObjectName(QString::fromUtf8("btn_font"));
        btn_font->setEditable(false);
        btn_font->setCurrentText(QString::fromUtf8("DejaVu Sans"));

        lay_header->addWidget(btn_font);

        btn_fontsize = new QSpinBox(wid_header);
        btn_fontsize->setObjectName(QString::fromUtf8("btn_fontsize"));
        btn_fontsize->setSpecialValueText(QString::fromUtf8(""));
        btn_fontsize->setMinimum(3);
        btn_fontsize->setMaximum(100);
        btn_fontsize->setValue(12);

        lay_header->addWidget(btn_fontsize);

        lin_header1 = new QFrame(wid_header);
        lin_header1->setObjectName(QString::fromUtf8("lin_header1"));
        lin_header1->setFrameShape(QFrame::VLine);
        lin_header1->setFrameShadow(QFrame::Sunken);

        lay_header->addWidget(lin_header1);

        btn_font_bold = new QPushButton(wid_header);
        btn_font_bold->setObjectName(QString::fromUtf8("btn_font_bold"));
        btn_font_bold->setText(QString::fromUtf8(""));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Icons/Format/TextBold"), QSize(), QIcon::Normal, QIcon::Off);
        btn_font_bold->setIcon(icon);
        btn_font_bold->setCheckable(true);

        lay_header->addWidget(btn_font_bold);

        btn_font_italic = new QPushButton(wid_header);
        btn_font_italic->setObjectName(QString::fromUtf8("btn_font_italic"));
        btn_font_italic->setText(QString::fromUtf8(""));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Icons/Format/TextItalic"), QSize(), QIcon::Normal, QIcon::Off);
        btn_font_italic->setIcon(icon1);
        btn_font_italic->setCheckable(true);

        lay_header->addWidget(btn_font_italic);

        btn_font_underline = new QPushButton(wid_header);
        btn_font_underline->setObjectName(QString::fromUtf8("btn_font_underline"));
        btn_font_underline->setText(QString::fromUtf8(""));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Icons/Format/TextUnderline"), QSize(), QIcon::Normal, QIcon::Off);
        btn_font_underline->setIcon(icon2);
        btn_font_underline->setCheckable(true);

        lay_header->addWidget(btn_font_underline);

        lin_header2 = new QFrame(wid_header);
        lin_header2->setObjectName(QString::fromUtf8("lin_header2"));
        lin_header2->setFrameShape(QFrame::VLine);
        lin_header2->setFrameShadow(QFrame::Sunken);

        lay_header->addWidget(lin_header2);

        btn_align_left = new QPushButton(wid_header);
        btn_align_left->setObjectName(QString::fromUtf8("btn_align_left"));
        btn_align_left->setText(QString::fromUtf8(""));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/Icons/Format/AlignLeft"), QSize(), QIcon::Normal, QIcon::Off);
        btn_align_left->setIcon(icon3);
        btn_align_left->setCheckable(true);

        lay_header->addWidget(btn_align_left);

        btn_align_center = new QPushButton(wid_header);
        btn_align_center->setObjectName(QString::fromUtf8("btn_align_center"));
        btn_align_center->setText(QString::fromUtf8(""));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/Icons/Format/AlignCenter"), QSize(), QIcon::Normal, QIcon::Off);
        btn_align_center->setIcon(icon4);
        btn_align_center->setCheckable(true);

        lay_header->addWidget(btn_align_center);

        btn_align_right = new QPushButton(wid_header);
        btn_align_right->setObjectName(QString::fromUtf8("btn_align_right"));
        btn_align_right->setText(QString::fromUtf8(""));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/Icons/Format/AlignRight"), QSize(), QIcon::Normal, QIcon::Off);
        btn_align_right->setIcon(icon5);
        btn_align_right->setCheckable(true);

        lay_header->addWidget(btn_align_right);

        btn_align_justify = new QPushButton(wid_header);
        btn_align_justify->setObjectName(QString::fromUtf8("btn_align_justify"));
        btn_align_justify->setText(QString::fromUtf8(""));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/Icons/Format/AlignFill"), QSize(), QIcon::Normal, QIcon::Off);
        btn_align_justify->setIcon(icon6);
        btn_align_justify->setCheckable(true);

        lay_header->addWidget(btn_align_justify);

        spa_header = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        lay_header->addItem(spa_header);


        lay_textwindow->addWidget(wid_header);

        wid_text = new QTextEdit(TextWindow);
        wid_text->setObjectName(QString::fromUtf8("wid_text"));

        lay_textwindow->addWidget(wid_text);

        wid_footer = new QWidget(TextWindow);
        wid_footer->setObjectName(QString::fromUtf8("wid_footer"));

        lay_textwindow->addWidget(wid_footer);


        retranslateUi(TextWindow);

        QMetaObject::connectSlotsByName(TextWindow);
    } // setupUi

    void retranslateUi(QWidget *TextWindow)
    {
        TextWindow->setWindowTitle(QCoreApplication::translate("TextWindow", "I18N_TEXTWINDOW", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TextWindow: public Ui_TextWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEXTWINDOW_H
