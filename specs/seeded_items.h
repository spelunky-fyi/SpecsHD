#pragma once

#include <cstdint>
#include <vector>

// Seeded item RNG simulation
uint32_t getKaliRewardForSeed(uint32_t seed);
uint32_t getPotItemForSeed(uint32_t seed, bool sfx = true, bool rubble = true);
std::vector<uint32_t> getChestItemsForSeed(uint32_t seed, bool vault = false);
uint32_t getCrateItemForSeed(uint32_t seed);

// Seeded overlays
void drawSeededCrateOverlay();
void drawSeededPotOverlay();
void drawSeededChestOverlay();
void drawSeededKaliRewardsOverlay();
