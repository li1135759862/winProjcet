

#include "pch.h"

#include "framework.h"
#include "WinProject.h"
#include <iostream>

using namespace std;


void outInt(int number) {
   printf("The number is: %d\n", number);
    
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{

    /*HANDLE hFile = CreateFileW(L"1.txt",GENERIC_ALL,0,NULL,OPEN_ALWAYS,NULL,NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        OutputDebugStringA("open faild!");
        return 1;
    }*/

    HMODULE DLL = LoadLibraryA("MyDill");
    if (!DLL) {
        OutputDebugStringA("load fail dill!\n");
        
    }
    else {
        OutputDebugStringA("load  success dill!\n");
        int var_c__ = *(int*)GetProcAddress(DLL, "?var_c__@@3HA");
        int var_c = *(int*)GetProcAddress(DLL, "var_c");
        outInt(var_c__);
        outInt(var_c);

        typedef int (*FUN)();//定义一个函数
        FUN fun_c = (FUN)GetProcAddress(DLL, "fun_c");
        cout << (DWORD)fun_c << endl;

        
        outInt(fun_c());
        wprintf(L"The number is: %d\n", fun_c());
        OutputDebugStringA("open success!\n");

        //定义hook
        HHOOK(*StartHook)(CHAR type, DWORD pid) = (HHOOK(*)(CHAR type, DWORD pid))GetProcAddress(DLL, "StartHook");
       
        bool(*unHook)(HHOOK hook) = (bool(*)(HHOOK hook))GetProcAddress(DLL, "unHook");

        HHOOK hook = StartHook(9, 0);
        BYTE input = 0;

        
        while (true)
        {
            cin >> input;
        }
        unHook(hook);


    }

    
    return (0);
}