######################################################################
# .pro file for qmake
######################################################################

TEMPLATE = lib
TARGET = 
OBJECTS_DIR = build

CONFIG += debug

# Input
HEADERS += LibGroup.h \
           DotLib.h \
		   LibTrove.h \
		   Commands.h \
		   Troves.h
SOURCES += LibGroup.cpp \
           DotLib.cpp \
		   LibTrove.cpp \
		   Troves.cpp \
		   Commands.cpp \
           main.cpp

LEXSOURCES += liberty.l
YACCSOURCES += liberty.y

