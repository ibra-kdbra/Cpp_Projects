#include <QApplication>
#include "MainWindow.hxx"

qint32 main(qint32 argc, char **argv)
{
    QApplication app(argc, argv);
    MainWindow main_win;

    QApplication::setApplicationName("WordProc");
    QApplication::setOrganizationName("LinuxAddicted");
    QApplication::setApplicationVersion(QString("0.0.0.0 (") + __DATE__ + " " + __TIME__ + ")");

    main_win.show();

    return app.exec();
}
