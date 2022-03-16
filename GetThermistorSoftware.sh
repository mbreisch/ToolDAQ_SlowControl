#!/bin/bash

mkdir -p ToolDAQ
cd ToolDAQ #into ToolDAQ

mkdir YOCTO 
git clone https://github.com/yoctopuce/yoctolib_cpp.git YOCTO
cd YOCTO #into YOCTO
./build.sh
cd Binaries/linux/aarch64
ln -s libyocto.so.1.0.1 libyocto.so
cd ../../../../.. #into Main folder

