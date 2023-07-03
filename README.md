# DWD Weather Data Converter

![grafik](https://github.com/hirseboy/DWD-Weather-Data-Converter/assets/58851829/0cb63b55-b714-4603-a791-41926875deae)

This repository holds all data concerning the weather data converter for converting weather data from the German Weather Service (DWD) [https://www.dwd.de/DE/Home/home_node.html].

It is accessing the data stored in the ftp server and the user can select needed data in order to create a weather data file (epw, c6b) that can be used for simulations (SIM-VICUS, DELPHIN, etc.)

# Build

## Qt-Creator

Open the pro-file in Qt-Creator under '/build/qt'. Compile with MSVC 2019 or gcc

## cmake

Just go to '/build/cmake' and call under windows: 'build_x64_with_pause.bat' or run under Linux 'build.sh'

# Status

Windows: [![CMake](https://github.com/hirseboy/DWD-Weather-Data-Converter/actions/workflows/cmake_windows.yml/badge.svg)](https://github.com/hirseboy/DWD-Weather-Data-Converter/actions/workflows/cmake_windows.yml)
Linux:  [![CMake](https://github.com/hirseboy/DWD-Weather-Data-Converter/actions/workflows/cmake.yml/badge.svg)](https://github.com/hirseboy/DWD-Weather-Data-Converter/actions/workflows/cmake.yml)
