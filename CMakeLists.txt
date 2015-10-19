cmake_minimum_required(VERSION 3.0)
project(SkywardSwordPlugin)

if(NOT MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

find_package(Qt5 REQUIRED COMPONENTS Core Gui Xml Widgets)

add_definitions(-fPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS)

add_subdirectory(PluginFramework)
add_subdirectory(Updater)
add_subdirectory(Athena EXCLUDE_FROM_ALL)

set(CMAKE_INCLUDE_CURRENT_DIR True)
set(CMAKE_AUTOMOC True)

include_directories(Plugin/include Athena/include PluginFramework/include Updater/include)

qt5_wrap_ui(ui_out
    Plugin/ui/CopyWidget.ui
    Plugin/ui/PlaytimeWidget.ui
    Plugin/ui/SkipDatabaseWidget.ui
    Plugin/ui/SkywardSwordWidget.ui
    Plugin/ui/SkywardSwordEditorForm.ui
    Plugin/ui/SkipDatabaseElementEditor.ui
    Plugin/ui/ImportExportQuestDialog.ui
    Plugin/ui/SaveInfoDialog.ui
    Plugin/ui/SettingsDialog.ui
    Plugin/ui/UpdateDialog.ui)

qt5_add_resources(rc_out Plugin/resources/skyward_sword_resources.qrc)

add_library(skywardswordplugin SHARED
    Plugin/src/SkywardSwordPlugin.cpp Plugin/include/SkywardSwordPlugin.hpp
    Plugin/src/Common.cpp Plugin/include/Common.hpp
    Plugin/src/CopyWidget.cpp Plugin/include/CopyWidget.hpp
    Plugin/src/ImportExportQuestDialog.cpp Plugin/include/ImportExportQuestDialog.hpp
    Plugin/src/PlaytimeWidget.cpp Plugin/include/PlaytimeWidget.hpp
    Plugin/src/SaveInfoDialog.cpp Plugin/include/SaveInfoDialog.hpp
    Plugin/src/SettingsDialog.cpp Plugin/include/SettingsDialog.hpp
    Plugin/src/SettingsManager.cpp Plugin/include/SettingsManager.hpp
    Plugin/src/SkipDatabaseElementEditor.cpp Plugin/include/SkipDatabaseElementEditor.hpp
    Plugin/src/SkipDatabaseWidget.cpp Plugin/include/SkipDatabaseWidget.hpp
    Plugin/src/SkywardSwordGameDocument.cpp Plugin/include/SkywardSwordGameDocument.hpp
    Plugin/src/SkywardSwordQuestEditorForm.cpp Plugin/include/SkywardSwordQuestEditorForm.hpp
    Plugin/src/SkywardSwordTabWidget.cpp Plugin/include/SkywardSwordTabWidget.hpp
    Plugin/src/SkywardSwordWidget.cpp Plugin/include/SkywardSwordWidget.hpp
    Plugin/src/TriforceWidget.cpp Plugin/include/TriforceWidget.hpp
    Plugin/src/UpdateDialog.cpp Plugin/include/UpdateDialog.hpp
    Plugin/src/WiiSaveLoaderThread.cpp Plugin/include/WiiSaveLoaderThread.hpp
    Plugin/include/Constants.hpp
    Plugin/include/skywardswordplugin_global.hpp
    ${ui_out}
    ${rc_out}
)

target_link_libraries(skywardswordplugin
    PluginFramework
    Updater
    AthenaWiiSave
    AthenaZelda
    AthenaCore
    z

    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5Xml_LIBRARIES}
)
