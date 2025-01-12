#ifndef WINDOW_H
#define WINDOW_H

#include "framelesswindow.h"

#include <QSystemTrayIcon>

QT_BEGIN_NAMESPACE
class QSettings;
class QToolButton;
class QLabel;
QT_END_NAMESPACE

class Window: public FramelessWindow
{
    Q_OBJECT
public:
    explicit Window(QWidget *const parent = nullptr);
    virtual ~Window() = default;

signals:
    void themeChanged(bool);

    void newPasswordSignal();
    void cutPasswordSignal(bool);

    void saveSettingsSignal();

    void newAndCopyPasswordSignal();

protected:
    virtual void closeEvent(QCloseEvent *const event) override;

    qsizetype getCurrentIndexTheme();

    void updateIcons();

    QSettings *userSettings;
    QSystemTrayIcon *myTrayIcon;
    const QString appName, appVersion;

private:
    void initializeMembers();

    void createTitleBar();
    void createMenus();
    void createActions();
    void configurateActions();
    void configurateMenus();
    void configurateTitleBar();

    void createTrayIcon();
    void configurateTrayIcon();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

    void connectSignals();

    void changeLanguage();
    void restart();
    void saveSettings();
    void changeTheme();

    void about();
    void credits();

    qsizetype currentIndexLanguage, currentIndexTheme;

    QWidget *titleBar;
    QToolButton *popUpButton, *minimizeButton, *closeButton;
    QLabel *appIcon, *appTitleName;
    QMenu *appMenu,
        *languagesMenu,
        *themesMenu,
        *shortCutsMenu,
        *helpMenu,
        *myTrayIconMenu;
    QList<QAction *> languageActionList, themeActionList;
    QAction *generatePasswordAction, *cutPasswordAction, *copyPasswordAction,
        *customTitleBarAction,
        *aboutAction, *creditsAction, *aboutQtAction,
        *exitAction,
        *showProgramAction, *newAndCopyPasswordAction;
};

#endif // WINDOW_H
