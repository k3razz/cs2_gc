#pragma once
#include <cstdint>

namespace SteamInterop {

typedef void (*GCSendMessageFn)(uint32_t msg_type, const void* data, uint32_t size);

bool Initialize();
void Shutdown();
void SetGCSendMessageHook(GCSendMessageFn hook);
void* FindGCSendMessage();
bool InstallHooks();
void UninstallHooks();

}
