# DWD Weather Data Converter

![grafik](https://github.com/hirseboy/DWD-Weather-Data-Converter/assets/58851829/453ab876-abd7-498d-934b-c952ebbf1b61)

This repository holds all data concerning the weather data converter for generating climate files from measurement Data provided by the [German Weather Service (DWD)](https://www.dwd.de/DE/Home/home_node.html).

It is accessing measurement data stored on the ftp server and all data can be combined into weather-files (epw, c6b) needed for simulation tools (SIM-VICUS, DELPHIN, etc.)

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

![screenhot 01](https://github.com/hirseboy/DWD-Weather-Data-Converter/assets/58851829/02995f1f-5593-43d5-940d-3c936ec616b8)
![screenhot 02](https://github.com/hirseboy/DWD-Weather-Data-Converter/assets/58851829/0166dd6a-da86-455b-8d58-2f4be3f6e4f2)
![screenhot 03](https://github.com/hirseboy/DWD-Weather-Data-Converter/assets/58851829/8f5488d2-bf79-47e9-adcc-0a4431e80bcb)
![screenhot 04](https://github.com/hirseboy/DWD-Weather-Data-Converter/assets/58851829/bb62f65e-c4fc-4371-ad8b-78d81e19a5b7)
