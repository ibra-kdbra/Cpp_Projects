#include "window.h"
#include "icons.h"

#include <QApplication>
#include <QCloseEvent>
#include <QToolButton>
#include <QMenu>
#include <QLabel>
#include <QSettings>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QProcess>
#include <QClipboard>
#include <QFile>
#include <QDebug>

Window::Window(QWidget *const parent)
    : FramelessWindow{ parent },
    appName{ qApp->applicationName() },
    appVersion{ qApp->applicationVersion() }
{
    initializeMembers();

    createTitleBar();
    createMenus();
    createActions();
    configurateActions();
    configurateMenus();
    configurateTitleBar();

    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        createTrayIcon();
        configurateTrayIcon();
    }

    updateIcons();
    connectSignals();
}

void Window::closeEvent(QCloseEvent *const event)
{
    qDebug() << "Close event triggered";
    saveSettings();
    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        hide();
        event->ignore();
        qDebug() << "Window hidden and event ignored";

        myTrayIcon->showMessage(
            "GenXPassword",
            "GenXPassword has been minimized to the tray. To quit the application, right-click the tray icon and select 'Quit'.",
            QSystemTrayIcon::Information,
            3000
        );
    } else {
        qDebug() << "Quitting application";
        QApplication::quit();
    }
}

qsizetype Window::getCurrentIndexTheme()
{
    return currentIndexTheme;
}

void Window::updateIcons()
{
    if (currentIndexTheme) {
        popUpButton->setIcon(MENU_ICON_LIGHT);

        languagesMenu->setIcon(TRANSLATE_ICON_LIGHT);
        languageActionList[currentIndexLanguage]->setIcon(CHECK_ICON_LIGHT);

        themesMenu->setIcon(PALETTE_ICON_LIGHT);
        themeActionList[currentIndexTheme]->setIcon(CHECK_ICON_LIGHT);

        shortCutsMenu->setIcon(SHORTCUT_ICON_LIGHT);
        generatePasswordAction->setIcon(REFRESH_ICON_LIGHT);
        cutPasswordAction->setIcon(CUT_ICON_LIGHT);
        copyPasswordAction->setIcon(COPY_ICON_LIGHT);

        customTitleBarAction->setIcon(customTitleBarAction->isChecked()? DONE_ICON_LIGHT: QIcon());

        helpMenu->setIcon(HELP_ICON_LIGHT);
        aboutAction->setIcon(INFO_ICON_LIGHT);
        creditsAction->setIcon(COPYRIGHT_ICON_LIGHT);

        exitAction->setIcon(EXIT_ICON_LIGHT);

        appIcon->setPixmap(APP_ICON_LIGHT.pixmap(35));

        minimizeButton->setIcon(MINIMIZE_ICON_LIGHT);
        closeButton->setIcon(CLOSE_ICON_LIGHT);

        if(QSystemTrayIcon::isSystemTrayAvailable()) {
            showProgramAction->setIcon(APP_ICON_LIGHT);
            newAndCopyPasswordAction->setIcon(REFRESH_ICON_LIGHT);
        }
    } else {
        popUpButton->setIcon(MENU_ICON_DARK);

        languagesMenu->setIcon(TRANSLATE_ICON_DARK);
        languageActionList[currentIndexLanguage]->setIcon(CHECK_ICON_DARK);

        themesMenu->setIcon(PALETTE_ICON_DARK);
        themeActionList[currentIndexTheme]->setIcon(CHECK_ICON_DARK);

        shortCutsMenu->setIcon(SHORTCUT_ICON_DARK);
        generatePasswordAction->setIcon(REFRESH_ICON_DARK);
        cutPasswordAction->setIcon(CUT_ICON_DARK);
        copyPasswordAction->setIcon(COPY_ICON_DARK);

        customTitleBarAction->setIcon(customTitleBarAction->isChecked()? DONE_ICON_DARK: QIcon());

        helpMenu->setIcon(HELP_ICON_DARK);
        aboutAction->setIcon(INFO_ICON_DARK);
        creditsAction->setIcon(COPYRIGHT_ICON_DARK);

        exitAction->setIcon(EXIT_ICON_DARK);

        appIcon->setPixmap(APP_ICON_DARK.pixmap(35));

        minimizeButton->setIcon(MINIMIZE_ICON_DARK);
        closeButton->setIcon(CLOSE_ICON_DARK);

        if(QSystemTrayIcon::isSystemTrayAvailable()) {
            showProgramAction->setIcon(APP_ICON_DARK);
            newAndCopyPasswordAction->setIcon(REFRESH_ICON_DARK);
        }
    }
}

void Window::initializeMembers()
{
    userSettings = new QSettings(QCoreApplication::applicationDirPath()
                                     + "/config/Settings.ini",
                                 QSettings::Format::IniFormat);

    currentIndexLanguage = userSettings->value("Settings/IndexLanguage").toInt();
    currentIndexTheme = userSettings->value("Settings/IndexTheme").toInt();
}

void Window::createTitleBar()
{
    titleBar = new QWidget(this);

    popUpButton = new QToolButton(titleBar),
        appMenu = new QMenu(popUpButton);

    appIcon = new QLabel(titleBar);
    appTitleName = new QLabel( appName, titleBar);

    minimizeButton = new QToolButton(titleBar);
    closeButton = new QToolButton(titleBar);
}

void Window::createMenus()
{
    languagesMenu = new QMenu(tr("&Languages"), appMenu);
    themesMenu = new QMenu(tr("&Themes"), appMenu);
    shortCutsMenu = new QMenu(tr("&Shortcuts"), appMenu);
    helpMenu = new QMenu(tr("&Help"), appMenu);
}

void Window::createActions()
{
    languageActionList ={ new QAction(tr("[en] English"), languagesMenu),
                          new QAction(tr("[es] Español"), languagesMenu),
                          new QAction(tr("[fr] French"), languagesMenu),
                          new QAction(tr("[ja] Japanese"), languagesMenu),
                          new QAction(tr("[ar] Arabic"), languagesMenu),};

    themeActionList ={ new QAction(tr("Light"), themesMenu),
           new QAction(tr("Dark"), themesMenu) };

    generatePasswordAction = new QAction(shortCutsMenu);
    cutPasswordAction = new QAction(shortCutsMenu);
    copyPasswordAction = new QAction(shortCutsMenu);

    customTitleBarAction = new QAction(tr("&Custom Title Bar"), appMenu);

    aboutAction = new QAction(tr("&About %1").arg(appName), helpMenu);
    creditsAction = new QAction(tr("&Credits"), helpMenu);
    aboutQtAction = new QAction(tr("About &Qt"), helpMenu);

    exitAction = new QAction(tr("E&xit"), appMenu);
}

void Window::configurateActions()
{
    themeActionList[0]->setObjectName("light.css");
    themeActionList[1]->setObjectName("dark.css");

    generatePasswordAction->setText(tr("&Generate Password"));
    generatePasswordAction->setShortcut(QKeySequence::New);

    cutPasswordAction->setText(tr("Cut Password"));
    cutPasswordAction->setObjectName("cut");
    cutPasswordAction->setShortcut(QKeySequence::Cut);

    copyPasswordAction->setText(tr("&Copy Password"));
    copyPasswordAction->setShortcut(QKeySequence::Copy);

    customTitleBarAction->setCheckable(true);
    customTitleBarAction->setChecked(true);

    aboutQtAction->setIcon(QT_ICON);

    exitAction->setShortcut(QKeySequence::Quit);
}

void Window::configurateMenus()
{
    languagesMenu->addActions(languageActionList);

    themesMenu->addActions(themeActionList);

    shortCutsMenu->addAction(generatePasswordAction);
    shortCutsMenu->addAction(cutPasswordAction);
    shortCutsMenu->addAction(copyPasswordAction);

    helpMenu->addAction(aboutAction);
    helpMenu->addAction(creditsAction);
    helpMenu->addAction(aboutQtAction);
}

void Window::configurateTitleBar()
{
    popUpButton->setPopupMode(QToolButton::InstantPopup);
    popUpButton->setAutoRaise(true);
    popUpButton->setMenu(appMenu);

    appMenu->addMenu(languagesMenu);
    appMenu->addMenu(themesMenu);
    appMenu->addMenu(shortCutsMenu);
    appMenu->addAction(customTitleBarAction);
    appMenu->addMenu(helpMenu);
    appMenu->addSeparator();
    appMenu->addAction(exitAction);

    QHBoxLayout *const hLayoutBar{ new QHBoxLayout };
    hLayoutBar->setContentsMargins(0, 0, 0, 0);

    hLayoutBar->addWidget(popUpButton);
    hLayoutBar->addStretch();
    hLayoutBar->addWidget(appIcon);
    hLayoutBar->addWidget(appTitleName);
    hLayoutBar->addStretch();

    minimizeButton->setAutoRaise(true);
    closeButton->setAutoRaise(true);

    QHBoxLayout *const buttonsHLayout{ new QHBoxLayout };
    buttonsHLayout->setContentsMargins(0, 0, 0, 0);
    buttonsHLayout->setSpacing(0);
    buttonsHLayout->addWidget(minimizeButton);
    buttonsHLayout->addWidget(closeButton);

    hLayoutBar->addLayout(buttonsHLayout);

    titleBar->setLayout(hLayoutBar);
    setTitleBar(titleBar);
}

void Window::createTrayIcon()
{
    
    if (!myTrayIcon) {
        myTrayIcon = new QSystemTrayIcon(this);
    }

    myTrayIcon->setIcon(QIcon(":/icons/appIcon.ico"));

    myTrayIcon->setToolTip(appName);

    myTrayIconMenu = new QMenu(this);

    showProgramAction = new QAction(tr("Show %1").arg(appName), this);
    newAndCopyPasswordAction = new QAction(tr("New+Copy Password"), this);
    exitAction = new QAction(tr("Quit"), this);

    myTrayIconMenu->addAction(showProgramAction);
    myTrayIconMenu->addSeparator();
    myTrayIconMenu->addAction(newAndCopyPasswordAction);
    myTrayIconMenu->addSeparator();
    myTrayIconMenu->addAction(exitAction);

    myTrayIcon->setContextMenu(myTrayIconMenu);
    configurateTrayIcon();
    showProgramAction = new QAction(tr("Show %1").arg(appName), myTrayIcon);
    newAndCopyPasswordAction = new QAction(tr("New+Copy Password"), myTrayIcon);
}

void Window::configurateTrayIcon()
{
    myTrayIconMenu->addAction(showProgramAction);
    myTrayIconMenu->addSeparator();
    myTrayIconMenu->addAction(newAndCopyPasswordAction);
    myTrayIconMenu->addSeparator();
    myTrayIconMenu->addAction(exitAction);

    myTrayIcon->setContextMenu(myTrayIconMenu);
    myTrayIcon->show();
    myTrayIcon->setToolTip(appName);

    connect(myTrayIcon, &QSystemTrayIcon::activated, this, &Window::iconActivated);
    connect(showProgramAction, &QAction::triggered, this, [this] {
        if (!isActiveWindow() || isHidden() || isMinimized()) {
            activateWindow();
            showNormal();
        }
    });
    connect(newAndCopyPasswordAction, &QAction::triggered, this, [this] { emit newAndCopyPasswordSignal(); });
    connect(exitAction, &QAction::triggered, qApp, &QApplication::quit); // Ensure exitAction quits the app
}

void Window::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        if (!isActiveWindow() || isHidden() || isMinimized()) {
            activateWindow();
            showNormal();
        }
    }
}

void Window::connectSignals()
{
    for(const QAction *const languageAction: languageActionList)
        connect(languageAction, &QAction::triggered, this, &Window::changeLanguage);

    for(const QAction *const themeAction: themeActionList)
        connect(themeAction, &QAction::triggered, this, &Window::changeTheme);

    connect(generatePasswordAction, &QAction::triggered, this, [this] { emit newPasswordSignal(); });
    connect(cutPasswordAction, &QAction::triggered, this, [this] { emit cutPasswordSignal(true); });
    connect(copyPasswordAction, &QAction::triggered, this, [this] { emit cutPasswordSignal(false); });

    connect(customTitleBarAction, &QAction::triggered, this, [this] (bool checked) {
        setWindowFlag(Qt::FramelessWindowHint, checked);
        setWindowFlag(Qt::WindowMaximizeButtonHint, checked);

        setTitleBar(checked? titleBar: nullptr);
        appIcon->setVisible(checked);
        appTitleName->setVisible(checked);
        minimizeButton->setVisible(checked);
        closeButton->setVisible(checked);
        customTitleBarAction->setIcon(checked? currentIndexTheme? DONE_ICON_LIGHT: DONE_ICON_DARK
            : QIcon());

        showNormal();
    });

    connect(aboutAction, &QAction::triggered, this, &Window::about);
    connect(creditsAction, &QAction::triggered, this, &Window::credits);
    connect(aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);

    connect(exitAction, &QAction::triggered, this, &Window::close);

    connect(minimizeButton, &QToolButton::clicked,
            this, [this] { setWindowState(Qt::WindowMinimized); });
    connect(closeButton, &QToolButton::clicked, this, &Window::close);
}

void Window::changeLanguage()
{
    const qsizetype senderLanguageIndex{ languageActionList.indexOf(static_cast<QAction *>(sender())) };

    const static qsizetype programLanguage{ currentIndexLanguage };

    if (programLanguage != senderLanguageIndex) {
        languageActionList[currentIndexLanguage]->setIcon(QIcon());
        languageActionList[senderLanguageIndex]
            ->setIcon(currentIndexTheme? WARNING_ICON_LIGHT: WARNING_ICON_DARK);
        currentIndexLanguage = senderLanguageIndex;
        restart();
    } else {
        languageActionList[currentIndexLanguage]->setIcon(QIcon());
        languageActionList[senderLanguageIndex]
            ->setIcon(currentIndexTheme? CHECK_ICON_LIGHT: CHECK_ICON_DARK);
        currentIndexLanguage = senderLanguageIndex;
    }
}

void Window::restart()
{
    QMessageBox question{ this };
    question.setWindowTitle(tr("Restart Required"));
    question.setText(tr("You will need to restart in order to"
                        " use your new language setting\n"
                        "Your password will be lost if you do not "
                        "save it before restarting."));
    question.addButton(tr("Restart Now"), QMessageBox::YesRole);
    question.buttons().at(0)->setIcon(currentIndexTheme? REFRESH_ICON_LIGHT: REFRESH_ICON_DARK);
    question.addButton(tr("Restart Later"), QMessageBox::NoRole);
    question.buttons().at(1)->setIcon(currentIndexTheme? OFF_ICON_LIGHT: OFF_ICON_DARK);
    const bool isRestartNow{ !question.exec() };
    if (isRestartNow) {
        const QProcess process;
        const QString program{ qApp->arguments()[0] },
            workingDirectory{ qApp->applicationDirPath() };
        QStringList arguments{ qApp->arguments() };
        arguments.removeAt(0);
        const bool processHasError{ !process.startDetached(program, arguments, workingDirectory) };
        if (processHasError) {
            const QProcess::ProcessError error{ process.error() };
            QMessageBox errorBox{ this };
            errorBox.setWindowTitle(tr("Error"));
            errorBox.setText(tr("Error: \"enum QProcess::ProcessError\"\n"
                                "Value error: %1\n"
                                "The program can still be used\n"
                                "but cannot be restarted due to an error.\n\n"
                                "Do you want copy this error "
                                "in your clipboard?").arg(error));
            errorBox.addButton(tr("Copy to clipboard"), QMessageBox::YesRole);
            errorBox.buttons().at(0)->setIcon(currentIndexTheme? COPY_ICON_LIGHT: COPY_ICON_DARK);
            errorBox.addButton(tr("No"), QMessageBox::NoRole);
            errorBox.buttons().at(1)->setIcon(currentIndexTheme? OFF_ICON_LIGHT: OFF_ICON_DARK);
            bool userDecisionIsYes{ !errorBox.exec() };

            if (userDecisionIsYes)
                QApplication::clipboard()->
                    setText(QString("enum QProcess::ProcessError: %1").arg(error));
        } else close();
    }
}

void Window::saveSettings()
{
    userSettings->setValue("Settings/IndexLanguage", currentIndexLanguage);
    userSettings->setValue("Settings/IndexTheme", currentIndexTheme);
    emit saveSettingsSignal();
}

void Window::changeTheme()
{
    const qsizetype senderThemeIndex{ themeActionList.indexOf(static_cast<QAction *>(sender())) };
    if (senderThemeIndex != currentIndexTheme) {
        QFile styleSheetFile{ ":/qss/" + themeActionList[senderThemeIndex]->objectName() };
        styleSheetFile.open(QFile::ReadOnly);
        qApp->setStyleSheet(styleSheetFile.readAll());
        themeActionList[currentIndexTheme]->setIcon(QIcon());
        themeActionList[senderThemeIndex]->setIcon(currentIndexTheme? CHECK_ICON_LIGHT: CHECK_ICON_DARK);
        currentIndexTheme = senderThemeIndex;

        emit themeChanged(true);
    }
}

void Window::about()
{
    const static QString description
        { tr("<div style=\"text-align: center\">"
               "<h1>%1</h1>"
               "<p>Version: %2</p>"
               "<p>%1 is a free password generator that helps you"
               " create random and customizable passwords. "
               "The program adds or removes characters in the "
               "following order:</p>"
               "<ol>"
               "<li>Numbers, Symbols, Lowercase letters, Uppercase letters</li>"
               "<li>Adds the letters that the user has specified</li>"
               "<li>Removes the letters that the user has specified</li>"
               "</ol>"
               "<p>For issues or contributions, visit the "
               "<a href=https://github.com/ibra-kdbra/Cpp_Projects/tree/main/GenXPassword style=color:#0097CF>"
               "GitHub repository</a></p>"
               "<p>Copyright © 2024 - ibra-kdbra</p>"
               "<p>This program comes with NO WARRANTY OF ANY KIND.<br>"
               "See the <a href=https://www.gnu.org/licenses/gpl-3.0.html style=color:#0097CF>"
               "GNU General Public License</a> for details.</p>"
               "</div>").arg(appName, appVersion)
        };
    QMessageBox::about(this, tr("About %1").arg(appName), description);
}

void Window::credits()
{
    const static QString description
        { tr("<div style=\"text-align: center\">"
               "<h1>%1</h1>"
               "<p>Created by <a href=https://github.com/ibra-kdbra style=color:#0097CF>ibra-kdbra</a></p>"
               "<p>Contributors:</p>"
               "<ul>"
               "<li><a href=https://fonts.google.com/icons style=color:#0097CF>Material Icons</a></li>"
               "</ul>"
               "</div>").arg(appName)
        };
    QMessageBox::about(this, tr("Credits"), description);
}
