#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace CS2GC {

struct GCItem {
    uint64_t item_id;
    uint32_t def_index;
    uint32_t paint_seed;
    float wear;
    uint32_t stattrak_count;
    std::vector<uint32_t> sticker_slots;
    bool equipped;
};

class IGameCoordinator {
public:
    virtual ~IGameCoordinator() = default;
    virtual void Initialize(const std::string& config_path) = 0;
    virtual void Shutdown() = 0;
    virtual void ProcessMessage(uint32_t msg_type, const void* data, uint32_t size) = 0;
    virtual std::vector<GCItem> GetInventory() = 0;
    virtual void AddItem(const GCItem& item) = 0;
    virtual void RemoveItem(uint64_t item_id) = 0;
    virtual void EquipItem(uint64_t item_id, bool equipped) = 0;
};

}
