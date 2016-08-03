QT += core
QT -= gui

#CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++17

TARGET = cppdefault
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

HEADERS += \
    resource/resource.hpp
