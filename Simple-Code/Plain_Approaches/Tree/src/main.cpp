#include <QApplication>
#include "GuiApp.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    GuiApp guiApp;
    guiApp.setWindowTitle("Binary Tree Algorithms");
    guiApp.resize(800, 600);
    guiApp.show();

    return app.exec();
}
