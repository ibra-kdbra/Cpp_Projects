/********************************************************************************
** Form generated from reading UI file 'SearchWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SEARCHWINDOW_H
#define UI_SEARCHWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SearchWindow
{
public:
    QVBoxLayout *lay_main;
    QWidget *wid_search;
    QHBoxLayout *horizontalLayout;
    QLabel *lab_search;
    QLineEdit *wid_lineedit;
    QGroupBox *wid_options;
    QGridLayout *gridLayout;
    QCheckBox *chk_case_sensitive;
    QCheckBox *chk_whole_words;
    QWidget *wid_buttons;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *spa_buttons;
    QPushButton *btn_close;
    QPushButton *btn_prev;
    QPushButton *btn_next;

    void setupUi(QWidget *SearchWindow)
    {
        if (SearchWindow->objectName().isEmpty())
            SearchWindow->setObjectName(QString::fromUtf8("SearchWindow"));
        SearchWindow->resize(475, 117);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(SearchWindow->sizePolicy().hasHeightForWidth());
        SearchWindow->setSizePolicy(sizePolicy);
        lay_main = new QVBoxLayout(SearchWindow);
        lay_main->setSpacing(2);
        lay_main->setObjectName(QString::fromUtf8("lay_main"));
        lay_main->setContentsMargins(2, 2, 2, 2);
        wid_search = new QWidget(SearchWindow);
        wid_search->setObjectName(QString::fromUtf8("wid_search"));
        horizontalLayout = new QHBoxLayout(wid_search);
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        lab_search = new QLabel(wid_search);
        lab_search->setObjectName(QString::fromUtf8("lab_search"));

        horizontalLayout->addWidget(lab_search);

        wid_lineedit = new QLineEdit(wid_search);
        wid_lineedit->setObjectName(QString::fromUtf8("wid_lineedit"));

        horizontalLayout->addWidget(wid_lineedit);


        lay_main->addWidget(wid_search);

        wid_options = new QGroupBox(SearchWindow);
        wid_options->setObjectName(QString::fromUtf8("wid_options"));
        gridLayout = new QGridLayout(wid_options);
        gridLayout->setSpacing(2);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        chk_case_sensitive = new QCheckBox(wid_options);
        chk_case_sensitive->setObjectName(QString::fromUtf8("chk_case_sensitive"));

        gridLayout->addWidget(chk_case_sensitive, 0, 0, 1, 1);

        chk_whole_words = new QCheckBox(wid_options);
        chk_whole_words->setObjectName(QString::fromUtf8("chk_whole_words"));

        gridLayout->addWidget(chk_whole_words, 0, 1, 1, 1);


        lay_main->addWidget(wid_options);

        wid_buttons = new QWidget(SearchWindow);
        wid_buttons->setObjectName(QString::fromUtf8("wid_buttons"));
        horizontalLayout_2 = new QHBoxLayout(wid_buttons);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        spa_buttons = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(spa_buttons);

        btn_close = new QPushButton(wid_buttons);
        btn_close->setObjectName(QString::fromUtf8("btn_close"));

        horizontalLayout_2->addWidget(btn_close);

        btn_prev = new QPushButton(wid_buttons);
        btn_prev->setObjectName(QString::fromUtf8("btn_prev"));

        horizontalLayout_2->addWidget(btn_prev);

        btn_next = new QPushButton(wid_buttons);
        btn_next->setObjectName(QString::fromUtf8("btn_next"));

        horizontalLayout_2->addWidget(btn_next);


        lay_main->addWidget(wid_buttons);


        retranslateUi(SearchWindow);

        QMetaObject::connectSlotsByName(SearchWindow);
    } // setupUi

    void retranslateUi(QWidget *SearchWindow)
    {
        SearchWindow->setWindowTitle(QCoreApplication::translate("SearchWindow", "I18N_SEARCHWINDOW", nullptr));
        lab_search->setText(QCoreApplication::translate("SearchWindow", "I18N_SEARCH_FOR", nullptr));
        wid_options->setTitle(QCoreApplication::translate("SearchWindow", "I18N_SEARCH_OPTIONS", nullptr));
        chk_case_sensitive->setText(QCoreApplication::translate("SearchWindow", "I18N_SEARCH_CASE_SENSITIVE", nullptr));
        chk_whole_words->setText(QCoreApplication::translate("SearchWindow", "I18N_SEARCH_WHOLE_WORDS", nullptr));
        btn_close->setText(QCoreApplication::translate("SearchWindow", "I18N_SEARCH_CLOSE", nullptr));
        btn_prev->setText(QCoreApplication::translate("SearchWindow", "I18N_SEARCH_PREV", nullptr));
        btn_next->setText(QCoreApplication::translate("SearchWindow", "I18N_SEARCH_NEXT", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SearchWindow: public Ui_SearchWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SEARCHWINDOW_H
