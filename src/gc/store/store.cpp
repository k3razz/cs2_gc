#include "store.h"
#include "../inventory/schema.h"
#include "../utils/logging.h"
#include <random>
#include <chrono>

namespace CS2GC {

StoreManager& StoreManager::Instance() {
    static StoreManager instance;
    return instance;
}

void StoreManager::Init(const std::string& config_path) {
    if (initialized_) return;
    
    config_path_ = config_path;
    currency_ = 999999;
    initialized_ = true;
    SchemaManager::Instance().LoadSchema();
    Log("Store initialized, currency: %u", currency_);
}

Item StoreManager::OpenCrate(uint32_t crate_def_index, uint32_t key_def_index) {
    if (!SchemaManager::Instance().IsValidCrate(crate_def_index)) {
        Log("Invalid crate: %u", crate_def_index);
        return Item();
    }
    
    if (!HasKey(key_def_index)) {
        Log("No key for crate: %u", crate_def_index);
        return Item();
    }
    
    ConsumeKey(key_def_index);
    
    CrateLoot loot = SchemaManager::Instance().GetCrateLoot(crate_def_index);
    if (loot.item_defs.empty()) {
        Log("Empty loot table for crate: %u", crate_def_index);
        return Item();
    }
    
    std::mt19937 gen(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    float roll = dis(gen);
    float cumulative = 0.0f;
    uint32_t selected_def = 0;
    
    for (size_t i = 0; i < loot.item_defs.size(); ++i) {
        cumulative += loot.weights[i];
        if (roll <= cumulative) {
            selected_def = loot.item_defs[i];
            break;
        }
    }
    
    if (selected_def == 0) {
        selected_def = loot.item_defs.back();
    }
    
    ItemDef item_def = SchemaManager::Instance().GetItemDef(selected_def);
    Item new_item;
    new_item.def_index = selected_def;
    new_item.paint_seed = gen() % 1000;
    new_item.wear = 0.01f + dis(gen) * 0.99f;
    new_item.stattrak_count = (dis(gen) < 0.10f) ? 0 : 0;
    new_item.equipped = false;
    
    Log("Crate %u opened, got item %u (paint_seed: %u, wear: %.3f)", 
        crate_def_index, selected_def, new_item.paint_seed, new_item.wear);
    
    return new_item;
}

Item StoreManager::PurchaseItem(uint32_t def_index) {
    if (!SchemaManager::Instance().IsValidItem(def_index)) {
        Log("Invalid item for purchase: %u", def_index);
        return Item();
    }
    
    uint32_t price = GetPrice(def_index);
    if (currency_ < price) {
        Log("Insufficient currency for item %u", def_index);
        return Item();
    }
    
    currency_ -= price;
    ItemDef item_def = SchemaManager::Instance().GetItemDef(def_index);
    Item new_item;
    new_item.def_index = def_index;
    new_item.paint_seed = 0;
    new_item.wear = item_def.base_wear;
    new_item.stattrak_count = 0;
    new_item.equipped = false;
    
    Log("Item %u purchased for %u currency", def_index, price);
    return new_item;
}

uint32_t StoreManager::GetPrice(uint32_t def_index) {
    return SchemaManager::Instance().GetStorePrice(def_index);
}

uint32_t StoreManager::GetCurrency() {
    return currency_;
}

void StoreManager::AddCurrency(uint32_t amount) {
    currency_ += amount;
    Log("Added %u currency, total: %u", amount, currency_);
}

bool StoreManager::HasKey(uint32_t key_def_index) {
    return true;
}

void StoreManager::ConsumeKey(uint32_t key_def_index) {
    Log("Key %u consumed", key_def_index);
}

}