# -------------------------------------------------
# Project for DataMap library
# -------------------------------------------------

# first we define what we are
TARGET = DataMap
TEMPLATE = lib

# this pri must be sourced from all our libraries,
# it contains all functions defined for casual libraries
include( ../../../IBK/projects/Qt/IBK.pri )

QT += gui core widgets svg

# finally we setup our custom library specfic things
# like version number etc., we also may reset all
unix|mac {
	VER_MAJ = 0
	VER_MIN = 9
	VER_PAT = 0
	VERSION = $${VER_MAJ}.$${VER_MIN}.$${VER_PAT}
}

LIBS += -lIBK -lTiCPP -lQtExt

INCLUDEPATH +=	\
    ../../src/ \
	../../../IBK/src \
	../../../QtExt/src

SOURCES += \
    ../../src/DM_DataItem.cpp \
    ../../src/DM_DataItemGroup.cpp \
	../../src/DM_GraphicsView.cpp \
	../../src/DM_MapDialog.cpp \
	../../src/DM_Scene.cpp

HEADERS += \
	../../src/DM_Conversions.h \
	../../src/DM_Data.h \
	../../src/DM_DataItem.h \
	../../src/DM_DataItemGroup.h \
	../../src/DM_GraphicsView.h \
	../../src/DM_MapDialog.h \
	../../src/DM_Scene.h

FORMS += \
	../../src/DM_MapDialog.ui

RESOURCES += \
	../../resources/DataMap.qrc
