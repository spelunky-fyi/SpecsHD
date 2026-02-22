
#include "level_tab.h"

#include <fstream>

#include "../drawing.h"
#include "../game_hooks.h"
#include "../seeded_items.h"
#include "../state.h"

LevelsState gLevelsState = {};

void warpToLevel(uint32_t level) {
  resetForLevel(gGlobalState);
  gGlobalState->level = level;
  gGlobalState->screen_state = 1;
}

void resetRun() {
  resetForRun(gGlobalState, 0);
  gGlobalState->level = gGlobalState->respawn_level;
  gGlobalState->screen_state = 1;
}

static void searchStraightDown() {
  auto entranceRoomNumber = gGlobalState->level_state->entrance_room_x +
                            gGlobalState->level_state->entrance_room_y * 4;

  auto roomType = gGlobalState->level_state->room_types[entranceRoomNumber];
  auto roomType2 =
      gGlobalState->level_state->room_types[entranceRoomNumber + 4];
  auto roomType3 =
      gGlobalState->level_state->room_types[entranceRoomNumber + 8];
  auto exitRoomNumber = gGlobalState->level_state->exit_room_x +
                        gGlobalState->level_state->exit_room_y * 4;

  if (roomType != 2 || roomType2 != 2 || roomType3 != 2 ||
      entranceRoomNumber + 12 != exitRoomNumber) {
    resetRun();
  }
}

static void searchNoEggy13() {
  auto hasBox = false;
  auto hasIdol = false;
  auto hasAltar = false;

  auto boxPos = 0;
  auto idolPos = 0;
  auto altarPos = 0;

  for (size_t idx = 0; idx < gGlobalState->entities->entities_active_count;
       idx++) {
    auto ent = gGlobalState->entities->entities_active[idx];
    if (!ent) {
      continue;
    }

    if (ent->entity_kind == EntityKind::KIND_ITEM) {
      auto entity_item = (EntityItem *)ent;
      if (entity_item->field52_0x1f0 == 1 and entity_item->entity_type == 248) {
        hasBox = true;
        boxPos = GetRoomForPosition(ent->x, ent->y) / 4;
      }
    }
  }

  for (auto idx = 0; idx < 4692; idx++) {
    auto ent = gGlobalState->level_state->entity_floors[idx];
    if (!ent) {
      continue;
    }
    if (ent->entity_type == 35) {
      hasAltar = true;
      altarPos = GetRoomForPosition(ent->x, ent->y) / 4;
    }
    if (ent->entity_type == 15) {
      hasIdol = true;
      idolPos = GetRoomForPosition(ent->x, ent->y) / 4;
    }
  }

  if (hasBox && hasAltar && hasIdol && boxPos <= idolPos &&
      idolPos == altarPos) {
    std::ofstream file;
    file.open("1-3-seeds.txt", std::ios_base::app);
    file << lastSeed << std::endl;
    resetRun();
  } else {
    resetRun();
  }
}

static int getFirstTileForRoomIdx(int roomIdx) {
  const int startIdx = 141;
  const int roomHeight = 8;
  const int roomWidth = 10;
  const int rowWidth = 46;

  auto roomY = roomIdx / 4;
  auto roomX = roomIdx % 4;

  return startIdx + (roomY * roomHeight * rowWidth) + (roomX * roomWidth);
}

static int getTileByCoord(int x, int y) {
  const int startIdx = 141;
  const int rowWidth = 46;

  return 4692 - (y * rowWidth) + x;
}

static void searchDaR() {
  auto spawnedBM = gGlobalState->spawned_black_market_entrance;

  auto hasHC = false;
  auto hcRoom = 0;

  for (auto idx = 0; idx < 48; idx++) {
    auto room_type = gGlobalState->level_state->room_types[idx];
    if (room_type == 47) {
      hcRoom = idx;
      hasHC = true;
      break;
    }
  }

  if (!hasHC || spawnedBM) {
    resetRun();
  }

  auto roomBelowHC = hcRoom + 4;
  auto tileStart = getFirstTileForRoomIdx(roomBelowHC);

  for (auto idx = 0; idx < 10; idx++) {
    auto tile = gGlobalState->level_state->entity_floors[tileStart + idx];
    if (!tile || tile->entity_type == 4) {
      continue;
    } else {
      resetRun();
      return;
    }
  }

  for (auto idx = 0; idx < 10; idx++) {
    auto tile = gGlobalState->level_state->entity_floors[tileStart + 46 + idx];
    if (!tile) {
      resetRun();
      return;
    }
    if (tile->entity_type == 9097 || tile->entity_type == 4) {
      continue;
    } else {
      resetRun();
      return;
    }
  }

  std::ofstream file;
  file.open("haunted-castle-black-market-seeds.txt", std::ios_base::app);
  file << lastSeed << std::endl;
  resetRun();
}

static int hasBombChest() {
  auto bombCount = 0;
  for (size_t idx = 0; idx < gGlobalState->entities->entities_active_count;
       idx++) {
    auto ent = gGlobalState->entities->entities_active[idx];
    if (!ent) {
      continue;
    }

    if (ent->entity_type == 100) {
      auto entity_types = getChestItemsForSeed(ent->z_depth_as_int);
      bombCount += std::count(entity_types.begin(), entity_types.end(), 107);
    }
  }

  return bombCount;
}

static void searchHC() {
  auto spawnedBM = gGlobalState->spawned_black_market_entrance;
  if (!spawnedBM) {
    resetRun();
    return;
  }

  auto bmX = gGlobalState->level_state->alt_exit_x;
  auto bmY = gGlobalState->level_state->alt_exit_y;

  auto bmXGood = bmX >= 30.0 && bmX <= 31.0;
  auto bmYGood = bmY == 69.0;

  if (!bmXGood || !bmYGood) {
    resetRun();
    return;
  }

  std::ofstream file;
  file.open("haunted-castle-seeds.txt", std::ios_base::app);
  file << lastSeed << std::endl;
  resetRun();
}

static void searchWorm() {

  auto wormEggCount = 0;
  auto bacteriumCount = 0;
  auto ufoCount = 0;
  auto yetiCount = 0;
  auto skeletonCount = 0;

  for (size_t idx = 0; idx < gGlobalState->entities->entities_active_count;
       idx++) {
    auto ent = gGlobalState->entities->entities_active[idx];
    if (!ent) {
      continue;
    }

    if (ent->entity_type == 1046) {
      wormEggCount++;
    } else if (ent->entity_type == 1035) {
      bacteriumCount++;
    } else if (ent->entity_type == 1010) {
      ufoCount++;
    } else if (ent->entity_type == 1009) {
      yetiCount++;
    } else if (ent->entity_type == 1012) {
      skeletonCount++;
    }
  }

  std::ofstream file;
  file.open("worms.txt", std::ios_base::app);
  file << lastSeed << "," << wormEggCount << "," << bacteriumCount << ","
       << ufoCount << "," << yetiCount << "," << skeletonCount << std::endl;
  resetRun();
}

static void search34() {
  if (gGlobalState->level == 9) {
    gGlobalState->is_worm = 1;
    warpToLevel(gGlobalState->level);
    return;
  } else if (gGlobalState->level == 10) {
    warpToLevel(gGlobalState->level);
    return;
  }

  auto rightMoship =
      gGlobalState->level_state->entity_floors[getTileByCoord(33, 96)];
  auto leftMoship =
      gGlobalState->level_state->entity_floors[getTileByCoord(12, 96)];

  if (rightMoship && rightMoship->entity_type == 81) {
    auto block =
        gGlobalState->level_state->entity_floors[getTileByCoord(32, 97)];
    auto blockAbove =
        gGlobalState->level_state->entity_floors[getTileByCoord(32, 98)];
    if (!block || block->entity_type != 9098 || blockAbove) {
      resetRun();
      return;
    }
  } else if (leftMoship && leftMoship->entity_type == 81) {
    auto block =
        gGlobalState->level_state->entity_floors[getTileByCoord(13, 97)];
    auto blockAbove =
        gGlobalState->level_state->entity_floors[getTileByCoord(13, 98)];
    if (!block || block->entity_type != 9098 || blockAbove) {
      resetRun();
      return;
    }
  } else {
    resetRun();
    return;
  }

  std::ofstream file;
  file.open("34-seeds.txt", std::ios_base::app);
  file << lastSeed << std::endl;
  resetRun();
}

static void RectFilled(ImVec2 &size, ImU32 col = IM_COL32_WHITE,
                       float rounding = 0.f, ImDrawFlags flags = 0) {

  ImGui::Dummy(size);

  if (!ImGui::IsItemVisible()) {
    return;
  }

  auto p0 = ImGui::GetItemRectMin();
  auto p1 = ImGui::GetItemRectMax();
  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  draw_list->PushClipRect(p0, p1, true);
  draw_list->AddRectFilled(p0, p1, col, rounding, flags);
  draw_list->PopClipRect();
}

static void Rect(const char *label, ImVec2 &size, ImU32 col = IM_COL32_WHITE,
                 float rounding = 0.f, ImDrawFlags flags = 0,
                 float thickness = 1.0f) {

  ImGui::Dummy(size);

  if (!ImGui::IsItemVisible()) {
    return;
  }

  auto p0 = ImGui::GetItemRectMin();
  auto p1 = ImGui::GetItemRectMax();
  auto textSize = ImGui::CalcTextSize(label);
  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  draw_list->PushClipRect(p0, p1, true);
  draw_list->AddRect(p0, p1, col, rounding, flags, thickness);
  draw_list->AddText({p0.x + ((p1.x - p0.x) / 2) - (textSize.x / 2),
                      p0.y + ((p1.y - p0.y) / 2) - (textSize.y / 2)},
                     col, label);
  draw_list->PopClipRect();
}

static void drawLockedLevelFlag(std::string title, uint8_t &val,
                                LockableU8 *lockable) {
  ImGuiIO &io = ImGui::GetIO();

  if (ImGui::Checkbox(std::format("##LevelLock{}", title).c_str(),
                      &lockable->IsLocked)) {
    if (lockable->IsLocked) {
      lockable->LockedValue = val;
    }
  }
  ImGui::SameLine(80.0f * io.FontGlobalScale);
  if (drawCharBool(title.c_str(), val)) {
    lockable->LockedValue = val;
  }
}

static void ensureLockedU8(uint8_t &val, LockableU8 *lockable) {
  if (lockable->IsLocked) {
    val = lockable->LockedValue;
  }
}

void ensureLockedLevelsState() {
  ensureLockedU8(gGlobalState->flooded_mines, &gLevelsState.FloodedMines);
  ensureLockedU8(gGlobalState->skin_is_crawling, &gLevelsState.SkinIsCrawling);

  ensureLockedU8(gGlobalState->dead_are_restless,
                 &gLevelsState.DeadAreRestless);
  ensureLockedU8(gGlobalState->rushing_water, &gLevelsState.RushingWater);
  ensureLockedU8(gGlobalState->is_haunted_castle, &gLevelsState.HauntedCastle);
  ensureLockedU8(gGlobalState->tiki_village, &gLevelsState.TikiVillage);
  ensureLockedU8(gGlobalState->is_blackmarket, &gLevelsState.BlackMarket);

  ensureLockedU8(gGlobalState->is_wet_fur, &gLevelsState.WetFur);
  ensureLockedU8(gGlobalState->is_mothership, &gLevelsState.MotherShip);
  ensureLockedU8(gGlobalState->is_worm, &gLevelsState.Worm);

  ensureLockedU8(gGlobalState->is_city_of_gold, &gLevelsState.CityOfGold);

  ensureLockedU8(gGlobalState->altar_spawned, &gLevelsState.AltarSpawned);
  ensureLockedU8(gGlobalState->idol_spawned, &gLevelsState.IdolSpawned);
  ensureLockedU8(gGlobalState->damsel_spawned, &gLevelsState.DamselSpawned);
  ensureLockedU8(gGlobalState->ghost_spawned, &gLevelsState.GhostSpawned);
  ensureLockedU8(gGlobalState->vault_spawned_in_area,
                 &gLevelsState.VaultSpawnedInArea);
}

void drawLevelTab() {

  ImGuiIO &io = ImGui::GetIO();
  auto isDisabled =
      gGlobalState->screen_state != 0 || gGlobalState->play_state != 0;

  if (isDisabled) {
    ImGui::BeginDisabled();
  }
  ImGui::Text("");
  ImGui::SameLine(100.0f * io.FontGlobalScale);
  if (ImGui::Button("Next Level")) {
    warpToLevel(gGlobalState->level);
  }

  ImGui::Text("Mines");
  ImGui::SameLine(100.0f * io.FontGlobalScale);
  if (ImGui::Button("1-1"))
    warpToLevel(0);
  ImGui::SameLine();
  if (ImGui::Button("1-2"))
    warpToLevel(1);
  ImGui::SameLine();
  if (ImGui::Button("1-3")) {
    warpToLevel(2);
  }
  ImGui::SameLine();
  if (ImGui::Button("1-4")) {
    warpToLevel(3);
  }

  ImGui::Text("Jungle");
  ImGui::SameLine(100.0f * io.FontGlobalScale);
  if (ImGui::Button("2-1"))
    warpToLevel(4);
  ImGui::SameLine();
  if (ImGui::Button("2-2"))
    warpToLevel(5);
  ImGui::SameLine();
  if (ImGui::Button("2-3")) {
    warpToLevel(6);
  }
  ImGui::SameLine();
  if (ImGui::Button("2-4")) {
    warpToLevel(7);
  }

  ImGui::Text("Ice Caves");
  ImGui::SameLine(100.0f * io.FontGlobalScale);
  if (ImGui::Button("3-1"))
    warpToLevel(8);
  ImGui::SameLine();
  if (ImGui::Button("3-2"))
    warpToLevel(9);
  ImGui::SameLine();
  if (ImGui::Button("3-3")) {
    warpToLevel(10);
  }
  ImGui::SameLine();
  if (ImGui::Button("3-4")) {
    warpToLevel(11);
  }

  ImGui::Text("Temple");
  ImGui::SameLine(100.0f * io.FontGlobalScale);
  if (ImGui::Button("4-1"))
    warpToLevel(12);
  ImGui::SameLine();
  if (ImGui::Button("4-2"))
    warpToLevel(13);
  ImGui::SameLine();
  if (ImGui::Button("4-3")) {
    warpToLevel(14);
  }
  ImGui::SameLine();
  if (ImGui::Button("Olmec")) {
    warpToLevel(15);
  }

  ImGui::Text("Hell");
  ImGui::SameLine(100.0f * io.FontGlobalScale);
  if (ImGui::Button("5-1"))
    warpToLevel(16);
  ImGui::SameLine();
  if (ImGui::Button("5-2"))
    warpToLevel(17);
  ImGui::SameLine();
  if (ImGui::Button("5-3")) {
    warpToLevel(18);
  }
  ImGui::SameLine();
  if (ImGui::Button("Yama")) {
    warpToLevel(19);
  }
  if (ImGui::Button("Reset Run")) {
    resetRun();
  }
  if (isDisabled) {
    ImGui::EndDisabled();
  }

  ImGui::Separator();
  ImGui::PushItemWidth(100 * io.FontGlobalScale);
  ImGui::InputInt("Respawn Level Skip", (int *)&gGlobalState->respawn_level);
  ImGui::SameLine();
  if (ImGui::Button("Set Current Level")) {
    gGlobalState->respawn_level =
        std::clamp((int)gGlobalState->level - 1, 0, (int)gGlobalState->level);
  }

  if (ImGui::CollapsingHeader("Level Flags")) {
    ImGui::Text("Locked?");
    ImGui::SameLine(80.0f * io.FontGlobalScale);
    ImGui::Text("Value");
    ImGui::Separator();

    ImGui::Separator();

    drawLockedLevelFlag("Flooded Mines", gGlobalState->flooded_mines,
                        &gLevelsState.FloodedMines);
    drawLockedLevelFlag("Skin is Crawling", gGlobalState->skin_is_crawling,
                        &gLevelsState.SkinIsCrawling);

    ImGui::Separator();
    drawLockedLevelFlag("Dead are Restless", gGlobalState->dead_are_restless,
                        &gLevelsState.DeadAreRestless);
    drawLockedLevelFlag("Rushing Water", gGlobalState->rushing_water,
                        &gLevelsState.RushingWater);
    drawLockedLevelFlag("Haunted Castle", gGlobalState->is_haunted_castle,
                        &gLevelsState.HauntedCastle);
    drawLockedLevelFlag("Tiki Village", gGlobalState->tiki_village,
                        &gLevelsState.TikiVillage);
    drawLockedLevelFlag("Black Market", gGlobalState->is_blackmarket,
                        &gLevelsState.BlackMarket);

    ImGui::Separator();
    drawLockedLevelFlag("Wet Fur", gGlobalState->is_wet_fur,
                        &gLevelsState.WetFur);
    drawLockedLevelFlag("Mothership", gGlobalState->is_mothership,
                        &gLevelsState.MotherShip);
    drawLockedLevelFlag("Worm", gGlobalState->is_worm, &gLevelsState.Worm);

    ImGui::Separator();
    drawLockedLevelFlag("City of Gold", gGlobalState->is_city_of_gold,
                        &gLevelsState.CityOfGold);

    ImGui::Separator();
    drawLockedLevelFlag("Altar Spawned", gGlobalState->altar_spawned,
                        &gLevelsState.AltarSpawned);
    drawLockedLevelFlag("Idol Spawned", gGlobalState->idol_spawned,
                        &gLevelsState.IdolSpawned);
    drawLockedLevelFlag("Damsel Spawned", gGlobalState->damsel_spawned,
                        &gLevelsState.DamselSpawned);
    drawLockedLevelFlag("Ghost Spawned", gGlobalState->ghost_spawned,
                        &gLevelsState.GhostSpawned);
    drawLockedLevelFlag("Vault Spawned in Area",
                        gGlobalState->vault_spawned_in_area,
                        &gLevelsState.VaultSpawnedInArea);
  }

  ImVec2 size = {5.f, 5.f};
  ImGui::Separator();
  if (ImGui::CollapsingHeader("Level Map")) {
    for (auto idx = 0; idx < 4692; idx++) {
      auto ent = gGlobalState->level_state->entity_floors[idx];

      auto col = IM_COL32(183, 183, 183, 255);
      // Empty
      if (ent == NULL) {
        col = IM_COL32(0, 0, 0, 0);
      } else if (ent->entity_type == 3 || ent->entity_type == 2) {
        // Doors
        col = IM_COL32(59, 196, 0, 255);
      } else if (ent->entity_type == 4 || ent->entity_type == 5) {
        // Ladders
        col = IM_COL32(133, 133, 133, 100);
      } else if (ent->entity_type == 25) {
        // Bedrock
        col = IM_COL32(90, 90, 90, 255);
      } else if (ent->entity_type == 27) {
        // Water
        col = IM_COL32(43, 114, 214, 100);
      } else if (ent->entity_type == 36) {
        // Lava
        col = IM_COL32(214, 54, 43, 100);
      } else if (ent->entity_type == 91) {
        // Acid
        col = IM_COL32(43, 214, 77, 100);
      }

      if (idx % 46 > 0) {

        ImGui::SameLine(0.f * io.FontGlobalScale, 4.f * io.FontGlobalScale);
      }
      RectFilled(size, col);
    }
  }

  ImVec2 roomTypeSize = {40.f, 40.f};
  ImGui::Separator();
  if (ImGui::CollapsingHeader("Room Types")) {
    for (auto idx = 0; idx < 48; idx++) {
      auto column = idx % 4;
      auto type = gGlobalState->level_state->room_types[idx];
      auto col = IM_COL32(183, 183, 183, 255);
      if (type >= 1 && type <= 3) {
        // Path
        col = IM_COL32(59, 196, 0, 255);
      }
      if (column > 0) {

        ImGui::SameLine(0.f * io.FontGlobalScale, 4.f * io.FontGlobalScale);
      }
      auto label = std::format("{}", type);
      Rect(label.c_str(), roomTypeSize, col);
    }
  }
}
