#pragma once

class HookManager {
public:
    static HookManager& Instance();
    void Install();
    void Uninstall();
    
private:
    HookManager() = default;
    ~HookManager() = default;
    HookManager(const HookManager&) = delete;
    HookManager& operator=(const HookManager&) = delete;
    
    bool installed_;
};