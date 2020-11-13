TEMPLATE = lib
CONFIG += staticlib
CONFIG += Release

Release:DESTDIR=release
Release:OBJECTS_DIR=release/obj
Release:MOC_DIR=release/moc
Release:RCC_DIR=release/rcc
Release:UI_DIR=release/ui

Debug:DESTDIR=debug
Debug:OBJECTS_DIR=debug/obj
Debug:MOC_DIR=debug/moc
Debug:RCC_DIR=debug/rcc
Debug:UI_DIR=debug/ui

SOURCES += CSVParser.cpp
SOURCES += GeneratorManager.cpp
SOURCES += GeneratorParameterStore.cpp
SOURCES += ManagerBase.cpp
SOURCES += PathCollectionIO.cpp
SOURCES += PathGenerationEngine.cpp
SOURCES += RobotManager.cpp
SOURCES += CSVFlagsWriter.cpp
SOURCES += JSONFlagsWriter.cpp

INCLUDEPATH += ../PathGenCommon
