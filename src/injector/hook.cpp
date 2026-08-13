#include "hook.h"
#include <cstdio>
#include <funchook.h>
#include <cstdint>
#include <string>
#include "../gc/gc_server.h"

typedef void (*GCSendMessage_t)(uint32_t msg_type, const void* data, uint32_t size);
GCSendMessage_t OriginalGCSendMessage = nullptr;
funchook_t* g_funchook = nullptr;

void HookedGCSendMessage(uint32_t msg_type, const void* data, uint32_t size) {
    std::string raw_data(static_cast<const char*>(data), size);
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
        
        g_funchook = funchook_create();
        if (g_funchook) {
            int status = funchook_prepare(g_funchook, (void**)&OriginalGCSendMessage, (void*)HookedGCSendMessage);
            if (status == 0) {
                status = funchook_install(g_funchook, 0);
                if (status == 0) {
                    installed_ = true;
                    printf("[GC] Hook installed successfully\n");
                } else {
                    printf("[GC] Failed to install hook: %d\n", status);
                }
            } else {
                printf("[GC] Failed to prepare hook: %d\n", status);
            }
        } else {
            printf("[GC] Failed to create funchook\n");
        }
    } else {
        printf("[GC] Failed to find SendMessage function\n");
    }
}

void HookManager::Uninstall() {
    if (!installed_) return;
    
    if (g_funchook) {
        funchook_uninstall(g_funchook, 0);
        funchook_destroy(g_funchook);
        g_funchook = nullptr;
    }
    
    installed_ = false;
    printf("[GC] Hook uninstalled\n");
}