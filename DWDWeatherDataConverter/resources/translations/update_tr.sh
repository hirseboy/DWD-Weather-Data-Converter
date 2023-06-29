#!/bin/bash

export PATH=~/Qt/5.15.2/gcc_64/bin:$PATH
lupdate ../../projects/Qt/DWDWeatherDataConverter.pro

linguist DWDWeatherDataConverter_de.ts

