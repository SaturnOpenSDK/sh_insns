#!/bin/sh

#echo "converting..."
# inkscape movi20.pdf --export-plain-svg=movi20.svg

echo "compiling..."
if [ ! -e "build_instructions.o" ];then
 c++ -std=c++17 -O2 -c build_instructions.cpp
fi
c++ -std=c++17 -O2 sh_insns.cpp build_instructions.o -o sh_insns

echo "executing..."
./sh_insns > sh_insns.html

echo "done"

