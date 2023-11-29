// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <iostream>
#include <cstdio>
using namespace std;


//实例句柄
HINSTANCE handle = 0;
HHOOK hook;

//c++导出变量,变量名会发生变化，使用dumpbin /exports MyDill.dll 查看正是变量
int __declspec(dllexport) var_c__ = 10;
//c导出变量
extern "C" int __declspec(dllexport) var_c = 10;
//c 导出函数

extern "C"  __declspec(dllexport) int fun_c() {
    return 12345;
}

void PrintWParam(WPARAM wParam)
{
    char str[64];
    sprintf_s(str, sizeof(str), "%lu", wParam);
    OutputDebugStringA(str);
}

LRESULT CALLBACK CALLBACK_WH_GETMESSAGE(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM IParam) {
    //拦截POSTmessage消息
    OutputDebugStringA(" CALLBACK_WH_GETMESSAGE!\n");
    return CallNextHookEx(NULL, nCode, wParam, IParam);
}

LRESULT CALLBACK CALLBACK_WH_CALLWNDPROC(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM IParam) {
    //拦截SENDmessage消息
    OutputDebugStringA(" CALLBACK_WH_CALLWNDPROC!\n");
    return CallNextHookEx(NULL, nCode, wParam, IParam);
}

LRESULT CALLBACK CALLBACK_DEBUG(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM IParam) {
    //拦截钩子类型
    OutputDebugStringA(" CALLBACK_DEBUG!\n");
    //钩子加载前处理
    //switch (wParam)
    //{
    //case WH_KEYBOARD: //加载
    //    break;
    //
    //default:
    //    break;
    //}
    return CallNextHookEx(NULL, nCode, wParam, IParam);
}

LRESULT CALLBACK CALLBACK_WH_DEBUG(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM IParam) {
    //拦截钩子类型
    OutputDebugStringA(" CALLBACK_WH_DEBUG!\n");
    return CallNextHookEx(NULL, nCode, wParam, IParam);
}


LRESULT CALLBACK CALLBACK_WH_FORDGROUNDIDLE(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM IParam) {
    //拦截线程休眠
    OutputDebugStringA(" CALLBACK_WH_FORDGROUNDIDLE!\n");
    return CallNextHookEx(NULL, nCode, wParam, IParam);
}

LRESULT CALLBACK CALLBACK_WH_MOUSE_LL(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM IParam) {
    //鼠标 能判断是模拟还是硬件
    OutputDebugStringA(" CALLBACK_WH_MOUSE_LL!\n");
    if (nCode ==  HC_ACTION) {
        MOUSEHOOKSTRUCT pMouseHook = *((PMOUSEHOOKSTRUCT)IParam);
        if (pMouseHook.wHitTestCode) {
            OutputDebugStringA("模拟移动!\n");
        }
        switch (wParam)
        {
        case WM_MOUSEMOVE: //加载
            OutputDebugStringA("鼠标移动!\n");
            break;
        case WM_LBUTTONDOWN: //加载
            OutputDebugStringA("鼠标点击!\n");
            break;
        case WM_LBUTTONUP: //加载
            OutputDebugStringA("鼠标拿起!\n");
            break;
        default:
            break;
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, IParam);
}


LRESULT CALLBACK CALLBACK_WH_KEYBOARD(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM IParam) {
    //拦截键盘
    /*OutputDebugStringA(" CALLBACK_WH_KEYBOARD!\n");*/

    KBDLLHOOKSTRUCT *key = (KBDLLHOOKSTRUCT*)IParam;
    if (nCode == HC_ACTION) {
        
        //PrintWParam(wParam);
        printf("拦截键盘%d\n", key->vkCode);
        printf("拦截键盘%d\n", key->scanCode);
        printf("拦截键盘%d\n", key->scanCode);
        
    }

    return CallNextHookEx(NULL, nCode, wParam, IParam);
}

//
LRESULT CALLBACK CALLBACK_WH_CBT(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM IParam) {
    OutputDebugStringA(" CALLBACK_WH_CBT!\n");
    //拦截所有窗口事件
    return CallNextHookEx(NULL, nCode, wParam, IParam);
}



//导出hook函数，导出给我们自己的应用程序使用
//type 钩子类型 pid 线程id
extern "C"  __declspec(dllexport) HHOOK StartHook(CHAR type,DWORD pid) {
    OutputDebugStringA(" StartHook!\n");
    switch (type)
    {
    case 1: //加载
        hook=SetWindowsHookEx(WH_GETMESSAGE, CALLBACK_WH_GETMESSAGE, handle, pid);
        break;
    case 2: //加载
        break;
    case 3: //加载
        break;
    case 4: //加载
        break;
    case 5: //加载
        break;
    case 6: //加载
        break;
    case 7: //加载
        break;
    case 8: //加载
        break;
    case 9: //加载
        hook=SetWindowsHookEx(WH_KEYBOARD_LL, CALLBACK_WH_KEYBOARD, handle, pid);
        MSG msg;
        while (GetMessage(&msg,NULL,0,0))
        {
            //虚拟消息转字符
            TranslateMessage(&msg);
            //传递消息给我们的应用处理
            DispatchMessageW(&msg);
        }
        break;
    case 10: //加载
        break;
    case 11: //加载
        break;
    case 12: //加载
        break;
    case 13: //加载
        break;
    default:
        break;
    }
    return hook;
}

extern "C"  __declspec(dllexport) bool unHook(CHAR type, DWORD pid) {
    return UnhookWindowsHookEx(hook);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    
    switch (ul_reason_for_call)
    {

        OutputDebugStringA("MyDill switch!\n");
    case DLL_PROCESS_ATTACH: //加载
        //
        handle = hModule;
        OutputDebugStringA("load MyDill!\n");
        
        break;
    case DLL_THREAD_ATTACH: //新线程
        OutputDebugStringA("DLL_THREAD_ATTACH MyDill!\n");
        break;
    case DLL_THREAD_DETACH://线程关闭
        OutputDebugStringA("OutputDebugStringA MyDill!\n");
        break;
    case DLL_PROCESS_DETACH://卸载

        OutputDebugStringA("unload MyDill!\n");
        break;
    }
    return TRUE;
}

