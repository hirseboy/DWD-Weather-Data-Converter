# remember to set DYLD_FALLBACK_LIBRARY_PATH on MacOSX
# set LD_LIBRARY_PATH on Linux

TARGET = DWDWeatherDataConverter
TEMPLATE = app

# this pri must be sourced from all our applications
include( ../../../externals/IBK/projects/Qt/IBK.pri )

QT += xml core gui network printsupport widgets svg
CONFIG += console

LIBS += -L../../../lib$${DIR_PREFIX} \
				#-lIBKMK \
				-lQtExt \
				-lQuaZIP \
				-lCCM \
				-lTiCPP \
				-lIBK \
				-lqwt6 \
				-lDataMap \
				-lqftp

INCLUDEPATH = \
				../../src \
				../../../externals/IBK/src \
				#../../../externals/IBKMK/src \
				../../../externals/QtExt/src \
				../../../externals/CCM/src \
				../../../externals/TiCPP/src \
				../../../externals/QuaZIP/src \
				../../../externals/qwt/src \
				../../../externals/DataMap/src \
				../../../externals/qftp/src

win32 {
PRE_TARGETDEPS += \
				$$PWD/../../../externals/lib$${DIR_PREFIX}/IBK.lib \
				$$PWD/../../../externals/lib$${DIR_PREFIX}/CCM.lib \
				$$PWD/../../../externals/lib$${DIR_PREFIX}/QtExt.lib \
				$$PWD/../../../externals/lib$${DIR_PREFIX}/qwt6.lib \
				$$PWD/../../../externals/lib$${DIR_PREFIX}/QuaZIP.lib \
}

#TRANSLATIONS += ../../resources/translations/GenBuiGui.ts
CODECFORSRC = UTF-8

RESOURCES += \
                ../../resources/DWDWeatherDataConverter.qrc \
				../../resources/style/style.qrc


SOURCES += \
                # ../../src/DWDChart.cpp \
				#../../src/DWDChartView.cpp \
				../../src/DWDConstants.cpp \
				../../src/DWDData.cpp \
				../../src/DWDDelegate.cpp \
				../../src/DWDDescriptonData.cpp \
				#../../src/DWD_CheckBox.cpp \
				../../src/DWDDownloader.cpp \
				../../src/DWDLogWidget.cpp \
				../../src/DWDMainWindow.cpp \
				../../src/DWDMessageHandler.cpp \
				../../src/DWDSortFilterProxyModel.cpp \
				../../src/DWDUtilities.cpp \
				../../src/MetaDataEditWidget.cpp \
				../../src/main.cpp \
				../../src/DWDTableModel.cpp \
				../../src/DWDTimePlotPicker.cpp \
				../../src/DWDDateTimeScaleEngine.cpp

FORMS += \
				../../src/DWDLogWidget.ui \
				../../src/DWDMainWindow.ui \
				../../src/MetaDataEditWidget.ui

HEADERS += \
                #../../src/DWDChart.h \
				#../../src/DWDChartView.h \
				../../src/DWDConstants.h \
				../../src/DWDConversions.h \
				../../src/DWDData.h \
				../../src/DWDDelegate.h \
				../../src/DWDDescriptonData.h \
				#../../src/DWD_CheckBox.h \
				../../src/DWDDownloader.h \
				../../src/DWDLogWidget.h \
				../../src/DWDMainWindow.h \
				../../src/DWDMessageHandler.h \
				../../src/DWDPlotZoomer.h \
				../../src/DWDSortFilterProxyModel.h \
				../../src/DWDTableModel.h \
				../../src/DWDTimePlotPicker.h \
				../../src/DWDDateTimeScaleEngine.h \
				../../src/DWDUtilities.h \
				../../src/MetaDataEditWidget.h \
				../../ôsrc/DWDConversions.h

DISTFILES +=


