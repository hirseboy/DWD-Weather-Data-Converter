# CMakeLists.txt file for DWDWeatherDataConverter

project( DWDWeatherDataConverter )

if (COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
  cmake_policy(SET CMP0020 NEW)
endif (COMMAND cmake_policy)

# add include directories
include_directories(
	${PROJECT_SOURCE_DIR}/../../src	
	${PROJECT_SOURCE_DIR}/../../../externals/IBK/src
	${PROJECT_SOURCE_DIR}/../../../externals/QtExt/src
	${PROJECT_SOURCE_DIR}/../../../externals/CCM/src
	${PROJECT_SOURCE_DIR}/../../../externals/TiCPP/src
	${PROJECT_SOURCE_DIR}/../../../externals/QuaZIP/src
	${PROJECT_SOURCE_DIR}/../../../externals/qwt/src
	${PROJECT_SOURCE_DIR}/../../../externals/DataMap/src
	${PROJECT_SOURCE_DIR}/../../../externals/qftp/src
	${Qt5Widgets_INCLUDE_DIRS}
	${Qt5Network_INCLUDE_DIRS}
	${Qt5Svg_INCLUDE_DIRS}
	${Qt5PrintSupport_INCLUDE_DIRS}
)

# collect a list of all source files
file( GLOB APP_SRCS ${PROJECT_SOURCE_DIR}/../../src/*.cpp )

# collect a list of all header files (to be used in MOC compiler)
file( GLOB APP_HDRS ${PROJECT_SOURCE_DIR}/../../src/*.h )

# collect a list of all form files
file( GLOB APP_UIS ${PROJECT_SOURCE_DIR}/../../src/*.ui )

set( APP_SRCS 
	${APP_SRCS} 
)
set( APP_HDRS 
	${APP_HDRS} 
)

# The Qt resource file (*.qrc)
set( APP_QRCS	
	${PROJECT_SOURCE_DIR}/../../resources/DWDWeatherDataConverter.qrc
	${PROJECT_SOURCE_DIR}/../../resources/style/style.qrc
)

# run uic and moc compilers, add resources
qt5_add_resources( APP_RC_SRCS ${APP_QRCS} )
qt5_wrap_ui( APP_UI_SRCS ${APP_UIS} )
qt5_wrap_cpp( APP_MOC_SRCS ${APP_HDRS} )

# generate list of all required QT_LIBRARIES
set ( QT_LIBRARIES 
	Qt5::Widgets
	Qt5::PrintSupport
	Qt5::Svg
	Qt5::Xml
	Qt5::Network
	Qt5::Gui
	Qt5::Core
)

# build application executable for the different platforms
if( WIN32 )
	add_executable( ${PROJECT_NAME} WIN32 
		${APP_SRCS} ${APP_MOC_SRCS} ${APP_RC_SRCS} ${APP_UI_SRCS}
	)

	# Add pre target dependencies
	add_dependencies(${PROJECT_NAME} IBK.lib CCM.lib QtExt.lib QuaZIP.lib qwt6.lib)
	
endif( WIN32 ) 

if( UNIX )
	add_executable( ${PROJECT_NAME} 
		${APP_SRCS} ${APP_MOC_SRCS} ${APP_RC_SRCS} ${APP_UI_SRCS}
	)
endif( UNIX )


# link against the dependent libraries
target_link_libraries( ${PROJECT_NAME} 
	QtExt
	QuaZIP
	CCM
	TiCPP
	IBK
	qwt6
	DataMap
	qftp
	${QT_LIBRARIES}
)

