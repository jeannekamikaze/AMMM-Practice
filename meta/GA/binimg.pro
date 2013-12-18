#-------------------------------------------------
#
# Project created by QtCreator 2013-11-24T20:17:45
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = binimg
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -O3 --std=c++0x -D_DEBUG -D_DEBUG_OUTPUT -fopenmp

SOURCES += \
    Pool.cc \
    main.cc \
    Chromosome.cc \
    crossover.cc \
    mutation.cc

HEADERS += \
    Pool.h \
    Chromosome.h \
    ../ParseDatFile.hpp \
    ../ObjFunc.h \
    crossover.h \
    mutation.h \
    ../SolGen.hpp

LIBS += -lopencv_core -lopencv_imgproc -lopencv_highgui -lSDL2 -lgomp
