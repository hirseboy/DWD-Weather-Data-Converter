#!/bin/bash

export PATH=~/Qt/5.15.2/gcc_64/bin:$PATH

lupdate ../../projects/Qt/DataMap.pro -ts DataMap_de.ts

linguist DataMap_de.ts
