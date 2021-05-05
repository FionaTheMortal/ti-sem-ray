#!/usr/bin/env sh

CompilerOptimizations="-O3 -g"
CompilerWarnings="-Werror -Wall -Wno-unused-function -Wno-unused-variable"
CompilerFlags="$CompilerOptimizations $CompilerWarnings $PreprocessorSymbols"

LinkedLibraries=
LinkerFlags=

cd ..

g++ $CompilerFlags code/ti_sem_ray.cpp -o build/ti_sem_ray

cd - 