TEMPLATE = app
CONFIG += console -std=c++11
CONFIG -= app_bundle

QMAKE_CXXFLAGS += -std=c++11 -O2 -I/usr/lib64/llvm/include/c++/v1/

SOURCES += main.cpp example.cpp

HEADERS += example.h task.h

