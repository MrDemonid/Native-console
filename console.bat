rem Должны быть созданы две переменных окружения:
rem MINGW_HOME - путь до корневого каталога MinGW
rem %JIDEA_NATIVE_PROJECT - путь до корневого каталога Java-проекта, использующего этот DLL

del mcon.dll
del mcon.o

gcc -Wl,--strip-all -fdiagnostics-color=always -I"%JAVA_HOME%\include" -I"%JAVA_HOME%\include\win32" -shared -o mcon.dll console.c %MINGW_HOME%\lib\libwinmm.a

copy /b /Y mcon.dll "%JIDEA_NATIVE_PROJECT%\mcon.dll"

pause
