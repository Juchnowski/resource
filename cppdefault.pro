QT += core
QT -= gui

#CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++17

TARGET = cppdefault
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += test/main.cpp \
    test/test_fd.cpp \
    test/test_new.cpp \
    test/test_malloc.cpp \
    test/test_mutex_locker.cpp

HEADERS += \
    resource/resource.hpp \
    resource/storage/default_storage.hpp \
    resource/storage/automatic_storage.hpp \
    resource/traits.hpp \
    resource/cleanup/default_deleter.hpp \
    resource/cleanup/function_deleter.hpp \
    resource/copy/not_customized.hpp \
    resource/copy/disable_copy_enable_move.hpp \
    resource/copy/disable_copy_and_move.hpp \
    resource/copy/default_copy.hpp \
    resource/cleanup/delete_deleter.hpp \
    test/includes.hpp
