
#include "level_tab.h"

#include "../drawing.h"
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
    for (auto idx = 0; idx < ENTITY_FLOORS_COUNT; idx++) {
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
