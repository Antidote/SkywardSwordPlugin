TEMPLATE = subdirs
CONFIG += ordered

Plugin.depends += libzelda \
        PluginFramework \
        Updater
		
SUBDIRS += \
    libzelda \
    Updater \
    PluginFramework \
    Plugin

OTHER_FILES += .travis.yml
