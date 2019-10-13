TEMPLATE = app

TARGET = PathGenerate

QT += core

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

DEFINES += QT_DLL

CONFIG += release debug console
CONFIG += sdk_no_version_check

LIBS += -L"$(SolutionDir)$(Platform)/$(Configuration)" \
    -L"$(NOINHERIT)" \
    -lXeroPathCommon \
    -lPathGenCommon \
    -l$(Qt_LIBS_)

DEPENDPATH += .
INCLUDEPATH += ../PathGenCommon
INCLUDEPATH += ../XeroPathCommon

include(PathGenerate.pri)
