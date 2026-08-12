#pragma once
#include <string>
#include <vector>

namespace CS2GC {

std::vector<uint8_t> GenerateSignature(const std::string& data);
bool VerifySignature(const std::string& data, const std::vector<uint8_t>& signature);
uint64_t GenerateSessionID();
uint32_t GenerateRandomSeed();

}