@echo off

pushd ..

ctime -begin ti_sem_ray.ctm

set PreprocessorSymbols=

set CompilerOptimizations=-Od -Oi -Gy -fp:except-
set CompilerWarnings=-W4 -WX -wd4201 -wd4100 -wd4127 -wd4189 -wd4101 -wd4505 -wd4702 -wd4060 -wd4065 -wd4723 -wd4701 -wd4408
set CompilerFlags=-nologo -GR- -EHa- -Z7 -Zo -FC -D_CRT_SECURE_NO_WARNINGS %CompilerOptimizations% %CompilerWarnings% %PreprocessorSymbols%

set LinkedLibraries=
set LinkerFlags=/incremental:no /opt:ref /SUBSYSTEM:CONSOLE 

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

cl %CompilerFlags% -Feti_sem_ray ..\code\ti_sem_ray.cpp /link %LinkerFlags% %LinkedLibraries%

popd

ctime -end ti_sem_ray.ctm
