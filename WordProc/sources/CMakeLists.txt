# sources
SET                   (CXX_SOURCES Main.cxx MainWindow.cxx SearchWindow.cxx TextWindow.cxx
                                   Types.cxx)
SET                   (INC_FILES MainWindow.hxx SearchWindow.hxx TextWindow.hxx Types.hxx)
SET                   (UI_FILES MainWindow.ui TextWindow.ui SearchWindow.ui)
SET                   (RES_FILES Resources.qrc)
SET                   (TS_FILES i18n_Dutch.ts i18n_English.ts i18n_French.ts i18n_Italian.ts
                                i18n_German.ts i18n_Spanish.ts)

# wrapper and Qt automoc
QT5_ADD_TRANSLATION   (TS_SOURCES ${TS_FILES})
QT5_WRAP_CPP          (MOC_SOURCES ${INC_FILES})
QT5_WRAP_UI           (UI_SOURCES ${UI_FILES})
QT5_ADD_RESOURCES     (RES_SOURCES ${RES_FILES})

# binaries
ADD_EXECUTABLE        (${CMAKE_PROJECT_NAME} ${CXX_SOURCES} ${MOC_SOURCES} ${UI_SOURCES}
                       ${RES_SOURCES} ${TS_SOURCES})
TARGET_LINK_LIBRARIES (${CMAKE_PROJECT_NAME} Qt5::Core Qt5::PrintSupport Qt5::Widgets)
