#-------------------------------------------------
#
# Project created by Creator 2013-09-06T02:05:41
#
#-------------------------------------------------

QT    += core gui network xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = skywardswordplugin
TEMPLATE = lib
CONFIG += plugin
TRANSLATIONS += \
    lang/en_ES.ts

DESTDIR = $$OUT_PWD/../../build/plugins
UI_DIR = ui

DEFINES += SKYWARDSWORDPLUGGIN_LIBRARY

INCLUDEPATH += \
    include \
    ../PluginFramework/include \
    ../Athena/include \
    ../Updater/include

QMAKE_CXXFLAGS = -std=c++11

LIBS += \
    -L$$OUT_PWD/../PluginFramework -lpluginframework \
    -L$$OUT_PWD/../Updater -lupdater

CONFIG(release, release|debug){
    LIBS += -L$$OUT_PWD/Athena/lib -lAthena
    DEFINES -= SS_DEBUG
    # We don't want the objects, or MOC sources
    # in the project directory, so tell qmake
    # where to put them
    OBJECTS_DIR = obj/release
    MOC_DIR = moc/release
}


CONFIG(debug, debug|release){
    LIBS += -L$$OUT_PWD/Athena/lib -lAthena-d
    DEFINES += SS_DEBUG
    # We don't want the objects, or MOC sources
    # in the project directory, so tell qmake
    # where to put them
    OBJECTS_DIR = obj/debug
    MOC_DIR = moc/debug
}

SOURCES += \
    src/SkywardSwordPlugin.cpp \
    src/SkywardSwordEditorForm.cpp \
    src/SkywardSwordGameFile.cpp \
    src/SettingsManager.cpp \
    src/SettingsDialog.cpp \
    src/PlaytimeWidget.cpp \
    src/Common.cpp \
    src/TriforceWidget.cpp \
    src/CopyWidget.cpp \
    src/SkywardSwordTabWidget.cpp \
    src/UpdateDialog.cpp \
    src/SkipDatabaseWidget.cpp \
    src/SkipDatabaseElementEditor.cpp \
    src/ImportExportQuestDialog.cpp

HEADERS +=\
    include/SkywardSwordPlugin.hpp \
    include/skywardswordplugin_global.hpp \
    include/SkywardSwordEditorForm.hpp \
    include/SkywardSwordGameFile.hpp \
    include/SettingsManager.hpp \
    include/SettingsDialog.hpp \
    include/PlaytimeWidget.hpp \
    include/Common.hpp \
    include/Constants.hpp \
    include/TriforceWidget.hpp \
    include/CopyWidget.hpp \
    include/SkywardSwordTabWidget.hpp \
    include/UpdateDialog.hpp \
    include/SkipDatabaseWidget.hpp \
    include/SkipDatabaseElementEditor.hpp \
    include/ImportExportQuestDialog.hpp

RESOURCES += \
    resources/resources.qrc
OTHER_FILES += \
    resources/resource.rc \
    skywardswordplugin.json

FORMS += \
    ui/SkywardSwordEditorForm.ui \
    ui/SettingsDialog.ui \
    ui/PlaytimeWidget.ui \
    ui/CopyWidget.ui \
    ui/UpdateDialog.ui \
    ui/SkipDatabaseWidget.ui \
    ui/SkipDatabaseElementEditor.ui \
    ui/ImportExportQuestDialog.ui

win32:RC_FILE += resources/resource.rc
