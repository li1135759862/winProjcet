// WinShell.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <tlhelp32.h>
#include <Psapi.h>



#include <string>

//ProcessPhone进程快照
HANDLE ProcessList = 0;
//ProcessClass 进程结构
PROCESSENTRY32 ProcessClass = { 0 };
//进程
DWORD DebugPID = 0;
//
HANDLE DebugProcessHandel = 0;

//PidName 进程名
CHAR SetProcess(const std::wstring targetProcessName) {
    ProcessClass.dwSize = sizeof(PROCESSENTRY32);
    //进程路径
    TCHAR ProcessPaht[MAX_PATH];
    //
    ZeroMemory(ProcessPaht, MAX_PATH);
    //快照
    ProcessList = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    while (Process32Next(ProcessList,&ProcessClass)) {
        //DebugProcessHandel = OpenProcess(PROCESS_ALL_ACCESS | THREAD_ALL_ACCESS, FALSE, ProcessClass.th32ProcessID);
        DebugProcessHandel = OpenProcess(PROCESS_ALL_ACCESS | THREAD_ALL_ACCESS, FALSE, ProcessClass.th32ProcessID);


        GetProcessImageFileName(DebugProcessHandel, ProcessPaht, MAX_PATH);

        std::wstring currentProcessName(ProcessPaht);
        if (!currentProcessName.empty() && currentProcessName.find(targetProcessName) != std::wstring::npos) {
            // 打印进程的镜像文件路径。
            std::wcout << L"Process ID: " << ProcessClass.th32ProcessID << L", Image Filename: " << ProcessPaht << std::endl;

        }
       

    }

    printf("中文");
    return 0;
}



struct  SheelCodeStruct
{
    BYTE PUSH = 0x68; DWORD EIP_ADDRESS;
    BYTE PUSHFD = 0x9C;
    BYTE PUSHAD = 0x60;
    BYTE MOV_EAX = 0xB8; DWORD MOVE_EAX_ADDRESS;//保存函数参数地址
    BYTE PUSH_EAX = 0x50;
    BYTE MOV_ECX = 0xB9; DWORD MOVE_ECX_ADDRESS;//保存函数参数地址
    WORD CALL_ECX = 0xD1FF;
    BYTE POPAD = 0X61;
    BYTE POPFD = 0X90;
    BYTE RETN = 0XC3;
    CHAR DLLPath[MAX_PATH];
};
SheelCodeStruct SheelCode;

//ProcessPhone进程快照
HANDLE snaProcessList = 0;
//ProcessClass 线程结构
THREADENTRY32 snaProcessClass = { 0 };
//进程
DWORD snaDebugPID = 0;
//
HANDLE snaDebugProcessHandel = 0;
LPVOID applyMemAddress = 0;
//eip注入dll
CHAR EIPushDll(const CHAR* dllPath) {
    snaProcessClass.dwSize = sizeof(PROCESSENTRY32);
    applyMemAddress=VirtualAllocEx(snaDebugProcessHandel, NULL, sizeof(SheelCode),MEM_COMMIT,PAGE_EXECUTE_READWRITE);
    //快照
    snaProcessList = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    while (Thread32Next(snaProcessList, &snaProcessClass)) {


        if (snaProcessClass.th32OwnerProcessID == DebugPID) {
            break;
        }
        HANDLE DebugThreadHandle= OpenProcess(THREAD_ALL_ACCESS, FALSE, snaProcessClass.th32ThreadID);
        SuspendThread(DebugThreadHandle);
        CONTEXT ThreadContext;
        ThreadContext.ContextFlags = CONTEXT_FULL;
        GetThreadContext(DebugThreadHandle,&ThreadContext);//线程上下文
        SheelCode.EIP_ADDRESS = ThreadContext.EiP; //备份EIP
        ThreadContext.EiP = (SIZE_T)applyMemAddress;
        SheelCode.MOVE_EAX_ADDRESS = (SIZE_T)applyMemAddress + offsetof(SheelCodeStruct,DLLPath); //dll地址
        SheelCode.MOVE_ECX_ADDRESS = (SIZE_T)LoadLibraryA;//LoadLibraryW 宽字节

        memcpy(SheelCode.DLLPath, dllPath, strlen(dllPath) + 1);
        WriteProcessMemory(snaDebugProcessHandel, applyMemAddress, &SheelCode, sizeof(SheelCode),NULL);

        //修改回来，继续执行
        SetThreadContext(DebugThreadHandle, &ThreadContext);
        ResumeThread(DebugThreadHandle);
        CloseHandle(DebugThreadHandle);
        CloseHandle(snaDebugProcessHandel);
        DebugThreadHandle = NULL;
        snaDebugProcessHandel = NULL;



    }
}



std::string GetProcessPath(DWORD processId)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (hProcess != NULL)
    {
        HMODULE hModule;
        DWORD cbNeeded;
        if (EnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbNeeded))
        {
            TCHAR szBuffer[MAX_PATH];
            if (GetModuleFileNameEx(hProcess, hModule, szBuffer, MAX_PATH))
            {
                std::wstring widePath(szBuffer);
                std::string processPath(widePath.begin(), widePath.end());
                return processPath;
            }
        }
    }

    return "";
}

void EnumerateProcesses1()
{
    DWORD processes[1024];
    DWORD cbNeeded;
    if (EnumProcesses(processes, sizeof(processes), &cbNeeded))
    {
        DWORD count = cbNeeded / sizeof(DWORD);
        for (DWORD i = 0; i < count; i++)
        {
            DWORD processId = processes[i];
            std::string processPath = GetProcessPath(processId);
            if (!processPath.empty())
            {
                std::cout << "Process ID: " << processId << std::endl;
                std::cout << "Process Path: " << processPath << std::endl;
                std::cout << std::endl;
            }
        }
    }
}


void EnumProcessesGpt() {
    // 获取系统中正在运行的所有进程
    DWORD processes[1024];
    DWORD bytesReturned;
    if (!EnumProcesses(processes, sizeof(processes), &bytesReturned)) {
        std::cerr << "Failed to enumerate processes." << std::endl;
        return ;
    }

    // 计算进程数量
    int numProcesses = bytesReturned / sizeof(DWORD);

    // 遍历每个进程并获取其映像文件名
    for (int i = 0; i < numProcesses; ++i) {
        DWORD processID = processes[i];
        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

        if (processHandle != nullptr) {
            TCHAR szProcessName[MAX_PATH];
            if (GetProcessImageFileName(processHandle, szProcessName, MAX_PATH) > 0) {
                std::wcout << L"Process ID: " << processID << L", Image Filename: " << szProcessName << std::endl;
            }
            CloseHandle(processHandle);
        }
    }
}


int main(_In_ int _Argc, _In_reads_(_Argc) _Pre_z_ char** _Argv, _In_z_ char** _Env)
{
    EnumerateProcesses1();
    printf("------------------------------------------------------------------------------------------");
    //SetProcess(L"");
    SetProcess(L"dllhost.exe");
    printf("中文");
    // 关闭进程枚举器。
    return 0;
}
