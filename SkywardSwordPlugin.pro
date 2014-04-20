TEMPLATE = subdirs
CONFIG += ordered

Plugin.depends += libzelda \
        PluginFramework \
        Updater
		
SUBDIRS += \
    Athena \
    Updater \
    PluginFramework \
    Plugin

OTHER_FILES += .travis.yml
