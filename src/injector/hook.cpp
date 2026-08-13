#include "hook.h"
#include <cstdio>
#include <cstdint>
#include <string>
#include <windows.h>
#include "../gc/gc_server.h"

typedef void (*GCSendMessage_t)(uint32_t msg_type, const void* data, uint32_t size);
GCSendMessage_t OriginalGCSendMessage = nullptr;

void HookedGCSendMessage(uint32_t msg_type, const void* data, uint32_t size) {
    CS2GC::GCServer::Instance().ProcessMessage(msg_type, data, size);
}

void* FindGCSendMessage() {
    HMODULE hSteam = GetModuleHandleA("steam_api64.dll");
    if (!hSteam) {
        hSteam = GetModuleHandleA("steamclient64.dll");
    }
    if (!hSteam) {
        return nullptr;
    }
    
    FARPROC addr = GetProcAddress(hSteam, "SteamAPI_ISteamGameCoordinator_SendMessage");
    if (addr) {
        return addr;
    }
    
    return nullptr;
}

HookManager& HookManager::Instance() {
    static HookManager instance;
    return instance;
}

void HookManager::Install() {
    if (installed_) return;
    
    void* target = FindGCSendMessage();
    if (target) {
        OriginalGCSendMessage = reinterpret_cast<GCSendMessage_t>(target);
        installed_ = true;
        printf("[GC] Hook installed successfully\n");
    } else {
        printf("[GC] Failed to find SendMessage function\n");
    }
}

void HookManager::Uninstall() {
    installed_ = false;
    printf("[GC] Hook uninstalled\n");
}