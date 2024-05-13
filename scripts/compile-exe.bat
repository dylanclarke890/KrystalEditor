@echo off

IF NOT EXIST .\build mkdir .\build
pushd .\build

set editor_start_time=%time%
echo Compiling 'Krystal Editor'.
echo Started at%editor_start_time%.

set editor_defines=^
-DKRYS_ENABLE_ASSERTS=1 ^
-DKRYS_ENABLE_LOGGING=1 ^
-DKRYS_ENABLE_DEBUG_BREAK=1 ^
-DKRYS_ENABLE_PERFORMANCE_CHECKS=1

set editor_include_directories=^
/I "B:\src" ^
/I "K:\src" ^
/I "K:\src\Core" ^
/I "K:\src\Platform" ^
/I "K:\src\ThirdParty\Glad\include\glad" ^
/I "K:\src\ThirdParty\Glad\include" ^
/I "K:\src\ThirdParty\glm" ^
/I "K:\src\ThirdParty\stb"

set editor_source_files=^
B:\src\Entry.cpp ^
B:\src\KrystalEditor.cpp

set editor_linked_libs=^
Krystal.lib ^
user32.lib ^
gdi32.lib ^
OpenGL32.lib ^
Winmm.lib

set editor_disabled_warnings= -wd4100 -wd4201

set editor_compiler_flags=-nologo -Zi -Oi -FC -W4 -WX -MP -EHsc -std:c++latest
set editor_linker_flags=-LIBPATH:"K:\build" %editor_linked_libs%

cl ^
%editor_compiler_flags% ^
%editor_include_directories% ^
%editor_disabled_warnings% ^
%editor_source_files% ^
/link ^
%editor_linker_flags%

popd

set editor_compilation_error=%ERRORLEVEL%
if %editor_compilation_error% equ 0 (
  echo Compilation of 'Krystal Editor' succeeded.
) else (
  echo Compilation of 'Krystal Editor' failed.
)

set editor_end_time=%time%
set /A "editor_start_in_seconds=((%editor_start_time:~0,2%*3600)+(%editor_start_time:~3,2%*60)+%editor_start_time:~6,2%)"
set /A "editor_end_in_seconds=((%editor_end_time:~0,2%*3600)+(%editor_end_time:~3,2%*60)+%editor_end_time:~6,2%)"
set /A "editor_compilation_duration_in_seconds=%editor_end_in_seconds%-%editor_start_in_seconds%"
echo Finished at%editor_end_time% (%editor_compilation_duration_in_seconds%s).

exit /b %editor_compilation_error%
