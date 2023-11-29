// WinShell.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <tlhelp32.h>
#include <Psapi.h>

//ProcessPhone进程快照
HANDLE ProcessList = 0;
//ProcessClass 进程结构
PROCESSENTRY32 ProcessClass = { 0 };
//进程
DWORD DebugPID = 0;
//
HANDLE DebugProcessHandel = 0;

//PidName 进程名
CHAR SetProcess(CONST CHAR* PidName ) {
    ProcessClass.dwSize = sizeof(PROCESSENTRY32);
    //进程路径
    TCHAR ProcessPaht[MAX_PATH];
    //
    ZeroMemory(ProcessPaht, MAX_PATH);
    //快照
    ProcessList = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    while (Process32Next(ProcessList,&ProcessClass)) {
        DebugProcessHandel = OpenProcess(PROCESS_ALL_ACCESS | THREAD_ALL_ACCESS, FALSE, ProcessClass.th32ProcessID);
        GetProcessImageFileName(DebugProcessHandel, ProcessPaht, MAX_PATH);
        // 打印进程的镜像文件路径。
        printf("Process image file path: %s\n", ProcessPaht);

    }
    return 0;
}



int main(_In_ int _Argc, _In_reads_(_Argc) _Pre_z_ char** _Argv, _In_z_ char** _Env)
{
    // 创建一个进程枚举器。
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    // 获取第一个进程的句柄。
    PROCESSENTRY32 pe32 = { 0 };
   
    //
    HANDLE DebugProcessHandel = 0;
    pe32.dwSize = sizeof(pe32);
    if (Process32First(hProcessSnap, &pe32)) {
        // 循环获取所有进程的句柄。
        do {
            // 获取进程的镜像文件路径。
            TCHAR szImageFileName[MAX_PATH];
            DWORD nSize = MAX_PATH;
            DebugProcessHandel = OpenProcess(PROCESS_ALL_ACCESS | THREAD_ALL_ACCESS, FALSE, pe32.th32ProcessID);

            GetProcessImageFileName(DebugProcessHandel, szImageFileName, nSize);

            // 打印进程的镜像文件路径。
            printf("Process image file path: %s\n", szImageFileName);

        } while (Process32Next(hProcessSnap, &pe32));
    }

    // 关闭进程枚举器。
    CloseHandle(hProcessSnap);
    return 0;
}
