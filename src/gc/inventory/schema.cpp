#include "schema.h"
#include <cstdio>

namespace CS2GC {

SchemaManager& SchemaManager::Instance() {
    static SchemaManager instance;
    return instance;
}

void SchemaManager::LoadSchema() {
    if (loaded_) return;
    
    schema_[1] = {"AK-47 | Redline", 1, 0.15f, true, 4, 0};
    schema_[2] = {"M4A4 | Howl", 2, 0.08f, true, 5, 0};
    schema_[3] = {"AWP | Dragon Lore", 3, 0.02f, true, 5, 0};
    schema_[4] = {"Desert Eagle | Blaze", 4, 0.10f, false, 4, 0};
    schema_[5] = {"USP-S | Kill Confirmed", 5, 0.12f, true, 4, 0};
    schema_[6] = {"Glock-18 | Fade", 6, 0.05f, true, 5, 0};
    schema_[7] = {"M4A1-S | Hyper Beast", 7, 0.18f, true, 4, 0};
    schema_[8] = {"AWP | Gungnir", 8, 0.03f, true, 5, 0};
    schema_[9] = {"AK-47 | Fire Serpent", 9, 0.06f, true, 5, 0};
    schema_[10] = {"P250 | Splash", 10, 0.20f, false, 3, 0};
    
    schema_[1001] = {"Operation Bravo Case", 1001, 0.0f, false, 0, 1001};
    schema_[1002] = {"Chroma Case", 1002, 0.0f, false, 0, 1002};
    schema_[1003] = {"Spectrum Case", 1003, 0.0f, false, 0, 1003};
    schema_[1004] = {"Prisma Case", 1004, 0.0f, false, 0, 1004};
    schema_[1005] = {"Glove Case", 1005, 0.0f, false, 0, 1005};
    
    schema_[2001] = {"Team Liquid | Katowice 2026", 2001, 0.0f, false, 0, 0};
    schema_[2002] = {"Natus Vincere | Cologne 2026", 2002, 0.0f, false, 0, 0};
    schema_[2003] = {"FaZe Clan | Major 2026", 2003, 0.0f, false, 0, 0};
    
    schema_[3001] = {"StatTrak Swap Tool", 3001, 0.0f, false, 0, 0};
    schema_[3002] = {"Name Tag", 3002, 0.0f, false, 0, 0};
    schema_[3003] = {"Souvenir Token", 3003, 0.0f, false, 0, 0};
    
    CrateLoot bravo;
    bravo.crate_def_index = 1001;
    bravo.item_defs = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    bravo.weights = {0.40f, 0.30f, 0.15f, 0.08f, 0.05f, 0.02f, 0.0f, 0.0f, 0.0f, 0.0f};
    crate_loot_[1001] = bravo;
    
    CrateLoot chroma;
    chroma.crate_def_index = 1002;
    chroma.item_defs = {1, 4, 5, 6, 7, 8, 9, 10};
    chroma.weights = {0.35f, 0.25f, 0.20f, 0.12f, 0.06f, 0.02f, 0.0f, 0.0f};
    crate_loot_[1002] = chroma;
    
    CrateLoot spectrum;
    spectrum.crate_def_index = 1003;
    spectrum.item_defs = {2, 3, 5, 7, 8, 9};
    spectrum.weights = {0.30f, 0.25f, 0.20f, 0.15f, 0.08f, 0.02f};
    crate_loot_[1003] = spectrum;
    
    CrateLoot prisma;
    prisma.crate_def_index = 1004;
    prisma.item_defs = {1, 3, 6, 8, 9};
    prisma.weights = {0.35f, 0.30f, 0.20f, 0.12f, 0.03f};
    crate_loot_[1004] = prisma;
    
    CrateLoot glove;
    glove.crate_def_index = 1005;
    glove.item_defs = {2, 4, 7, 10};
    glove.weights = {0.40f, 0.30f, 0.20f, 0.10f};
    crate_loot_[1005] = glove;
    
    store_prices_[1] = 100;
    store_prices_[2] = 150;
    store_prices_[3] = 200;
    store_prices_[4] = 80;
    store_prices_[5] = 120;
    store_prices_[6] = 180;
    store_prices_[7] = 90;
    store_prices_[8] = 250;
    store_prices_[9] = 220;
    store_prices_[10] = 60;
    store_prices_[1001] = 50;
    store_prices_[1002] = 50;
    store_prices_[1003] = 50;
    store_prices_[1004] = 50;
    store_prices_[1005] = 50;
    store_prices_[2001] = 30;
    store_prices_[2002] = 30;
    store_prices_[2003] = 30;
    store_prices_[3001] = 150;
    store_prices_[3002] = 75;
    store_prices_[3003] = 200;
    
    loaded_ = true;
    printf("[GC] Schema loaded, items: %zu, crates: %zu\n", schema_.size(), crate_loot_.size());
}

ItemDef SchemaManager::GetItemDef(uint32_t def_index) {
    auto it = schema_.find(def_index);
    if (it != schema_.end()) {
        return it->second;
    }
    return ItemDef();
}

CrateLoot SchemaManager::GetCrateLoot(uint32_t crate_def_index) {
    auto it = crate_loot_.find(crate_def_index);
    if (it != crate_loot_.end()) {
        return it->second;
    }
    return CrateLoot();
}

uint32_t SchemaManager::GetStorePrice(uint32_t def_index) {
    auto it = store_prices_.find(def_index);
    if (it != store_prices_.end()) {
        return it->second;
    }
    return 0;
}

bool SchemaManager::IsValidItem(uint32_t def_index) {
    return schema_.find(def_index) != schema_.end();
}

bool SchemaManager::IsValidCrate(uint32_t def_index) {
    return crate_loot_.find(def_index) != crate_loot_.end();
}

bool SchemaManager::IsValidKey(uint32_t def_index) {
    return def_index >= 1001 && def_index <= 1005;
}

}