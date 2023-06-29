cmake_minimum_required(VERSION 3.9)
project(qftp)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Network REQUIRED)

file(GLOB LIB_SRCS ${PROJECT_SOURCE_DIR}/../../src/*.cpp)
file(GLOB LIB_HDRS ${PROJECT_SOURCE_DIR}/../../src/*.h)
file(GLOB LIB_UIS ${PROJECT_SOURCE_DIR}/../../src/*.ui)

include_directories(
	${PROJECT_SOURCE_DIR}/../../src
	${Qt5Core_INCLUDE_DIRS}
	${Qt5Network_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME} STATIC
	${LIB_SRCS}
	${LIB_HDRS}
)

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Network)
