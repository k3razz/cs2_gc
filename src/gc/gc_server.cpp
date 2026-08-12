#include "gc_server.h"
#include "protobufs/cs2_gc.pb.h"
#include "inventory/manager.h"
#include "store/store.h"
#include "utils/logging.h"
#include <random>
#include <chrono>

namespace CS2GC {

GCServer& GCServer::Instance() {
    static GCServer instance;
    return instance;
}

void GCServer::Init(const std::string& config_path) {
    if (initialized_) return;
    
    next_item_id_ = 1000;
    session_id_ = std::chrono::steady_clock::now().time_since_epoch().count();
    currency_ = 999999;
    inventory_version_ = 1;
    initialized_ = true;
    
    InventoryManager::Instance().Load(config_path);
    StoreManager::Instance().Init(config_path);
    
    handlers_[1] = std::bind(&GCServer::HandleHello, this, std::placeholders::_1);
    handlers_[2] = std::bind(&GCServer::HandleWelcome, this, std::placeholders::_1);
    handlers_[3] = std::bind(&GCServer::HandleInventoryRefresh, this, std::placeholders::_1);
    handlers_[4] = std::bind(&GCServer::HandleEquip, this, std::placeholders::_1);
    handlers_[5] = std::bind(&GCServer::HandleUnlockCrate, this, std::placeholders::_1);
    handlers_[6] = std::bind(&GCServer::HandleStorePurchase, this, std::placeholders::_1);
    handlers_[7] = std::bind(&GCServer::HandleApplySticker, this, std::placeholders::_1);
    handlers_[8] = std::bind(&GCServer::HandleRemoveSticker, this, std::placeholders::_1);
    handlers_[9] = std::bind(&GCServer::HandleNameTag, this, std::placeholders::_1);
    handlers_[10] = std::bind(&GCServer::HandleStatTrakSwap, this, std::placeholders::_1);
    handlers_[11] = std::bind(&GCServer::HandleGraffiti, this, std::placeholders::_1);
    handlers_[12] = std::bind(&GCServer::HandleMusicKit, this, std::placeholders::_1);
    handlers_[13] = std::bind(&GCServer::HandlePatchApply, this, std::placeholders::_1);
    handlers_[14] = std::bind(&GCServer::HandleSouvenir, this, std::placeholders::_1);
    
    Log("GCServer initialized");
}

void GCServer::Shutdown() {
    initialized_ = false;
    inventory_.clear();
    handlers_.clear();
    Log("GCServer shutdown");
}

void GCServer::ProcessMessage(uint32_t msg_type, const std::string& data) {
    auto it = handlers_.find(msg_type);
    if (it != handlers_.end()) {
        it->second(data);
    } else {
        Log("Unhandled message type: %u", msg_type);
    }
}

void GCServer::SendToClient(uint32_t msg_type, const std::string& data) {
    Log("Sending message type %u to client, size %zu", msg_type, data.size());
}

void GCServer::HandleHello(const std::string& data) {
    CMsgClientHello hello;
    if (!hello.ParseFromString(data)) {
        Log("Failed to parse CMsgClientHello");
        return;
    }
    
    CMsgClientWelcome welcome;
    welcome.set_server_version(1);
    welcome.set_session_id(session_id_);
    welcome.set_inventory_size(static_cast<uint32_t>(inventory_.size()));
    
    std::string response;
    welcome.SerializeToString(&response);
    SendToClient(2, response);
    Log("Client hello processed, steam_id: %llu", hello.steam_id());
}

void GCServer::HandleWelcome(const std::string& data) {
    Log("Welcome message received");
}

void GCServer::HandleInventoryRefresh(const std::string& data) {
    CMsgInventoryFullRefresh refresh;
    inventory_ = InventoryManager::Instance().GetAllItems();
    
    for (const auto& item : inventory_) {
        CMsgItem* proto_item = refresh.add_items();
        proto_item->set_item_id(item.item_id);
        proto_item->set_def_index(item.def_index);
        proto_item->set_paint_seed(item.paint_seed);
        proto_item->set_wear(item.wear);
        proto_item->set_stattrak_count(item.stattrak_count);
        for (uint32_t sticker : item.sticker_slots) {
            proto_item->add_sticker_slots(sticker);
        }
        proto_item->set_equipped(item.equipped);
    }
    
    std::string response;
    refresh.SerializeToString(&response);
    SendToClient(3, response);
    Log("Inventory refresh sent, items: %zu", inventory_.size());
}

void GCServer::HandleEquip(const std::string& data) {
    CMsgEquipItem equip;
    if (!equip.ParseFromString(data)) {
        Log("Failed to parse CMsgEquipItem");
        return;
    }
    
    for (auto& item : inventory_) {
        if (item.item_id == equip.item_id()) {
            item.equipped = equip.equipped();
            InventoryManager::Instance().UpdateItem(item);
            Log("Item %llu equipped: %d", equip.item_id(), equip.equipped());
            break;
        }
    }
}

void GCServer::HandleUnlockCrate(const std::string& data) {
    CMsgUnlockCrate req;
    if (!req.ParseFromString(data)) {
        Log("Failed to parse CMsgUnlockCrate");
        return;
    }
    
    Item new_item = StoreManager::Instance().OpenCrate(req.crate_def_index(), req.key_def_index());
    if (new_item.def_index != 0) {
        new_item.item_id = next_item_id_++;
        inventory_.push_back(new_item);
        InventoryManager::Instance().AddItem(new_item);
        
        CMsgUnlockCrateResponse resp;
        CMsgItem* proto_item = resp.mutable_item();
        proto_item->set_item_id(new_item.item_id);
        proto_item->set_def_index(new_item.def_index);
        proto_item->set_paint_seed(new_item.paint_seed);
        proto_item->set_wear(new_item.wear);
        proto_item->set_stattrak_count(new_item.stattrak_count);
        
        std::string response;
        resp.SerializeToString(&response);
        SendToClient(5, response);
        Log("Crate unlocked, item: %u", new_item.def_index);
    } else {
        Log("Failed to unlock crate");
    }
}

void GCServer::HandleStorePurchase(const std::string& data) {
    CMsgStorePurchase req;
    if (!req.ParseFromString(data)) {
        Log("Failed to parse CMsgStorePurchase");
        return;
    }
    
    uint32_t price = StoreManager::Instance().GetPrice(req.def_index());
    if (currency_ >= price) {
        currency_ -= price;
        Item new_item = StoreManager::Instance().PurchaseItem(req.def_index());
        if (new_item.def_index != 0) {
            new_item.item_id = next_item_id_++;
            inventory_.push_back(new_item);
            InventoryManager::Instance().AddItem(new_item);
            
            CMsgStorePurchaseResponse resp;
            resp.set_success(true);
            CMsgItem* proto_item = resp.mutable_item();
            proto_item->set_item_id(new_item.item_id);
            proto_item->set_def_index(new_item.def_index);
            proto_item->set_paint_seed(new_item.paint_seed);
            proto_item->set_wear(new_item.wear);
            
            std::string response;
            resp.SerializeToString(&response);
            SendToClient(6, response);
            Log("Store purchase success, item: %u", new_item.def_index);
        }
    } else {
        CMsgStorePurchaseResponse resp;
        resp.set_success(false);
        std::string response;
        resp.SerializeToString(&response);
        SendToClient(6, response);
        Log("Store purchase failed, insufficient currency");
    }
}

void GCServer::HandleApplySticker(const std::string& data) {
    CMsgApplySticker req;
    if (!req.ParseFromString(data)) {
        Log("Failed to parse CMsgApplySticker");
        return;
    }
    
    for (auto& item : inventory_) {
        if (item.item_id == req.item_id()) {
            if (req.sticker_slot() < item.sticker_slots.size()) {
                item.sticker_slots[req.sticker_slot()] = req.sticker_def_index();
                InventoryManager::Instance().UpdateItem(item);
                Log("Sticker applied to item %llu, slot %u", req.item_id(), req.sticker_slot());
            }
            break;
        }
    }
}

void GCServer::HandleRemoveSticker(const std::string& data) {
    CMsgRemoveSticker req;
    if (!req.ParseFromString(data)) {
        Log("Failed to parse CMsgRemoveSticker");
        return;
    }
    
    for (auto& item : inventory_) {
        if (item.item_id == req.item_id()) {
            if (req.sticker_slot() < item.sticker_slots.size()) {
                item.sticker_slots[req.sticker_slot()] = 0;
                InventoryManager::Instance().UpdateItem(item);
                Log("Sticker removed from item %llu, slot %u", req.item_id(), req.sticker_slot());
            }
            break;
        }
    }
}

void GCServer::HandleNameTag(const std::string& data) {
    CMsgNameTag req;
    if (!req.ParseFromString(data)) {
        Log("Failed to parse CMsgNameTag");
        return;
    }
    
    Log("Name tag applied to item %llu: %s", req.item_id(), req.name_tag().c_str());
}

void GCServer::HandleStatTrakSwap(const std::string& data) {
    CMsgStatTrakSwap req;
    if (!req.ParseFromString(data)) {
        Log("Failed to parse CMsgStatTrakSwap");
        return;
    }
    
    uint32_t source_count = 0;
    uint32_t target_count = 0;
    
    for (auto& item : inventory_) {
        if (item.item_id == req.source_item_id()) {
            source_count = item.stattrak_count;
        }
        if (item.item_id == req.target_item_id()) {
            target_count = item.stattrak_count;
        }
    }
    
    for (auto& item : inventory_) {
        if (item.item_id == req.source_item_id()) {
            item.stattrak_count = target_count;
            InventoryManager::Instance().UpdateItem(item);
        }
        if (item.item_id == req.target_item_id()) {
            item.stattrak_count = source_count;
            InventoryManager::Instance().UpdateItem(item);
        }
    }
    
    Log("StatTrak swap between %llu and %llu", req.source_item_id(), req.target_item_id());
}

void GCServer::HandleGraffiti(const std::string& data) {
    CMsgGraffiti req;
    if (!req.ParseFromString(data)) {
        Log("Failed to parse CMsgGraffiti");
        return;
    }
    
    Log("Graffiti applied: %u", req.graffiti_def_index());
}

void GCServer::HandleMusicKit(const std::string& data) {
    CMsgMusicKit req;
    if (!req.ParseFromString(data)) {
        Log("Failed to parse CMsgMusicKit");
        return;
    }
    
    Log("Music kit equipped: %u", req.music_kit_def_index());
}

void GCServer::HandlePatchApply(const std::string& data) {
    CMsgPatchApply req;
    if (!req.ParseFromString(data)) {
        Log("Failed to parse CMsgPatchApply");
        return;
    }
    
    Log("Patch applied: %u to item %llu", req.patch_def_index(), req.item_id());
}

void GCServer::HandleSouvenir(const std::string& data) {
    CMsgSouvenir req;
    if (!req.ParseFromString(data)) {
        Log("Failed to parse CMsgSouvenir");
        return;
    }
    
    Log("Souvenir applied: %u", req.souvenir_def_index());
}

}