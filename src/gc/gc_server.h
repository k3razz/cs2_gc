#pragma once
#include <cstdint>
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include "../../include/gc_api.h"

namespace CS2GC {

class GCServer : public IGameCoordinator {
public:
    static GCServer& Instance();
    
    void Init(const std::string& config_path);
    void Initialize(const std::string& config_path) override { Init(config_path); }
    void Shutdown() override;
    void ProcessMessage(uint32_t msg_type, const void* data, uint32_t size) override;
    std::vector<GCItem> GetInventory() override;
    void AddItem(const GCItem& item) override;
    void RemoveItem(uint64_t item_id) override;
    void EquipItem(uint64_t item_id, bool equipped) override;
    
private:
    GCServer() = default;
    ~GCServer() = default;
    GCServer(const GCServer&) = delete;
    GCServer& operator=(const GCServer&) = delete;
    
    void HandleHello(const std::string& data);
    void HandleWelcome(const std::string& data);
    void HandleInventoryRefresh(const std::string& data);
    void HandleEquip(const std::string& data);
    void HandleUnlockCrate(const std::string& data);
    void HandleStorePurchase(const std::string& data);
    void HandleApplySticker(const std::string& data);
    void HandleRemoveSticker(const std::string& data);
    void HandleNameTag(const std::string& data);
    void HandleStatTrakSwap(const std::string& data);
    void HandleGraffiti(const std::string& data);
    void HandleMusicKit(const std::string& data);
    void HandlePatchApply(const std::string& data);
    void HandleSouvenir(const std::string& data);
    
    void SendToClient(uint32_t msg_type, const std::string& data);
    
    std::unordered_map<uint32_t, std::function<void(const std::string&)>> handlers_;
    std::vector<GCItem> inventory_;
    uint64_t next_item_id_;
    uint64_t session_id_;
    uint32_t currency_;
    uint32_t inventory_version_;
    bool initialized_;
};

}