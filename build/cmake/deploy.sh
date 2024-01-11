#!/bin/bash

cqtdeployer -confFile CQtDeployer.json -bin ../../bin/release/DWDWeatherDataConverter -qmake ~/Qt/5.15.2/gcc_64/bin/qmake -name "DWDWeatherDataConverter" -icon "../../DWDWeatherDataConverter/resources/logo/icons/Icon_512.png" deb -tr ../../DWDWeatherDataConverter/resources/translations
