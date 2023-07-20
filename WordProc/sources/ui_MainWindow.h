/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *me_app_new;
    QAction *me_app_open;
    QAction *me_app_save;
    QAction *me_app_saveas;
    QAction *me_app_close;
    QAction *me_app_print;
    QAction *me_app_quit;
    QAction *me_edit_undo;
    QAction *me_edit_redo;
    QAction *me_edit_cut;
    QAction *me_edit_copy;
    QAction *me_edit_paste;
    QAction *me_edit_clear;
    QAction *me_edit_selectall;
    QAction *me_search_find;
    QAction *me_search_replace;
    QAction *me_search_goto;
    QAction *me_format_font;
    QAction *me_align_left;
    QAction *me_align_center;
    QAction *me_align_right;
    QAction *me_align_fill;
    QAction *me_window_next;
    QAction *me_window_prev;
    QAction *me_window_close;
    QAction *me_window_closeall;
    QAction *me_help_app;
    QAction *me_help_qt;
    QAction *me_lang_english;
    QAction *me_lang_dutch;
    QAction *me_lang_french;
    QAction *me_lang_italian;
    QAction *me_lang_german;
    QAction *me_lang_spanish;
    QAction *me_theme_default;
    QAction *me_theme_fusion;
    QAction *me_theme_windows;
    QAction *me_theme_macos;
    QAction *me_theme_qtcurve;
    QAction *me_theme_darkmode;
    QAction *me_search_find_next;
    QAction *me_search_find_prev;
    QAction *me_export_txt;
    QAction *me_export_html;
    QAction *me_export_odf;
    QAction *me_export_pdf;
    QAction *me_window_maximize;
    QAction *me_window_minimize;
    QWidget *wid_center;
    QGridLayout *lay_mainwindow;
    QMdiArea *wid_mdi;
    QMenuBar *wid_menubar;
    QMenu *mm_app;
    QMenu *sm_app_export;
    QMenu *mm_edit;
    QMenu *mm_window;
    QMenu *mm_help;
    QMenu *mm_search;
    QMenu *mm_config;
    QMenu *sm_config_lang;
    QMenu *sm_config_theme;
    QStatusBar *wid_statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 600);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Logos/App"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        me_app_new = new QAction(MainWindow);
        me_app_new->setObjectName(QString::fromUtf8("me_app_new"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Icons/Application/New"), QSize(), QIcon::Normal, QIcon::Off);
        me_app_new->setIcon(icon1);
        me_app_open = new QAction(MainWindow);
        me_app_open->setObjectName(QString::fromUtf8("me_app_open"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Icons/Application/Open"), QSize(), QIcon::Normal, QIcon::Off);
        me_app_open->setIcon(icon2);
        me_app_save = new QAction(MainWindow);
        me_app_save->setObjectName(QString::fromUtf8("me_app_save"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/Icons/Application/Save"), QSize(), QIcon::Normal, QIcon::Off);
        me_app_save->setIcon(icon3);
        me_app_saveas = new QAction(MainWindow);
        me_app_saveas->setObjectName(QString::fromUtf8("me_app_saveas"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/Icons/Application/SaveAs"), QSize(), QIcon::Normal, QIcon::Off);
        me_app_saveas->setIcon(icon4);
        me_app_close = new QAction(MainWindow);
        me_app_close->setObjectName(QString::fromUtf8("me_app_close"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/Icons/Application/Close"), QSize(), QIcon::Normal, QIcon::Off);
        me_app_close->setIcon(icon5);
        me_app_print = new QAction(MainWindow);
        me_app_print->setObjectName(QString::fromUtf8("me_app_print"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/Icons/Application/Print"), QSize(), QIcon::Normal, QIcon::Off);
        me_app_print->setIcon(icon6);
        me_app_quit = new QAction(MainWindow);
        me_app_quit->setObjectName(QString::fromUtf8("me_app_quit"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/Icons/Application/Exit"), QSize(), QIcon::Normal, QIcon::Off);
        me_app_quit->setIcon(icon7);
        me_edit_undo = new QAction(MainWindow);
        me_edit_undo->setObjectName(QString::fromUtf8("me_edit_undo"));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/Icons/Edit/Undo"), QSize(), QIcon::Normal, QIcon::Off);
        me_edit_undo->setIcon(icon8);
        me_edit_redo = new QAction(MainWindow);
        me_edit_redo->setObjectName(QString::fromUtf8("me_edit_redo"));
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/Icons/Edit/Redo"), QSize(), QIcon::Normal, QIcon::Off);
        me_edit_redo->setIcon(icon9);
        me_edit_cut = new QAction(MainWindow);
        me_edit_cut->setObjectName(QString::fromUtf8("me_edit_cut"));
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/Icons/Edit/Cut"), QSize(), QIcon::Normal, QIcon::Off);
        me_edit_cut->setIcon(icon10);
        me_edit_copy = new QAction(MainWindow);
        me_edit_copy->setObjectName(QString::fromUtf8("me_edit_copy"));
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/Icons/Edit/Copy"), QSize(), QIcon::Normal, QIcon::Off);
        me_edit_copy->setIcon(icon11);
        me_edit_paste = new QAction(MainWindow);
        me_edit_paste->setObjectName(QString::fromUtf8("me_edit_paste"));
        QIcon icon12;
        icon12.addFile(QString::fromUtf8(":/Icons/Edit/Paste"), QSize(), QIcon::Normal, QIcon::Off);
        me_edit_paste->setIcon(icon12);
        me_edit_clear = new QAction(MainWindow);
        me_edit_clear->setObjectName(QString::fromUtf8("me_edit_clear"));
        QIcon icon13;
        icon13.addFile(QString::fromUtf8(":/Icons/Edit/Clear"), QSize(), QIcon::Normal, QIcon::Off);
        me_edit_clear->setIcon(icon13);
        me_edit_selectall = new QAction(MainWindow);
        me_edit_selectall->setObjectName(QString::fromUtf8("me_edit_selectall"));
        QIcon icon14;
        icon14.addFile(QString::fromUtf8(":/Icons/Edit/SelectAll"), QSize(), QIcon::Normal, QIcon::Off);
        me_edit_selectall->setIcon(icon14);
        me_search_find = new QAction(MainWindow);
        me_search_find->setObjectName(QString::fromUtf8("me_search_find"));
        QIcon icon15;
        icon15.addFile(QString::fromUtf8(":/Icons/Search/Find"), QSize(), QIcon::Normal, QIcon::Off);
        me_search_find->setIcon(icon15);
        me_search_replace = new QAction(MainWindow);
        me_search_replace->setObjectName(QString::fromUtf8("me_search_replace"));
        QIcon icon16;
        icon16.addFile(QString::fromUtf8(":/Icons/Search/Replace"), QSize(), QIcon::Normal, QIcon::Off);
        me_search_replace->setIcon(icon16);
        me_search_goto = new QAction(MainWindow);
        me_search_goto->setObjectName(QString::fromUtf8("me_search_goto"));
        QIcon icon17;
        icon17.addFile(QString::fromUtf8(":/Icons/Search/Goto"), QSize(), QIcon::Normal, QIcon::Off);
        me_search_goto->setIcon(icon17);
        me_format_font = new QAction(MainWindow);
        me_format_font->setObjectName(QString::fromUtf8("me_format_font"));
        QIcon icon18;
        icon18.addFile(QString::fromUtf8(":/Icons/Format/Font"), QSize(), QIcon::Normal, QIcon::Off);
        me_format_font->setIcon(icon18);
        me_align_left = new QAction(MainWindow);
        me_align_left->setObjectName(QString::fromUtf8("me_align_left"));
        QIcon icon19;
        icon19.addFile(QString::fromUtf8(":/Icons/Format/AlignLeft"), QSize(), QIcon::Normal, QIcon::Off);
        me_align_left->setIcon(icon19);
        me_align_center = new QAction(MainWindow);
        me_align_center->setObjectName(QString::fromUtf8("me_align_center"));
        QIcon icon20;
        icon20.addFile(QString::fromUtf8(":/Icons/Format/AlignCenter"), QSize(), QIcon::Normal, QIcon::Off);
        me_align_center->setIcon(icon20);
        me_align_right = new QAction(MainWindow);
        me_align_right->setObjectName(QString::fromUtf8("me_align_right"));
        QIcon icon21;
        icon21.addFile(QString::fromUtf8(":/Icons/Format/AlignRight"), QSize(), QIcon::Normal, QIcon::Off);
        me_align_right->setIcon(icon21);
        me_align_fill = new QAction(MainWindow);
        me_align_fill->setObjectName(QString::fromUtf8("me_align_fill"));
        QIcon icon22;
        icon22.addFile(QString::fromUtf8(":/Icons/Format/AlignFill"), QSize(), QIcon::Normal, QIcon::Off);
        me_align_fill->setIcon(icon22);
        me_window_next = new QAction(MainWindow);
        me_window_next->setObjectName(QString::fromUtf8("me_window_next"));
        QIcon icon23;
        icon23.addFile(QString::fromUtf8(":/Icons/Window/Next"), QSize(), QIcon::Normal, QIcon::Off);
        me_window_next->setIcon(icon23);
        me_window_prev = new QAction(MainWindow);
        me_window_prev->setObjectName(QString::fromUtf8("me_window_prev"));
        QIcon icon24;
        icon24.addFile(QString::fromUtf8(":/Icons/Window/Prev"), QSize(), QIcon::Normal, QIcon::Off);
        me_window_prev->setIcon(icon24);
        me_window_close = new QAction(MainWindow);
        me_window_close->setObjectName(QString::fromUtf8("me_window_close"));
        QIcon icon25;
        icon25.addFile(QString::fromUtf8(":/Icons/Window/Close"), QSize(), QIcon::Normal, QIcon::Off);
        me_window_close->setIcon(icon25);
        me_window_closeall = new QAction(MainWindow);
        me_window_closeall->setObjectName(QString::fromUtf8("me_window_closeall"));
        me_window_closeall->setIcon(icon25);
        me_help_app = new QAction(MainWindow);
        me_help_app->setObjectName(QString::fromUtf8("me_help_app"));
        me_help_app->setIcon(icon);
        me_help_qt = new QAction(MainWindow);
        me_help_qt->setObjectName(QString::fromUtf8("me_help_qt"));
        QIcon icon26;
        icon26.addFile(QString::fromUtf8(":/Logos/Qt"), QSize(), QIcon::Normal, QIcon::Off);
        me_help_qt->setIcon(icon26);
        me_lang_english = new QAction(MainWindow);
        me_lang_english->setObjectName(QString::fromUtf8("me_lang_english"));
        me_lang_dutch = new QAction(MainWindow);
        me_lang_dutch->setObjectName(QString::fromUtf8("me_lang_dutch"));
        me_lang_french = new QAction(MainWindow);
        me_lang_french->setObjectName(QString::fromUtf8("me_lang_french"));
        me_lang_italian = new QAction(MainWindow);
        me_lang_italian->setObjectName(QString::fromUtf8("me_lang_italian"));
        me_lang_german = new QAction(MainWindow);
        me_lang_german->setObjectName(QString::fromUtf8("me_lang_german"));
        me_lang_spanish = new QAction(MainWindow);
        me_lang_spanish->setObjectName(QString::fromUtf8("me_lang_spanish"));
        me_theme_default = new QAction(MainWindow);
        me_theme_default->setObjectName(QString::fromUtf8("me_theme_default"));
        me_theme_fusion = new QAction(MainWindow);
        me_theme_fusion->setObjectName(QString::fromUtf8("me_theme_fusion"));
        me_theme_windows = new QAction(MainWindow);
        me_theme_windows->setObjectName(QString::fromUtf8("me_theme_windows"));
        me_theme_macos = new QAction(MainWindow);
        me_theme_macos->setObjectName(QString::fromUtf8("me_theme_macos"));
        me_theme_qtcurve = new QAction(MainWindow);
        me_theme_qtcurve->setObjectName(QString::fromUtf8("me_theme_qtcurve"));
        me_theme_darkmode = new QAction(MainWindow);
        me_theme_darkmode->setObjectName(QString::fromUtf8("me_theme_darkmode"));
        me_theme_darkmode->setCheckable(true);
        me_search_find_next = new QAction(MainWindow);
        me_search_find_next->setObjectName(QString::fromUtf8("me_search_find_next"));
        me_search_find_next->setIcon(icon15);
        me_search_find_prev = new QAction(MainWindow);
        me_search_find_prev->setObjectName(QString::fromUtf8("me_search_find_prev"));
        me_search_find_prev->setIcon(icon15);
        me_export_txt = new QAction(MainWindow);
        me_export_txt->setObjectName(QString::fromUtf8("me_export_txt"));
        me_export_html = new QAction(MainWindow);
        me_export_html->setObjectName(QString::fromUtf8("me_export_html"));
        me_export_odf = new QAction(MainWindow);
        me_export_odf->setObjectName(QString::fromUtf8("me_export_odf"));
        me_export_pdf = new QAction(MainWindow);
        me_export_pdf->setObjectName(QString::fromUtf8("me_export_pdf"));
        me_window_maximize = new QAction(MainWindow);
        me_window_maximize->setObjectName(QString::fromUtf8("me_window_maximize"));
        QIcon icon27;
        icon27.addFile(QString::fromUtf8(":/Icons/Window/Maximize"), QSize(), QIcon::Normal, QIcon::Off);
        me_window_maximize->setIcon(icon27);
        me_window_minimize = new QAction(MainWindow);
        me_window_minimize->setObjectName(QString::fromUtf8("me_window_minimize"));
        QIcon icon28;
        icon28.addFile(QString::fromUtf8(":/Icons/Window/Minimize"), QSize(), QIcon::Normal, QIcon::Off);
        me_window_minimize->setIcon(icon28);
        wid_center = new QWidget(MainWindow);
        wid_center->setObjectName(QString::fromUtf8("wid_center"));
        lay_mainwindow = new QGridLayout(wid_center);
        lay_mainwindow->setSpacing(2);
        lay_mainwindow->setObjectName(QString::fromUtf8("lay_mainwindow"));
        lay_mainwindow->setContentsMargins(2, 2, 2, 2);
        wid_mdi = new QMdiArea(wid_center);
        wid_mdi->setObjectName(QString::fromUtf8("wid_mdi"));

        lay_mainwindow->addWidget(wid_mdi, 0, 0, 1, 1);

        MainWindow->setCentralWidget(wid_center);
        wid_menubar = new QMenuBar(MainWindow);
        wid_menubar->setObjectName(QString::fromUtf8("wid_menubar"));
        wid_menubar->setGeometry(QRect(0, 0, 800, 20));
        mm_app = new QMenu(wid_menubar);
        mm_app->setObjectName(QString::fromUtf8("mm_app"));
        sm_app_export = new QMenu(mm_app);
        sm_app_export->setObjectName(QString::fromUtf8("sm_app_export"));
        QIcon icon29;
        icon29.addFile(QString::fromUtf8(":/Icons/Application/Export"), QSize(), QIcon::Normal, QIcon::Off);
        sm_app_export->setIcon(icon29);
        mm_edit = new QMenu(wid_menubar);
        mm_edit->setObjectName(QString::fromUtf8("mm_edit"));
        mm_window = new QMenu(wid_menubar);
        mm_window->setObjectName(QString::fromUtf8("mm_window"));
        mm_help = new QMenu(wid_menubar);
        mm_help->setObjectName(QString::fromUtf8("mm_help"));
        mm_search = new QMenu(wid_menubar);
        mm_search->setObjectName(QString::fromUtf8("mm_search"));
        mm_config = new QMenu(wid_menubar);
        mm_config->setObjectName(QString::fromUtf8("mm_config"));
        sm_config_lang = new QMenu(mm_config);
        sm_config_lang->setObjectName(QString::fromUtf8("sm_config_lang"));
        sm_config_theme = new QMenu(mm_config);
        sm_config_theme->setObjectName(QString::fromUtf8("sm_config_theme"));
        MainWindow->setMenuBar(wid_menubar);
        wid_statusbar = new QStatusBar(MainWindow);
        wid_statusbar->setObjectName(QString::fromUtf8("wid_statusbar"));
        MainWindow->setStatusBar(wid_statusbar);

        wid_menubar->addAction(mm_app->menuAction());
        wid_menubar->addAction(mm_edit->menuAction());
        wid_menubar->addAction(mm_search->menuAction());
        wid_menubar->addAction(mm_window->menuAction());
        wid_menubar->addAction(mm_config->menuAction());
        wid_menubar->addAction(mm_help->menuAction());
        mm_app->addAction(me_app_new);
        mm_app->addSeparator();
        mm_app->addAction(me_app_open);
        mm_app->addAction(me_app_save);
        mm_app->addAction(me_app_saveas);
        mm_app->addSeparator();
        mm_app->addAction(sm_app_export->menuAction());
        mm_app->addSeparator();
        mm_app->addAction(me_app_close);
        mm_app->addSeparator();
        mm_app->addAction(me_app_print);
        mm_app->addSeparator();
        mm_app->addAction(me_app_quit);
        sm_app_export->addAction(me_export_txt);
        sm_app_export->addAction(me_export_html);
        sm_app_export->addAction(me_export_odf);
        sm_app_export->addAction(me_export_pdf);
        mm_edit->addAction(me_edit_undo);
        mm_edit->addAction(me_edit_redo);
        mm_edit->addSeparator();
        mm_edit->addAction(me_edit_cut);
        mm_edit->addAction(me_edit_copy);
        mm_edit->addAction(me_edit_paste);
        mm_edit->addAction(me_edit_clear);
        mm_edit->addSeparator();
        mm_edit->addAction(me_edit_selectall);
        mm_window->addAction(me_window_next);
        mm_window->addAction(me_window_prev);
        mm_window->addSeparator();
        mm_window->addAction(me_window_maximize);
        mm_window->addAction(me_window_minimize);
        mm_window->addSeparator();
        mm_window->addAction(me_window_close);
        mm_window->addAction(me_window_closeall);
        mm_help->addAction(me_help_app);
        mm_help->addAction(me_help_qt);
        mm_search->addAction(me_search_find);
        mm_search->addAction(me_search_find_next);
        mm_search->addAction(me_search_find_prev);
        mm_search->addSeparator();
        mm_search->addAction(me_search_replace);
        mm_search->addAction(me_search_goto);
        mm_config->addAction(sm_config_lang->menuAction());
        mm_config->addAction(sm_config_theme->menuAction());
        sm_config_lang->addAction(me_lang_english);
        sm_config_lang->addSeparator();
        sm_config_lang->addAction(me_lang_dutch);
        sm_config_lang->addAction(me_lang_french);
        sm_config_lang->addAction(me_lang_german);
        sm_config_lang->addAction(me_lang_italian);
        sm_config_lang->addAction(me_lang_spanish);
        sm_config_theme->addAction(me_theme_fusion);
        sm_config_theme->addSeparator();
        sm_config_theme->addAction(me_theme_macos);
        sm_config_theme->addAction(me_theme_qtcurve);
        sm_config_theme->addAction(me_theme_windows);
        sm_config_theme->addSeparator();
        sm_config_theme->addAction(me_theme_darkmode);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "I18N_MAINWINDOW", nullptr));
        me_app_new->setText(QCoreApplication::translate("MainWindow", "I18N_ME_APP_NEW", nullptr));
        me_app_open->setText(QCoreApplication::translate("MainWindow", "I18N_ME_APP_OPEN", nullptr));
        me_app_save->setText(QCoreApplication::translate("MainWindow", "I18N_ME_APP_SAVE", nullptr));
        me_app_saveas->setText(QCoreApplication::translate("MainWindow", "I18N_ME_APP_SAVEAS", nullptr));
        me_app_close->setText(QCoreApplication::translate("MainWindow", "I18N_ME_APP_CLOSE", nullptr));
        me_app_print->setText(QCoreApplication::translate("MainWindow", "I18N_ME_APP_PRINT", nullptr));
        me_app_quit->setText(QCoreApplication::translate("MainWindow", "I18N_ME_APP_QUIT", nullptr));
        me_edit_undo->setText(QCoreApplication::translate("MainWindow", "I18N_ME_EDIT_UNDO", nullptr));
        me_edit_redo->setText(QCoreApplication::translate("MainWindow", "I18N_ME_EDIT_REDO", nullptr));
        me_edit_cut->setText(QCoreApplication::translate("MainWindow", "I18N_ME_EDIT_CUT", nullptr));
        me_edit_copy->setText(QCoreApplication::translate("MainWindow", "I18N_ME_EDIT_COPY", nullptr));
        me_edit_paste->setText(QCoreApplication::translate("MainWindow", "I18N_ME_EDIT_PASTE", nullptr));
        me_edit_clear->setText(QCoreApplication::translate("MainWindow", "I18N_ME_EDIT_CLEAR", nullptr));
        me_edit_selectall->setText(QCoreApplication::translate("MainWindow", "I18N_ME_EDIT_SELECTALL", nullptr));
        me_search_find->setText(QCoreApplication::translate("MainWindow", "I18N_ME_SEARCH_FIND", nullptr));
        me_search_replace->setText(QCoreApplication::translate("MainWindow", "I18N_ME_SEARCH_REPLACE", nullptr));
        me_search_goto->setText(QCoreApplication::translate("MainWindow", "I18N_ME_SEARCH_GOTO", nullptr));
        me_format_font->setText(QCoreApplication::translate("MainWindow", "I18N_ME_FORMAT_FONT", nullptr));
        me_align_left->setText(QCoreApplication::translate("MainWindow", "I18N_ME_ALIGN_LEFT", nullptr));
        me_align_center->setText(QCoreApplication::translate("MainWindow", "I18N_ME_ALIGN_CENTER", nullptr));
        me_align_right->setText(QCoreApplication::translate("MainWindow", "I18N_ME_ALIGN_RIGHT", nullptr));
        me_align_fill->setText(QCoreApplication::translate("MainWindow", "I18N_ME_ALIGN_FILL", nullptr));
        me_window_next->setText(QCoreApplication::translate("MainWindow", "I18N_ME_WINDOW_NEXT", nullptr));
        me_window_prev->setText(QCoreApplication::translate("MainWindow", "I18N_ME_WINDOW_PREV", nullptr));
        me_window_close->setText(QCoreApplication::translate("MainWindow", "I18N_ME_WINDOW_CLOSE", nullptr));
        me_window_closeall->setText(QCoreApplication::translate("MainWindow", "I18N_ME_WINDOW_CLOSEALL", nullptr));
        me_help_app->setText(QCoreApplication::translate("MainWindow", "I18N_ME_HELP_APP", nullptr));
        me_help_qt->setText(QCoreApplication::translate("MainWindow", "I18N_ME_HELP_QT", nullptr));
        me_lang_english->setText(QCoreApplication::translate("MainWindow", "I18N_ME_LANG_ENGLISH", nullptr));
        me_lang_dutch->setText(QCoreApplication::translate("MainWindow", "I18N_ME_LANG_DUTCH", nullptr));
        me_lang_french->setText(QCoreApplication::translate("MainWindow", "I18N_ME_LANG_FRENCH", nullptr));
        me_lang_italian->setText(QCoreApplication::translate("MainWindow", "I18N_ME_LANG_ITALIAN", nullptr));
        me_lang_german->setText(QCoreApplication::translate("MainWindow", "I18N_ME_LANG_GERMAN", nullptr));
        me_lang_spanish->setText(QCoreApplication::translate("MainWindow", "I18N_ME_LANG_SPANISH", nullptr));
        me_theme_default->setText(QCoreApplication::translate("MainWindow", "I18N_ME_THEME_DEFAULT", nullptr));
        me_theme_fusion->setText(QCoreApplication::translate("MainWindow", "I18N_ME_THEME_FUSION", nullptr));
        me_theme_windows->setText(QCoreApplication::translate("MainWindow", "I18N_ME_THEME_WINDOWS", nullptr));
        me_theme_macos->setText(QCoreApplication::translate("MainWindow", "I18N_ME_THEME_MACOS", nullptr));
        me_theme_qtcurve->setText(QCoreApplication::translate("MainWindow", "I18N_ME_THEME_QTCURVE", nullptr));
        me_theme_darkmode->setText(QCoreApplication::translate("MainWindow", "I18N_ME_THEME_DARKMODE", nullptr));
        me_search_find_next->setText(QCoreApplication::translate("MainWindow", "I18N_ME_SEARCH_FIND_NEXT", nullptr));
        me_search_find_prev->setText(QCoreApplication::translate("MainWindow", "I18N_ME_SEARCH_FIND_PREV", nullptr));
        me_export_txt->setText(QCoreApplication::translate("MainWindow", "I18N_ME_EXPORT_TXT", nullptr));
        me_export_html->setText(QCoreApplication::translate("MainWindow", "I18N_ME_EXPORT_HTML", nullptr));
        me_export_odf->setText(QCoreApplication::translate("MainWindow", "I18N_ME_EXPORT_ODF", nullptr));
        me_export_pdf->setText(QCoreApplication::translate("MainWindow", "I18N_ME_EXPORT_PDF", nullptr));
        me_window_maximize->setText(QCoreApplication::translate("MainWindow", "I18N_ME_WINDOW_MAXIMIZE", nullptr));
        me_window_minimize->setText(QCoreApplication::translate("MainWindow", "I18N_ME_WINDOW_MINIMIZE", nullptr));
        mm_app->setTitle(QCoreApplication::translate("MainWindow", "I18N_MM_APP", nullptr));
        sm_app_export->setTitle(QCoreApplication::translate("MainWindow", "I18N_SM_APP_EXPORT", nullptr));
        mm_edit->setTitle(QCoreApplication::translate("MainWindow", "I18N_MM_EDIT", nullptr));
        mm_window->setTitle(QCoreApplication::translate("MainWindow", "I18N_MM_WINDOW", nullptr));
        mm_help->setTitle(QCoreApplication::translate("MainWindow", "I18N_MM_HELP", nullptr));
        mm_search->setTitle(QCoreApplication::translate("MainWindow", "I18N_MM_SEARCH", nullptr));
        mm_config->setTitle(QCoreApplication::translate("MainWindow", "I18N_MM_CONFIG", nullptr));
        sm_config_lang->setTitle(QCoreApplication::translate("MainWindow", "I18N_SM_CONFIG_LANG", nullptr));
        sm_config_theme->setTitle(QCoreApplication::translate("MainWindow", "I18N_SM_CONFIG_THEME", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
