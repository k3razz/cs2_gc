#pragma once
#include <string>
#include "../gc_server.h"

namespace CS2GC {

class StoreManager {
public:
    static StoreManager& Instance();
    
    void Init(const std::string& config_path);
    Item OpenCrate(uint32_t crate_def_index, uint32_t key_def_index);
    Item PurchaseItem(uint32_t def_index);
    uint32_t GetPrice(uint32_t def_index);
    uint32_t GetCurrency();
    void AddCurrency(uint32_t amount);
    bool HasKey(uint32_t key_def_index);
    void ConsumeKey(uint32_t key_def_index);
    
private:
    StoreManager() = default;
    ~StoreManager() = default;
    StoreManager(const StoreManager&) = delete;
    StoreManager& operator=(const StoreManager&) = delete;
    
    uint32_t currency_;
    bool initialized_;
    std::string config_path_;
};

}