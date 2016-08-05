QT += core
QT -= gui

#CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++17 -fconcepts

TARGET = cppdefault
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

HEADERS += \
    resource/resource.hpp \
    resource/storage/default_storage.hpp \
    resource/storage/automatic_storage.hpp \
    resource/traits.hpp \
    resource/cleanup/default_deleter.hpp \
    resource/cleanup/function_deleter.hpp
