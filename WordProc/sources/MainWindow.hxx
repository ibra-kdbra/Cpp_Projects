#pragma once

#include <QMainWindow>
#include <QPalette>
#include <QString>
#include "ui_MainWindow.h"
#include "Types.hxx"

class QSettings;
class QTranslator;
class SearchWindow;
class TextWindow;

class MainWindow : public QMainWindow, protected Ui::MainWindow {
    Q_OBJECT
public:
    //--- public constructors ---
    MainWindow(QWidget *parent = nullptr);
    MainWindow(const MainWindow &rhs) = delete;
    MainWindow(MainWindow &&rhs) = delete;
    virtual ~MainWindow();

    //--- public operators ---
    MainWindow &operator=(const MainWindow &rhs) = delete;
    MainWindow &operator=(MainWindow &&rhs) = delete;

protected:
    //--- protected methods ---
    virtual void changeEvent(QEvent *event) override final;
    virtual void closeEvent(QCloseEvent *event) override final;

    void setupActions();
    void setLanguage(const Types::Language lang = Types::Language::English);
    void setTheme(const Types::Theme theme = Types::Theme::Fusion);
    void setDarkmode(const bool mode = true);
    void setIcons(const QString &prefix);

    TextWindow *createTextWindow(const QString &filename = "", const bool empty = false);
    TextWindow *currentTextWindow();

private:
    //--- private properties ---
    QTranslator *_trans;
    QSettings *_conf;
    SearchWindow *_search;
    QPalette _opalette;
    Types::Language _lang;
    Types::Theme _theme;
    bool _darkmode;

signals:
    //--- Qt signals ---
    void switchIcons(const QString &prefix);
};
