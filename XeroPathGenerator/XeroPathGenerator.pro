# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Tools.
# ------------------------------------------------------

BUILDTYPE=release
OSTYPE=linux
TEMPLATE = app
TARGET = XeroPathGenerator
DESTDIR = ../x64/$${BUILDTYPE}
QT += core network gui widgets charts printsupport
CONFIG += $${BUILDTYPE}
CONFIG += c++17
CONFIG += app_bundle
ICON=icon.icns
DEFINES += _UNICODE _ENABLE_EXTENDED_ALIGNED_STORAGE QT_DLL QT_CHARTS_LIB QT_NETWORK_LIB QT_WIDGETS_LIB
INCLUDEPATH += $(SolutionDir)external/wpi/windows/include \
    $(SolutionDir)PathGenCommon \
    ./GeneratedFiles \
    . \
    ./GeneratedFiles/$(ConfigurationName)\
    ../external/wpi/include\
    ../PathGenCommon
LIBS += -L"$(SolutionDir)$(Platform)/$(Configuration)" \
    ../PathGenCommon/$${BUILDTYPE}/PathGenCommon.a\
    ../external/wpi/$${OSTYPE}/$${BUILDTYPE}/lib/ntsim.a\
    ../external/wpi/$${OSTYPE}/$${BUILDTYPE}/lib/wpiutilsim.a

images.files=$$PWD/images
images.path=Contents/MacOS
QMAKE_BUNDLE_DATA += images

fields.files=$$PWD/../fields
fields.path=Contents/MacOS
QMAKE_BUNDLE_DATA += fields

generators.files=$$PWD/../generators
generators.path=Contents/MacOS
QMAKE_BUNDLE_DATA += generators

DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/$(ConfigurationName)
OBJECTS_DIR += $${BUILDTYPE}
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
QMAKE_CXXFLAGS += -std=c++17
QMAKE_MACOSX_DEPLOYMENT_TARGET=10.12
include(XeroPathGenerator.pri)

win32:RC_FILE = XeroPathGenerator.rc
