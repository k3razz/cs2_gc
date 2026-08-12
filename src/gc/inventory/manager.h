#pragma once
#include <string>
#include <vector>
#include "../gc_server.h"

namespace CS2GC {

class InventoryManager {
public:
    static InventoryManager& Instance();
    
    void Load(const std::string& config_path);
    void Save();
    std::vector<Item> GetAllItems();
    void AddItem(const Item& item);
    void UpdateItem(const Item& item);
    void RemoveItem(uint64_t item_id);
    Item GetItem(uint64_t item_id);
    uint32_t GetVersion() const;
    
private:
    InventoryManager() = default;
    ~InventoryManager() = default;
    InventoryManager(const InventoryManager&) = delete;
    InventoryManager& operator=(const InventoryManager&) = delete;
    
    std::vector<Item> items_;
    std::string inventory_path_;
    uint32_t version_;
    bool loaded_;
};

}