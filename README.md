# Native-console

## Создание заголовка

  javac -h . src/main/java/mr/demonid/Main.java

В корневой папке должен появиться файл:
    `<GroupId>_Main.h`
 
Это и есть инклюд, для подключения к нашему C++ проекту, 
в который будут включены все объявленные
как `public native` методы.


## Компиляция

```shell
gcc -Wl,--add-stdcall-alias ^
-I"%JAVA_HOME%\include" ^
-I"%JAVA_HOME%\include\win32" ^
-shared -o main.dll Main.c
```

или так:

```shell
gcc -Wl,--strip-all -fdiagnostics-color=always ^
-I"%JAVA_HOME%\include" ^
-I"%JAVA_HOME%\include\win32" ^
-shared -o mcon.dll main.c %MINGW_HOME%\lib\libwinmm.a
```
