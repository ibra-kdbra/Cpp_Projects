QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
VERSION = 2.0.0
QMAKE_TARGET_PRODUCT = GenXPassword
QMAKE_TARGET_DESCRIPTION = "A generator password"
QMAKE_TARGET_COPYRIGHT = ibra-kdbra

win32: RC_ICONS = icons/appIcon.ico

SOURCES += \
    src/framelesswindow.cpp \
    src/main.cpp \
    src/passwordgenerator.cpp \
    src/switchbutton.cpp \
    src/window.cpp

HEADERS += \
    include/framelesswindow.h \
    include/icons.h \
    include/passwordgenerator.h \
    include/switchbutton.h \
    include/window.h

INCLUDEPATH += include
RESOURCES += \
    resources/i18n.qrc \
    resources/icons.qrc \
    resources/qss.qrc

TRANSLATIONS += \
    i18n/es.ts \
    i18n/ja.ts \
    i18n/fr.ts \
    i18n/ar.ts


# Set the output directory for the final executable
DESTDIR = build/bin

# Set directories for intermediate files (object files and MOC files)
OBJECTS_DIR = build/objects
MOC_DIR = build/moc
RCC_DIR = build/rcc

# Conditional output directory for different platforms
win32: DESTDIR = win_build/bin
unix:!android: DESTDIR = build/bin

# Set the installation path
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
