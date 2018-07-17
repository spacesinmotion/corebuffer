TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
    parser.cpp \
    package.cpp \
    cppoutput.cpp

HEADERS += \
    parser.h \
    package.h \
    cppoutput.h \
    testout.h
