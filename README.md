# DWD Weather Data Converter

![grafik](https://github.com/hirseboy/DWD-Weather-Data-Converter/assets/58851829/453ab876-abd7-498d-934b-c952ebbf1b61)

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

# Screenshots

![grafik](https://github.com/hirseboy/DWD-Weather-Data-Converter/assets/58851829/a462e4e1-3f89-4888-9c2c-62582f480de5)
![grafik](https://github.com/hirseboy/DWD-Weather-Data-Converter/assets/58851829/58b4d317-771d-4397-bb9f-e155dd2dbd26)
