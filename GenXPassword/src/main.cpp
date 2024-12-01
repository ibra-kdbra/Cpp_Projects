#include "passwordgenerator.h"
#include "icons.h"

#include <QApplication>
#include <QStyleFactory>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QTranslator>
#include <QMessageBox>
#include <QSharedMemory>

int main(int argc, char *argv[])
{
    QApplication app{ argc, argv };
    app.setStyle(QStyleFactory::create("Fusion"));
    app.setApplicationName("GenXPassword");
    app.setWindowIcon(APP_ICON_LIGHT);
    app.setApplicationVersion("2.0.0");
    app.setOrganizationName("ibra-kdbra");
    app.setOrganizationDomain("https://github.com/ibra-kdbra/Cpp_Projects/tree/main/GenXPassword");

    if (!QFile::exists("config")) {
        const QDir config{ app.applicationDirPath() };
        config.mkdir("config");
    }

    QSettings settings{ app.applicationDirPath() + "/config/Settings.ini",
                       QSettings::Format::IniFormat };
    if (!QFile::exists(settings.fileName())) {
        settings.setValue("Settings/IndexLanguage", 0);
        settings.setValue("Settings/IndexTheme", 0);
        settings.setValue("Settings/HidePassword", true);
        settings.setValue("Settings/Length", 32);
        settings.setValue("Settings/Numeric", false);
        settings.setValue("Settings/Symbols", false);
        settings.setValue("Settings/Lowercase", false);
        settings.setValue("Settings/Uppercase", false);
        settings.setValue("Settings/Add", "");
        settings.setValue("Settings/Remove", "");
    }

    QTranslator translator;
    const QStringList languages{ "en.qm", "es.qm", "fr.qm", "ja.qm" };
    int indexLanguage{ settings.value("Settings/IndexLanguage").toInt() };
    if (indexLanguage >= 0 && indexLanguage < languages.size()) {
        if (!indexLanguage);
        else if (translator.load(":/i18n/" + languages[indexLanguage]))
            app.installTranslator(&translator);
        else QMessageBox::warning(nullptr, "Warning",
                                 "Could not load translation file.",
                                 QMessageBox::StandardButton::Close);
    } else return QMessageBox::critical(nullptr, "Error",
                                     "The language index specified in the "
                                     "configuration file is invalid. \n"
                                     "Please check the value and try again.",
                                     QMessageBox::StandardButton::Abort);

    QSharedMemory sharedMemory(app.applicationName());
    if (!sharedMemory.create(1)) {
        return QMessageBox::warning(nullptr,
                                    QObject::tr("GenPassword is now running"),
                                    QObject::tr("The application is already "
                                                "running.\nAnother instance "
                                                "cannot be started."),
                                    QMessageBox::StandardButton::Abort);
    }

    const QStringList themes{ "light.css", "dark.css" };
    int indexTheme{ settings.value("Settings/IndexTheme").toInt() };
    if (indexTheme >= 0 && indexTheme < themes.size()) {
        QFile styleSheetFile{ ":/qss/" + themes[indexTheme] };
        styleSheetFile.open(QFile::ReadOnly);
        app.setStyleSheet(styleSheetFile.readAll());
    } else return QMessageBox::critical(nullptr, QObject::tr("Error"),
                                     QObject::tr("The theme index specified "
                                                 "in the configuration file "
                                                 "is invalid. \nPlease check "
                                                 "the value and try again."),
                                     QMessageBox::StandardButton::Abort);

    PasswordGenerator mainWindow;
    mainWindow.setFixedHeight(mainWindow.sizeHint().height());
    mainWindow.show();

    return app.exec();
}
