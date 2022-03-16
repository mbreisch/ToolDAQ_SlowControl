#!/bin/bash

mkdir -p ToolDAQ
cd ToolDAQ

mkdir YOCTO
git clone https://github.com/yoctopuce/yoctolib_cpp.git YOCTO
cd YOCTO
./build.sh
cd ..
cd ..

