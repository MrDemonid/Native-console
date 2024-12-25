#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <string.h>
#include "console.h"


#define _sizeof(array) (sizeof(array) / sizeof(array[0]))

HINSTANCE hInstance = NULL;


BOOL isDebug = FALSE;
HANDLE hCon = NULL;
TCHAR fileName[MAX_PATH];


JNIEXPORT jint JNICALL Java_mr_demonid_Main_logOpenConsole(JNIEnv * env, jobject obj)
{
    if (!isDebug)
    {
        if (hCon == NULL)
        {
            FreeConsole();
            if (AllocConsole())
            {
                SetConsoleTitle(TEXT("Debug"));
                // freopen("CONOUT$", "w", stdout);
                hCon = CreateFile(_T("CONOUT$"), GENERIC_READ+GENERIC_WRITE, FILE_SHARE_READ+FILE_SHARE_WRITE,0,OPEN_EXISTING,0,0);
                if (hCon == INVALID_HANDLE_VALUE)
                {
                    hCon = NULL;
                    return 0;
                }
                isDebug = TRUE;
                return -1;
            }
        }
    }
    return 0;
}


JNIEXPORT jint JNICALL Java_mr_demonid_Main_logOpenFile(JNIEnv *env, jobject obj, jstring filename)
{
    const TCHAR *tmp;

    if (filename == NULL)
        return 0;
    #ifdef UNICODE
        tmp = (*env)->GetStringChars(env, filename, NULL);
    #else
        tmp = (*env)->GetStringUTFChars(env, filename, NULL);
    #endif
    if (tmp != NULL)
        return 0;

    jsize length = (*env)->GetStringLength(env, filename);
    if (length == 0)
    {
        (*env)->ReleaseStringChars(env, filename, (jchar *) tmp);
        return 0;
    }

    _tcscpy_s(fileName, length, tmp);
    fileName[length] = 0;
    (*env)->ReleaseStringChars(env, filename, (jchar *) tmp);

    Java_mr_demonid_Main_logClose(env, obj);
    isDebug = TRUE;
    return -1;
}

JNIEXPORT void JNICALL Java_mr_demonid_Main_logClose(JNIEnv *env, jobject)
{
    if (isDebug) {
        if (hCon != NULL) {
            CloseHandle(hCon);
            FreeConsole();
            hCon = NULL;
        }
    }
    isDebug = FALSE;
}



/**
 * Вывод в консоль ansi-строки
 * Возвращает кол-во записанных символов
 */
int put_ascii_string(const char *buff)
{
    DWORD reads = 0;
    if (isDebug && hCon != NULL && buff != NULL)
    {
        int len = strlen(buff);
        if (len > 0)
            WriteFile(hCon, buff, len, &reads, NULL);   
    }
    return reads;
}

int put_to_file(TCHAR *buff)
{
    DWORD writes = 0;
    HANDLE f;

    if (isDebug && hCon == NULL && buff != NULL)
    {
        if ((f = CreateFile(fileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0)) != INVALID_HANDLE_VALUE)
        {
            SetFilePointer(f, 0, 0, FILE_END);
            int len = _tcslen(buff);
            WriteFile(f, buff, len*sizeof(TCHAR), &writes, NULL);
            CloseHandle(f);
        }
    }
    return writes;
}

void make_date(TCHAR *buff)
{
    SYSTEMTIME time;

    GetLocalTime(&time);
    wsprintf(buff, "%hu.%hu.%hu  %hu:%hu:%hu", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
}


// Вспомогательная функция для преобразования объекта в строку
jstring toString(JNIEnv *env, jobject obj) {
    jclass objClass = (*env)->GetObjectClass(env, obj);
    jmethodID toStringMethod = (*env)->GetMethodID(env, objClass, "toString", "()Ljava/lang/String;");
    return (jstring)(*env)->CallObjectMethod(env, obj, toStringMethod);
}


// // Обработка строки формата и вывод
// void processAndPrint(const char *format, void **argValues, jsize argCount) {
//     const char *p = format;
//     jsize currentArg = 0;

//     while (*p != '\0') {
//         if (*p == '%' && currentArg < argCount) {
//             p++; // Переходим к символу после '%'

//             // Определяем тип аргумента
//             switch (*p) {
//                 case 's': // Строка
//                     printf("%s", (char *)argValues[currentArg]);
//                     break;
//                 case 'd': // Целое число
//                     printf("%d", *(int *)argValues[currentArg]);
//                     break;
//                 case 'l': // Длинное целое (long)
//                     if (*(p + 1) == 'd') {
//                         printf("%ld", *(long *)argValues[currentArg]);
//                         p++; // Пропускаем 'd'
//                     }
//                     break;
//                 case 'f': // float или double
//                     printf("%f", *(float *)argValues[currentArg]);
//                     break;
//                 case '.': // Обработка %.<точность>f
//                     if (*(p + 1) == 'f') {
//                         printf("%f", *(double *)argValues[currentArg]);
//                         p++; // Пропускаем 'f'
//                     }
//                     break;
//                 default:
//                     printf("%%"); // Если не распознано, просто выводим '%'
//                     printf("%c", *p);
//                     break;
//             }
//             currentArg++;
//         } else {
//             // Обычный символ, просто выводим
//             putchar(*p);
//         }
//         p++;
//     }

//     printf("\n");
// }

char *add_string(char* pc, char *string) {
    strcpy(pc, string);
    return pc + strlen(string);
}

char *set_invalid(char *pc) {
    return add_string(pc, "(invalid)");
}

char *get_string(JNIEnv *env, jobject arg, char *pc) 
{
    // jclass argClass = (*env)->GetObjectClass(env, arg);
    jclass stringClass = (*env)->FindClass(env, "java/lang/String");

    if ((*env)->IsInstanceOf(env, arg, stringClass)) 
    {
        const char *str = (*env)->GetStringUTFChars(env, (jstring)arg, NULL);
        strcpy(pc, str);
        pc += strlen(str);
        (*env)->ReleaseStringUTFChars(env, (jstring)arg, str);
    } else {
        pc = set_invalid(pc);
    }
    return pc;
}

char *get_integer(JNIEnv *env, jobject arg, char *pc)
{
    char bnum[256];
    jclass integerClass = (*env)->FindClass(env, "java/lang/Integer");

    if ((*env)->IsInstanceOf(env, arg, integerClass)) 
    {
        jmethodID intValueMethod = (*env)->GetMethodID(env, integerClass, "intValue", "()I");
        jint intValue = (*env)->CallIntMethod(env, arg, intValueMethod);
        sprintf(bnum, "%d", intValue);
        strcpy(pc, bnum);
        pc += strlen(bnum);
    } else {
        pc = set_invalid(pc);
    }
    return pc;
}

char *get_long(JNIEnv *env, jobject arg, char *pc)
{
    char bnum[256];
    jclass longClass = (*env)->FindClass(env, "java/lang/Long");

    if ((*env)->IsInstanceOf(env, arg, longClass)) {
        jmethodID longValueMethod = (*env)->GetMethodID(env, longClass, "longValue", "()J");
        jlong longValue = (*env)->CallLongMethod(env, arg, longValueMethod);
        sprintf(bnum, "%lld", longValue);
        strcpy(pc, bnum);
        pc += strlen(bnum);
    } else {
        pc = set_invalid(pc);
    }
    return pc;
}

char *get_float(JNIEnv *env, jobject arg, char *pc)
{
    char bnum[256];
    jclass floatClass = (*env)->FindClass(env, "java/lang/Float");
    jclass doubleClass = (*env)->FindClass(env, "java/lang/Double");

    if ((*env)->IsInstanceOf(env, arg, floatClass)) {
        jmethodID floatValueMethod = (*env)->GetMethodID(env, floatClass, "floatValue", "()F");
        jfloat floatValue = (*env)->CallFloatMethod(env, arg, floatValueMethod);
        sprintf(bnum, "%f", (float)floatValue);
        strcpy(pc, bnum);
        pc += strlen(bnum);
    } else if ((*env)->IsInstanceOf(env, arg, doubleClass)) {
        jmethodID doubleValueMethod = (*env)->GetMethodID(env, doubleClass, "doubleValue", "()D");
        jdouble doubleValue = (*env)->CallDoubleMethod(env, arg, doubleValueMethod);
        sprintf(bnum, "%f", (double)doubleValue);
        strcpy(pc, bnum);
        pc += strlen(bnum);
    } else {
        pc = set_invalid(pc);
    }
    return pc;
}

JNIEXPORT void JNICALL Java_mr_demonid_Main_logPrintf(JNIEnv *env, jobject obj, jstring format, jobjectArray args) {
    char cache[1024*16];
    char *pc = &cache[0];

    if (format == NULL) 
        return;
    // получаем строку формата
    const char *formatStr = (*env)->GetStringUTFChars(env, format, NULL);
    if (formatStr == NULL)
        return;

    jsize currentArg = 0;
    jsize argCount = (*env)->GetArrayLength(env, args);     // кол. аргументов (не считая строки формата)
    const char *p = formatStr;

    while (*p != '\0') {
        if (*p == '%' && currentArg < argCount) {
            p++;
            jobject arg = (*env)->GetObjectArrayElement(env, args, currentArg++);   // arg - аргумент, соответствующий текущему %x
            if (arg != NULL) {
                switch (*p) {
                    case 's':
                        pc = get_string(env, arg, pc);
                        break;
                    case 'd':
                        pc = get_integer(env, arg, pc);
                        break;
                    case 'l':
                        pc = get_long(env, arg, pc);
                        break;
                    case 'f':
                        pc = get_float(env, arg, pc);
                        break;
                    default:
                        *pc++ = *p;
                }
            } else {
                pc = add_string(pc, "(null)");
            }
        } else {
            *pc++ = *p;
        }
        p++;
    }

    *pc = 0;
    put_ascii_string(cache);
    (*env)->ReleaseStringUTFChars(env, format, formatStr);
}


BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) {
    switch(reason) {
        case DLL_PROCESS_ATTACH:
            hInstance = instance;
            break;
        default:
            break;
    }
    return TRUE;
}
