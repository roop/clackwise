######################################################################
# .pro file for qmake
######################################################################

TEMPLATE = lib
TARGET = 
OBJECTS_DIR = build
INCLUDEPATH += . /usr/include/tcl8.4

CONFIG += debug

# Swig
swig_tcl.name = Creating Tcl interface using Swig
swig_tcl.input = SWIG_INTERFACES
swig_tcl.depends = Commands.h
swig_tcl.output = ${QMAKE_FILE_BASE}_tcl_wrap.cpp
swig_tcl.commands = swig -c++ -tcl8 -o ${QMAKE_FILE_BASE}_tcl_wrap.cpp ${QMAKE_FILE_IN} 
swig_tcl.variable_out = SOURCES
QMAKE_EXTRA_COMPILERS += swig_tcl

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
SWIG_INTERFACES += Commands.i

LEXSOURCES += liberty.l
YACCSOURCES += liberty.y

