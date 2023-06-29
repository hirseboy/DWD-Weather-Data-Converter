cmake_minimum_required(VERSION 3.10)
project(DWDWeatherDataConverter)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Svg REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Concurrent REQUIRED) 
find_package(Qt5Core REQUIRED) 

# automatically add CMAKE_CURRENT_SOURCE_DIR and CMAKE_CURRENT_BINARY_DIR to the include directories in every processed CMakeLists.txt
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# -------------------------------------------------------------
# Support for build flags
# -------------------------------------------------------------
include( ../../externals/IBK/projects/cmake_local/cmake_pri/CMakeLists.txt  )

# Include subdirectories
add_subdirectory({CMAKE_CURRENT_LIST_DIR}/../../externals/IBK/projects/cmake_local IBK)
add_subdirectory({CMAKE_CURRENT_LIST_DIR}/../../externals/QtExt/projects/cmake_local QtExt)
add_subdirectory({CMAKE_CURRENT_LIST_DIR}/../../externals/QuaZIP/projects/cmake_local QuaZIP)
add_subdirectory({CMAKE_CURRENT_LIST_DIR}/../../externals/CCM/projects/cmake_local CCM)
add_subdirectory({CMAKE_CURRENT_LIST_DIR}/../../externals/TiCPP/projects/cmake_local TiCPP)
add_subdirectory({CMAKE_CURRENT_LIST_DIR}/../../externals/qftp/projects/cmake_local qftp)
add_subdirectory({CMAKE_CURRENT_LIST_DIR}/../../externals/qwt/projects/cmake_local qwt6)
add_subdirectory({CMAKE_CURRENT_LIST_DIR}/../../externals/DataMap/projects/cmake_local DataMap)
add_subdirectory({CMAKE_CURRENT_LIST_DIR}/../../DWDWeatherDataConverter/projects/cmake_local DWDWeatherDataConverter)

# Set dependencies
add_dependencies(QtExt IBK)
add_dependencies(QuaZIP IBK)
add_dependencies(CCM IBK TiCPP)
add_dependencies(TiCPP IBK)
add_dependencies(DataMap IBK QtExt)
add_dependencies(DWDWeatherDataConverter IBK QtExt qftp qwt6 DataMap)
