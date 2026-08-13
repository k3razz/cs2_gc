#include "manager.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <cstdio>

namespace CS2GC {

InventoryManager& InventoryManager::Instance() {
    static InventoryManager instance;
    return instance;
}

void InventoryManager::Load(const std::string& config_path) {
    if (loaded_) return;
    
    inventory_path_ = config_path;
    std::ifstream config_file(config_path);
    if (!config_file.is_open()) {
        printf("[GC] Config file not found: %s\n", config_path.c_str());
        return;
    }
    
    nlohmann::json config;
    config_file >> config;
    
    if (config.contains("inventory")) {
        for (const auto& item_json : config["inventory"]) {
            GCItem item;
            item.item_id = item_json.value("id", 0);
            item.def_index = item_json.value("def_index", 0);
            item.paint_seed = item_json.value("paint_seed", 0);
            item.wear = item_json.value("wear", 0.0f);
            item.stattrak_count = item_json.value("stattrak", 0);
            item.equipped = item_json.value("equipped", false);
            
            if (item_json.contains("stickers")) {
                for (const auto& sticker : item_json["stickers"]) {
                    item.sticker_slots.push_back(sticker.get<uint32_t>());
                }
            }
            
            items_.push_back(item);
        }
    }
    
    version_ = 1;
    loaded_ = true;
    printf("[GC] Inventory loaded, items: %zu\n", items_.size());
}

void InventoryManager::Save() {
    nlohmann::json config;
    config["inventory"] = nlohmann::json::array();
    
    for (const auto& item : items_) {
        nlohmann::json item_json;
        item_json["id"] = item.item_id;
        item_json["def_index"] = item.def_index;
        item_json["paint_seed"] = item.paint_seed;
        item_json["wear"] = item.wear;
        item_json["stattrak"] = item.stattrak_count;
        item_json["equipped"] = item.equipped;
        
        if (!item.sticker_slots.empty()) {
            item_json["stickers"] = item.sticker_slots;
        }
        
        config["inventory"].push_back(item_json);
    }
    
    std::ofstream config_file(inventory_path_);
    if (config_file.is_open()) {
        config_file << config.dump(4);
        printf("[GC] Inventory saved, items: %zu\n", items_.size());
    }
    
    version_++;
}

std::vector<GCItem> InventoryManager::GetAllItems() {
    return items_;
}

void InventoryManager::AddItem(const GCItem& item) {
    items_.push_back(item);
    Save();
}

void InventoryManager::UpdateItem(const GCItem& item) {
    for (auto& existing : items_) {
        if (existing.item_id == item.item_id) {
            existing = item;
            Save();
            break;
        }
    }
}

void InventoryManager::RemoveItem(uint64_t item_id) {
    for (auto it = items_.begin(); it != items_.end(); ++it) {
        if (it->item_id == item_id) {
            items_.erase(it);
            Save();
            break;
        }
    }
}

GCItem InventoryManager::GetItem(uint64_t item_id) {
    for (const auto& item : items_) {
        if (item.item_id == item_id) {
            return item;
        }
    }
    return GCItem();
}

uint32_t InventoryManager::GetVersion() const {
    return version_;
}

}