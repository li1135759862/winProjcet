// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <iostream>
#include <cstdio>
#define MemHoookLen 12

BYTE oldRecoveryMem[MemHoookLen]; //之前地址信息

BYTE oldRecoveryMytext[MemHoookLen]; //之前地址信息
DWORD oldParam;//之前属性



//内存钩子 ,存储old地址
// APIAddress 原地址 jmpAddress 跳转的地址
BYTE* memCall(LPVOID APIAddress , LPVOID jmpAddress) {

    

    //修改保护属性
   ;
   if (VirtualProtectEx(GetCurrentProcess(), APIAddress, MemHoookLen, PAGE_EXECUTE_READWRITE, &oldParam)) {
       if (!ReadProcessMemory(GetCurrentProcess(), APIAddress, oldRecoveryMem, MemHoookLen, NULL)) {
           printf("读取内存失败\n", GetLastError());

       }
       else
       {
           //32 5  64 12
           CHAR Shellcode[MemHoookLen];
           ZeroMemory(Shellcode, MemHoookLen);
           Shellcode[0] = 0xE9; //jmp
           *(DWORD*)(Shellcode + 1) = (DWORD)APIAddress - (DWORD)jmpAddress - 5;
           if (!WriteProcessMemory(GetCurrentProcess(), APIAddress, Shellcode, MemHoookLen, NULL)) {
               printf("修改内存失败\n", GetLastError());
           }
       }
      
   }
   VirtualProtectEx(GetCurrentProcess(), APIAddress, MemHoookLen, oldParam, &oldParam);

   return oldRecoveryMem;
}
//回复数据
BOOL recoveryMemFun(LPVOID APIAddress, BYTE* data) {


    if (!WriteProcessMemory(GetCurrentProcess(), APIAddress, data, MemHoookLen, NULL)) {
        printf("修改内存失败\n", GetLastError());
        return -1;
    }
    return 0;
}

//自己要替换的函数
BOOL WINAPI MyTextOutW(_In_ HDC hdc, _In_ int x, _In_ int y, _In_reads_(c) LPCWSTR lpString, _In_ int c) {
    wprintf(lpString);
    //自己操作

    //回复内存
    recoveryMemFun(TextOutW, oldRecoveryMytext);

    //回复函数
    return TextOutW(hdc, x, y, lpString, c);
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        BYTE* tmp = memCall(TextOutW, MyTextOutW);
        memcpy(oldRecoveryMytext, tmp, MemHoookLen);
        break;
    }
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

