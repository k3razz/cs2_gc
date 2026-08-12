#include <windows.h>
#include <thread>
#include "hook.h"

DWORD WINAPI GCThread(LPVOID lpParam) {
    HookManager::Instance().Install();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, GCThread, NULL, 0, NULL);
    }
    return TRUE;
}