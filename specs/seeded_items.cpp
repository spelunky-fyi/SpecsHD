
#include "seeded_items.h"

#include <format>
#include <string>

#include "drawing.h"
#include <hddll/entities.h>
#include <hddll/utils.h>

uint32_t getKaliRewardForSeed(uint32_t seed) {
  hddll::mersenne_init_and_twist(seed);
  auto val = (hddll::mersenne_random() & 7) + 1;

  switch (val) {
  case 1:
    return 521; // Cape
  case 2:
    return 504; // Climbers
  case 3:
    return 503; // Specs
  case 4:
    return 505; // Pitchers
  case 5:
    return 506; // Spring Shoes
  case 6:
    return 507; // Spike Shoes
  case 7:
    return 508; // Paste
  case 8:
    return 509; // Compass
  }

  // should never happen
  return 0;
}

uint32_t getPotItemForSeed(uint32_t seed, bool sfx, bool rubble) {
  hddll::mersenne_init_and_twist(seed);

  // SFX
  if (sfx)
    hddll::mersenne_random();

  // Rubble
  if (rubble) {
    hddll::mersenne_random();
    hddll::mersenne_random();
    hddll::mersenne_random();
    hddll::mersenne_random();
    hddll::mersenne_random();

    hddll::mersenne_random();
    hddll::mersenne_random();
    hddll::mersenne_random();
    hddll::mersenne_random();
    hddll::mersenne_random();

    hddll::mersenne_random();
    hddll::mersenne_random();
    hddll::mersenne_random();
    hddll::mersenne_random();
    hddll::mersenne_random();
  }

  if (hddll::mersenne_random() % 5000 == 0)
    return 1026; // Alien

  if (hddll::mersenne_random() % 128 == 0)
    return 1029; // Scorpion

  if (hddll::mersenne_random() % 64 == 0)
    return 1036; // Cobra

  if (hddll::mersenne_random() % 12 == 0)
    return 1001; // Snake

  if (hddll::mersenne_random() % 8 == 0)
    return 1002; // Spider

  if (hddll::mersenne_random() % 16 == 0)
    return 106; // Large Ruby

  if (hddll::mersenne_random() % 12 == 0)
    return 105; // Large Saphire

  if (hddll::mersenne_random() % 8 == 0)
    return 104; // Large Emerald

  if (hddll::mersenne_random() % 8 == 0)
    return 118; // Large Gold Nugget

  if (hddll::mersenne_random() % 4 == 0)
    return 124; // Small Gold Nugget

  return 0;
}

std::vector<uint32_t> getChestItemsForSeed(uint32_t seed, bool vault) {

  std::vector<uint32_t> items = {};
  hddll::mersenne_init_and_twist(seed);

  if (vault) {
    auto num_items = (hddll::mersenne_random() % 3) + 6;
    for (unsigned int i = 0; i < num_items; i++) {

      if (hddll::mersenne_random() % 0x18 == 0) {
        items.push_back(173); // Diamond
      } else if (hddll::mersenne_random() % 6 == 0) {
        items.push_back(106); // Large Ruby
      } else if (hddll::mersenne_random() % 3 == 0) {
        items.push_back(105); // Large Saph
      } else {
        items.push_back(104); // Large Emerald
      }

      // Random velocity
      hddll::mersenne_random();
      hddll::mersenne_random();
    }
  } else {
    auto num_items = (hddll::mersenne_random() & 1) + 3;

    for (unsigned int i = 0; i < num_items; i++) {

      if (hddll::mersenne_random() % 0x50 == 0) {
        items.push_back(107); // Live Bomb
      } else if (hddll::mersenne_random() % 0x28 == 0) {
        items.push_back(106); // Large Ruby
      } else if (hddll::mersenne_random() % 0x14 == 0) {
        items.push_back(105); // Large Saph
      } else if (hddll::mersenne_random() % 10 == 0) {
        items.push_back(104); // Large Emerald
      } else if (hddll::mersenne_random() % 6 == 0) {
        items.push_back(127); // Small Ruby
      } else if (hddll::mersenne_random() % 3 == 0) {
        items.push_back(126); // Small Saph
      } else {
        items.push_back(125); // Small Emerald
      }
      // Random velocity
      hddll::mersenne_random();
      hddll::mersenne_random();
    }
  }

  return items;
}

uint32_t getCrateItemForSeed(uint32_t seed) {
  hddll::mersenne_init_and_twist(seed);

  if (hddll::mersenne_random() % 10000 == 0)
    return 517; // Plasma Cannon

  if (hddll::mersenne_random() % 500 == 0)
    return 522; // Jetpack

  if (hddll::mersenne_random() % 200 == 0)
    return 516; // Freeze Ray

  if (hddll::mersenne_random() % 200 == 0)
    return 521; // Cape

  if (hddll::mersenne_random() % 100 == 0)
    return 515; // Shotgun

  if (hddll::mersenne_random() % 100 == 0)
    return 510; // MATTOCK

  if (hddll::mersenne_random() % 100 == 0)
    return 519; // TELEPORTER

  if (hddll::mersenne_random() % 0x5a == 0)
    return 504; // CLIMBING_GLOVES

  if (hddll::mersenne_random() % 0x5a == 0)
    return 503; // SPECTACLE

  if (hddll::mersenne_random() % 0x50 == 0)
    return 514; // WEB_GUN

  if (hddll::mersenne_random() % 0x50 == 0)
    return 518; // CAMERA

  if (hddll::mersenne_random() % 0x50 == 0)
    return 505; // PITCHERS_MITT

  if (hddll::mersenne_random() % 0x3c == 0)
    return 508; // BOMB_PASTE

  if (hddll::mersenne_random() % 0x3c == 0)
    return 506; // SPRING_SHOES

  if (hddll::mersenne_random() % 0x3c == 0)
    return 507; // SPIKE_SHOES

  if (hddll::mersenne_random() % 0x3c == 0)
    return 511; // BOOMERANG

  if (hddll::mersenne_random() % 0x28 == 0)
    return 512; // MACHETE

  if (hddll::mersenne_random() % 0x28 == 0)
    return 502; // BOMB_BOX

  if (hddll::mersenne_random() % 0x14 == 0)
    return 509; // COMPASS

  if (hddll::mersenne_random() % 10 == 0)
    return 520; // PARACHUTE

  if ((hddll::mersenne_random() & 1) == 0)
    return 500; // Ropes

  return 501; // Bomb Bag
}

void drawSeededCrateOverlay() {
  for (size_t idx = 0;
       idx < hddll::gGlobalState->entities->entities_active_count; idx++) {
    auto ent = hddll::gGlobalState->entities->entities_active[idx];
    if (!ent) {
      continue;
    }

    if (ent->entity_type != 101) {
      continue;
    }

    auto screen = gameToScreen({ent->x, ent->y});
    auto entity_type = getCrateItemForSeed(ent->z_depth_as_int);
    gOverlayDrawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() + 2.f,
                              ImVec2{screen.x, screen.y}, IM_COL32_WHITE,
                              hddll::EntityTypeName(entity_type));
  }
}

void drawSeededPotOverlay() {
  for (size_t idx = 0;
       idx < hddll::gGlobalState->entities->entities_active_count; idx++) {
    auto ent = hddll::gGlobalState->entities->entities_active[idx];
    if (!ent) {
      continue;
    }

    if (ent->entity_type != 113) {
      continue;
    }

    auto screen = gameToScreen({ent->x - ent->hitbox_x, ent->y});
    auto fontSize = ImGui::GetFontSize() + 2.f;

    uint32_t entity_type = 0;
    entity_type = getPotItemForSeed(ent->z_depth_as_int);
    if (entity_type > 0) {
      gOverlayDrawList->AddText(ImGui::GetFont(), fontSize,
                                ImVec2{screen.x, screen.y - (fontSize + 2.f)},
                                IM_COL32_WHITE,
                                hddll::EntityTypeName(entity_type));
    }

    entity_type = getPotItemForSeed(ent->z_depth_as_int, false, true);
    if (entity_type > 0) {
      gOverlayDrawList->AddText(
          ImGui::GetFont(), fontSize, ImVec2{screen.x, screen.y},
          IM_COL32_WHITE,
          std::format("Dupe: {}", hddll::EntityTypeName(entity_type)).c_str());
    }
  }
}

void drawSeededChestOverlay() {
  for (size_t idx = 0;
       idx < hddll::gGlobalState->entities->entities_active_count; idx++) {
    hddll::EntityActive *ent =
        (hddll::EntityActive *)
            hddll::gGlobalState->entities->entities_active[idx];
    if (!ent) {
      continue;
    }

    if (ent->entity_type != 100) {
      continue;
    }

    auto screen = gameToScreen({ent->x - ent->hitbox_x, ent->y});
    auto fontSize = ImGui::GetFontSize() + 2.f;

    auto entity_types =
        getChestItemsForSeed(ent->z_depth_as_int, ent->field5_0x140);
    auto entity_names = std::vector<std::string>{};
    for (auto entity_type : entity_types) {
      entity_names.push_back(hddll::EntityTypeName(entity_type));
    }
    auto out = hddll::join(entity_names, ", ");
    gOverlayDrawList->AddText(ImGui::GetFont(), fontSize,
                              ImVec2{screen.x, screen.y - (fontSize + 2.f)},
                              IM_COL32_WHITE, out.c_str());
  }
}

void drawSeededKaliRewardsOverlay() {
  for (auto idx = 0; idx < hddll::ENTITY_FLOORS_COUNT; idx++) {
    auto ent = hddll::gGlobalState->level_state->entity_floors[idx];
    if (!ent) {
      continue;
    }

    if (ent->entity_type != 35) {
      continue;
    }

    auto screen = gameToScreen({ent->x - (ent->hitbox_x / 2), ent->y});
    auto entity_type = getKaliRewardForSeed(ent->z_depth_as_int);
    gOverlayDrawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() + 2.f,
                              ImVec2{screen.x, screen.y}, IM_COL32_WHITE,
                              hddll::EntityTypeName(entity_type));
  }
}
