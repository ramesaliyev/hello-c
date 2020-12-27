@REM Y:\Projects\Personal\hello-c\projects\course-automation\test\windows\run.cmd

"C:/Dev-Cpp/bin/gcc.exe" "Y:\Projects\Personal\hello-c\projects\course-automation\main.c" -o "Y:\Projects\Personal\hello-c\projects\course-automation\build\main.exe" -I"C:\Dev-Cpp\include" -L"C:\Dev-Cpp\lib"

cmd /k "Y: & cd Y:\Projects\Personal\hello-c\projects\course-automation & .\build\main"