#!/bin/bash

mkdir -p ToolDAQ
cd ToolDAQ #into ToolDAQ

mkdir YOCTO 
git clone https://github.com/yoctopuce/yoctolib_cpp.git YOCTO
cd YOCTO #into YOCTO
./build.sh
cp ./Sources/yocto_api.h ./Sources/yocto_api.cpp ./Sources/yocto_temperature.h ./Sources/yocto_temperature.cpp ./Sources/yocto_genericsensor.h ./Sources/yocto_genericsensor.cpp ../../DataModel/
cd .. #into ToolDAQ
cd .. #into Main folder

