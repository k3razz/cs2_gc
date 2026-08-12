#pragma once
#include <cstdint>
#include <string>
#include <map>
#include <vector>

namespace CS2GC {

struct ItemDef {
    std::string name;
    uint32_t def_index;
    float base_wear;
    bool is_stattrak;
    uint32_t rarity;
    uint32_t crate_type;
};

struct CrateLoot {
    uint32_t crate_def_index;
    std::vector<uint32_t> item_defs;
    std::vector<float> weights;
};

class SchemaManager {
public:
    static SchemaManager& Instance();
    
    void LoadSchema();
    ItemDef GetItemDef(uint32_t def_index);
    CrateLoot GetCrateLoot(uint32_t crate_def_index);
    uint32_t GetStorePrice(uint32_t def_index);
    bool IsValidItem(uint32_t def_index);
    bool IsValidCrate(uint32_t def_index);
    bool IsValidKey(uint32_t def_index);
    
private:
    SchemaManager() = default;
    ~SchemaManager() = default;
    SchemaManager(const SchemaManager&) = delete;
    SchemaManager& operator=(const SchemaManager&) = delete;
    
    std::map<uint32_t, ItemDef> schema_;
    std::map<uint32_t, CrateLoot> crate_loot_;
    std::map<uint32_t, uint32_t> store_prices_;
    bool loaded_;
};

}