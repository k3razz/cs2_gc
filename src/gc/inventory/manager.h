#pragma once
#include <string>
#include <vector>
#include "../../../include/gc_api.h"

namespace CS2GC {

class InventoryManager {
public:
    static InventoryManager& Instance();
    
    void Load(const std::string& config_path);
    void Save();
    std::vector<GCItem> GetAllItems();
    void AddItem(const GCItem& item);
    void UpdateItem(const GCItem& item);
    void RemoveItem(uint64_t item_id);
    GCItem GetItem(uint64_t item_id);
    uint32_t GetVersion() const;
    
private:
    InventoryManager() = default;
    ~InventoryManager() = default;
    InventoryManager(const InventoryManager&) = delete;
    InventoryManager& operator=(const InventoryManager&) = delete;
    
    std::vector<GCItem> items_;
    std::string inventory_path_;
    uint32_t version_;
    bool loaded_;
};

}
