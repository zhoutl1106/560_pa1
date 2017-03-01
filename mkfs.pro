TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    mkfs_dir.cpp \
    mkfs_file.cpp \
    mkfs.cpp \
    utils.cpp \
    mkfs_net.cpp

HEADERS += \
    mkfs_dir.h \
    mkfs_file.h \
    mkfs.h \
    utils.h \
    mkfs_net.h
