#include "hook.h"
#include <detours.h>
#include <cstdint>
#include "../gc/gc_server.h"

typedef void (*GCSendMessage_t)(uint32_t msg_type, const void* data, uint32_t size);
GCSendMessage_t OriginalGCSendMessage = nullptr;

void HookedGCSendMessage(uint32_t msg_type, const void* data, uint32_t size) {
    std::string raw_data(static_cast<const char*>(data), size);
    CS2GC::GCServer::Instance().ProcessMessage(msg_type, raw_data);
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
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)OriginalGCSendMessage, HookedGCSendMessage);
        DetourTransactionCommit();
        installed_ = true;
    }
}

void HookManager::Uninstall() {
    if (!installed_) return;
    
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&(PVOID&)OriginalGCSendMessage, HookedGCSendMessage);
    DetourTransactionCommit();
    installed_ = false;
}