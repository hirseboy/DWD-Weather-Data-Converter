# DWD Weather Data Converter

![grafik](https://github.com/hirseboy/DWD-Weather-Data-Converter/assets/58851829/453ab876-abd7-498d-934b-c952ebbf1b61)

This repository holds all data concerning the weather data converter for converting weather data from the [German Weather Service (DWD)](https://www.dwd.de/DE/Home/home_node.html).

It is accessing meta data stored on the ftp server and all weather data can be combined interactivly into weather-files (epw, c6b). So all data can be used for simulation tools (SIM-VICUS, DELPHIN, etc.)

# Status

| Test | Result|
|-----|-----|
| CI - Linux 64-bit (Ubuntu 20.04.3 LTS; Qt 5.12.9) |  [![CMake](https://github.com/hirseboy/DWD-Weather-Data-Converter/actions/workflows/cmake.yml/badge.svg)](https://github.com/hirseboy/DWD-Weather-Data-Converter/actions/workflows/cmake.yml)   |
| CI - Windows 64-bit (Win10, VC 2019, Qt 5.15.2) | [![CMake](https://github.com/hirseboy/DWD-Weather-Data-Converter/actions/workflows/cmake_windows.yml/badge.svg)](https://github.com/hirseboy/DWD-Weather-Data-Converter/actions/workflows/cmake_windows.yml) |
| CI - MacOS 64-bit (10.11 "El Capitan", Qt 5.11.3) | _currently no action_ |

# Build

## Qt-Creator

Open the pro-file in Qt-Creator under `/build/qt`. Compile with MSVC 2019 or gcc

## cmake

Just go to `/build/cmake` and call under windows: `build_x64_with_pause.bat` or run under Linux `build.sh`

# Screenshots

![grafik](https://github.com/hirseboy/DWD-Weather-Data-Converter/assets/58851829/9b453759-b5f7-4ba6-9933-2ecdfd796ff2)
![grafik](https://github.com/hirseboy/DWD-Weather-Data-Converter/assets/58851829/7baf825d-f5a2-4400-b536-c91eccd5ea3c)
