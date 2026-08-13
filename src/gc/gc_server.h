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
    
    void Initialize(const std::string& config_path) override;
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
    void HandleClientHello(const std::string& data);
    void HandleSetItemPosition(const std::string& data);
    void HandleDeleteItem(const std::string& data);
    void HandleNameItem(const std::string& data);
    void HandleOpenCrate(const std::string& data);
    void HandleAdjustEquipSlots(const std::string& data);
    
    void SendToClient(uint32_t msg_type, const std::string& data);
    void LoadInventory(const std::string& filepath);
    void SaveInventory(const std::string& filepath);
    
    std::unordered_map<uint32_t, std::function<void(const std::string&)>> handlers_;
    std::vector<GCItem> inventory_;
    std::string inventory_path_;
    uint64_t next_item_id_;
    uint64_t session_id_;
    uint32_t currency_;
    uint32_t inventory_version_;
    bool initialized_;
};

}
