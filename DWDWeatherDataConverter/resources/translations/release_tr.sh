#!/bin/bash

export PATH=~/Qt/5.15.2/gcc_64/bin/:$PATH

lrelease ../../../externals/QtExt/resources/translations/QtExt_de.ts ../../../externals/DataMap/resources/translations/DataMap_de.ts DWDWeatherDataConverter_de.ts -qm DWDWeatherDataConverter_de.qm


