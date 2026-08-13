#include "gc_server.h"
#include "inventory/manager.h"
#include "store/store.h"
#include <chrono>
#include <cstdio>
#include <cstdarg>
#include <fstream>
#include <nlohmann/json.hpp>

namespace CS2GC {

void Log(const char* format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    printf("[GC] %s\n", buffer);
    fflush(stdout);
}

GCServer& GCServer::Instance() {
    static GCServer instance;
    return instance;
}

void GCServer::Initialize(const std::string& config_path) {
    if (initialized_) return;
    
    next_item_id_ = 1000;
    session_id_ = std::chrono::steady_clock::now().time_since_epoch().count();
    currency_ = 999999;
    inventory_version_ = 1;
    initialized_ = true;
    inventory_path_ = config_path;
    
    LoadInventory(config_path);
    InventoryManager::Instance().Load(config_path);
    StoreManager::Instance().Init(config_path);
    
    Log("GCServer initialized for CS2");
}

void GCServer::Shutdown() {
    initialized_ = false;
    inventory_.clear();
    handlers_.clear();
    Log("GCServer shutdown");
}

void GCServer::ProcessMessage(uint32_t msg_type, const void* data, uint32_t size) {
    Log("Message received: type %u, size %u", msg_type, size);
}

void GCServer::SendToClient(uint32_t msg_type, const std::string& data) {
    Log("Sending message type %u to client, size %zu", msg_type, data.size());
}

std::vector<GCItem> GCServer::GetInventory() {
    return inventory_;
}

void GCServer::AddItem(const GCItem& item) {
    inventory_.push_back(item);
    InventoryManager::Instance().AddItem(item);
}

void GCServer::RemoveItem(uint64_t item_id) {
    for (auto it = inventory_.begin(); it != inventory_.end(); ++it) {
        if (it->item_id == item_id) {
            inventory_.erase(it);
            InventoryManager::Instance().RemoveItem(item_id);
            break;
        }
    }
}

void GCServer::EquipItem(uint64_t item_id, bool equipped) {
    for (auto& item : inventory_) {
        if (item.item_id == item_id) {
            item.equipped = equipped;
            InventoryManager::Instance().UpdateItem(item);
            break;
        }
    }
}

void GCServer::LoadInventory(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        Log("Inventory file not found, starting with empty inventory");
        return;
    }
    
    nlohmann::json config;
    file >> config;
    file.close();
    
    if (config.contains("inventory")) {
        for (const auto& item_json : config["inventory"]) {
            GCItem item;
            item.item_id = item_json.value("id", next_item_id_++);
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
            inventory_.push_back(item);
        }
    }
    
    if (config.contains("currency")) {
        currency_ = config["currency"].get<uint32_t>();
    }
    
    Log("Inventory loaded: %zu items, currency: %u", inventory_.size(), currency_);
}

void GCServer::SaveInventory(const std::string& filepath) {
    nlohmann::json config;
    config["inventory"] = nlohmann::json::array();
    config["currency"] = currency_;
    
    for (const auto& item : inventory_) {
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
    
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << config.dump(4);
        Log("Inventory saved: %zu items", inventory_.size());
    } else {
        Log("Failed to save inventory");
    }
}

}
