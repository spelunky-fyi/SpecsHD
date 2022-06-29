
#include <Windows.h>
#include <algorithm>
#include <format>
#include <functional>
#include <map>
#include <unordered_set>

#include "3rdparty/imgui/imgui.h"
#include "3rdparty/imgui/misc/cpp/imgui_stdlib.h"

#include "config.h"
#include "entities.h"
#include "hd.h"
#include "inputs.h"
#include "sounds.h"
#include "ui.h"

// Global States
DWORD gBaseAddress = NULL;
ImDrawList *gOverlayDrawList = NULL;

CameraState *gCameraState = NULL;
GlobalState *gGlobalState = NULL;

Specs::Config *gConfig = NULL;

bool gPaused = false;
int gPauseAt = 0;
int gFrame = 0;

int gWindowedMode = 0;
int gDisplayWidth = 0;
int gDisplayHeight = 0;

struct EnabledEntities {
  bool activeEntities = false;
  bool floorEntities = false;
  bool floorBgEntities = false;
  bool backgroundEntities = false;
  bool unknown1400 = false;
  bool foregroundEntities = false;
  bool lightEmittingEntities = false;
  bool _4cStructEntities = false;

  int excludeEntityInput = -1;
  std::unordered_set<uint32_t> excluded = {171, 177};
};

struct DebugState {
  bool EnableTileBorders = false;
  bool EnableBinBorders = false;
  bool EnablePacifistOverlay = false;

  EnabledEntities Ids;
  EnabledEntities Hitboxes;
  EnabledEntities Selection;

  bool DrawSelectedEntHitbox = false;
  bool DrawClosestEntHitbox = false;
  bool DrawClosestEntId = false;

  bool IncludeFloorDecos = false;
};
DebugState gDebugState = {};

struct SpawnState {
  int SpawnEntityInput = 0;
  std::string EntityListFilter;
  ImVec2 ClickedAt = {0, 0};
  bool ClickToSpawn = false;
  bool AddToActive = true;
  bool Clicking = false;
};
SpawnState gSpawnState = {};

struct SelectedEntityState {

  bool Clicking = false;
  Entity *Entity = NULL;
};
SelectedEntityState gSelectedEntityState = {};

void patchReadOnlyCode(HANDLE process, DWORD addr, void *value, size_t size) {
  DWORD oldrights;
  VirtualProtectEx(process, (LPVOID)addr, size, PAGE_EXECUTE_READWRITE,
                   &oldrights);

  WriteProcessMemory(process, (LPVOID)addr, value, size, NULL);
  VirtualProtectEx(process, (LPVOID)addr, size, oldrights, &oldrights);
}

void specsOnInit() {

  gConfig = Specs::Config::load();

  gBaseAddress = (size_t)GetModuleHandleA(NULL);
  setupOffsets(gBaseAddress);

  gDebugState.Selection.activeEntities = true;
  gDebugState.Selection.floorEntities = true;

  auto process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ |
                                 PROCESS_VM_WRITE | PROCESS_VM_OPERATION |
                                 PROCESS_CREATE_THREAD,
                             0, GetCurrentProcessId());

  BYTE patch[] = {0x4a};
  patchReadOnlyCode(process, gBaseAddress + 0x135B2A, patch, 1);

  BYTE patch2[] = {0xF0};
  patchReadOnlyCode(process, gBaseAddress + 0x1366C6, patch2, 1);
  CloseHandle(process);
}

struct PlayerState {
  bool LockHealth = false;
  int LockedHealthAmount = 0;

  bool LockBombs = false;
  int LockedBombsAmount = 0;

  bool LockRopes = false;
  int LockedRopesAmount = 0;
};
PlayerState gPlayersState[4] = {{}, {}, {}, {}};

// If we want to normalize the screen position to 0,0 at the top-left
// if (gWindowedMode == 2) {
//   RECT windowRect;
//   GetWindowRect(ui::window, &windowRect);
//   screen.x = screen.x + windowRect.left;
//   screen.y = screen.y + windowRect.top;
// }

ImVec2 screenToGame(ImVec2 screen) {

  auto x =
      (screen.x - ((float)gDisplayWidth / 2)) * (20 / (float)gDisplayWidth) +
      gCameraState->camera_x;
  auto y =
      (screen.y - ((float)gDisplayHeight / 2)) * -(20 / (float)gDisplayWidth) +
      gCameraState->camera_y;

  return {x, y};
}

ImVec2 gameToScreen(ImVec2 game) {
  auto x = (game.x - gCameraState->camera_x) / (20 / (float)gDisplayWidth) +
           ((float)gDisplayWidth / 2);
  auto y = (game.y - gCameraState->camera_y) / -(20 / (float)gDisplayWidth) +
           ((float)gDisplayHeight / 2);
  return {x, y};
}

void drawEntityHitbox(Entity *ent,
                      ImU32 color = ImGui::GetColorU32({255.f, 0.0f, 238.0f,
                                                        0.7f})) {
  auto screen = gameToScreen({ent->x, ent->y});
  ImVec2 topLeft =
      gameToScreen({ent->x - ent->hitbox_x, ent->y + ent->hitbox_up});
  ImVec2 topRight =
      gameToScreen({ent->x + ent->hitbox_x, ent->y + ent->hitbox_up});
  ImVec2 bottomRight =
      gameToScreen({ent->x + ent->hitbox_x, ent->y - ent->hitbox_down});
  ImVec2 bottomLeft =
      gameToScreen({ent->x - ent->hitbox_x, ent->y - ent->hitbox_down});

  gOverlayDrawList->AddQuad(topLeft, topRight, bottomRight, bottomLeft, color,
                            1.f);
}

void drawEntityHitboxDefault(Entity *ent) { drawEntityHitbox(ent); }

void drawPacifistOverlay() {
  for (size_t idx = 0; idx < gGlobalState->entities->entities_active_count;
       idx++) {
    auto ent = gGlobalState->entities->entities_active[idx];
    if (!ent) {
      continue;
    }

    if (ent->owner == Ownership::Unowned) {
      continue;
    }

    auto screen = gameToScreen({ent->x, ent->y});
    auto out = std::format("{}", (int)ent->owner);
    gOverlayDrawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() + 2.f,
                              ImVec2{screen.x, screen.y}, IM_COL32_WHITE,
                              out.c_str());
  }
}

void drawBinBorders() {
  for (auto x = -4.f; x < 12.f * 4.f; x += 4.f) {
    gOverlayDrawList->AddLine(gameToScreen({x, -4.f}), gameToScreen({x, 120.f}),
                              IM_COL32_WHITE);
  }
  for (auto y = -4.f; y < 26.f * 4.f; y += 4.f) {
    gOverlayDrawList->AddLine(gameToScreen({-4.f, y}), gameToScreen({50.f, y}),
                              IM_COL32_WHITE);
  }
}

void drawTileBorders() {
  for (auto x = -4.f; x < 12.f * 4.f; x += 1.f) {
    gOverlayDrawList->AddLine(gameToScreen({x + -.5f, -4.5f}),
                              gameToScreen({x + -.5f, 120.5f}), IM_COL32_WHITE,
                              0.5f);
  }
  for (auto y = -4.f; y < 26.f * 4.f; y += 1.f) {
    gOverlayDrawList->AddLine(gameToScreen({-4.5f, y + 0.5f}),
                              gameToScreen({50.5f, y + 0.5f}), IM_COL32_WHITE,
                              0.5f);
  }
}

void drawEntityId(Entity *ent) {
  auto screen = gameToScreen({ent->x, ent->y});
  auto out = std::format("{}", ent->entity_type);
  gOverlayDrawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() + 5,
                            ImVec2{screen.x, screen.y}, IM_COL32_WHITE,
                            out.c_str());
}

using EntityCallback = std::function<void(Entity *e)>;
void forEntities(std::unordered_set<uint32_t> excludedEntities,
                 EntityCallback callback, Entity **entities, size_t count,
                 bool decos = false) {
  for (size_t idx = 0; idx < count; idx++) {
    auto ent = entities[idx];
    if (!ent) {
      continue;
    }

    if (excludedEntities.contains(ent->entity_type)) {
      continue;
    }

    if (decos) {
      if (ent->deco_over) {
        callback(ent->deco_over);
      }
      if (ent->deco_top) {
        callback(ent->deco_top);
      }
      if (ent->deco_bottom) {
        callback(ent->deco_bottom);
      }
      if (ent->deco_left) {
        callback(ent->deco_left);
      }
      if (ent->deco_right) {
        callback(ent->deco_right);
      }
    }

    callback(ent);
  }
}

bool findEntityArray(Entity *searchEnt, Entity **entities, size_t count,
                     bool decos = false) {
  for (size_t idx = 0; idx < count; idx++) {
    auto ent = entities[idx];
    if (!ent) {
      continue;
    }

    if (decos) {
      if (ent->deco_over) {
        if (searchEnt == ent->deco_over) {
          return true;
        }
      }
      if (ent->deco_top) {
        if (searchEnt == ent->deco_top) {
          return true;
        }
      }
      if (ent->deco_bottom) {
        if (searchEnt == ent->deco_bottom) {
          return true;
        }
      }
      if (ent->deco_left) {
        if (searchEnt == ent->deco_left) {
          return true;
        }
      }
      if (ent->deco_right) {
        if (searchEnt == ent->deco_right) {
          return true;
        }
      }
    }

    if (searchEnt == ent)
      return true;
  }
  return false;
}

bool findEntity(Entity *searchEnt) {
  if (searchEnt == NULL) {
    return false;
  }
  return (
      findEntityArray(searchEnt, gGlobalState->entities->entities_active,
                      gGlobalState->entities->entities_active_count) ||
      findEntityArray(searchEnt, gGlobalState->entities->array_1400,
                      gGlobalState->entities->array_1400_count) ||
      findEntityArray(
          searchEnt, gGlobalState->entities->entities_foreground,
          gGlobalState->entities->array_entities_foreground_count) ||
      findEntityArray(
          searchEnt, gGlobalState->entities->entities_foreground,
          gGlobalState->entities->array_entities_foreground_count) ||
      findEntityArray(searchEnt,
                      gGlobalState->entities->entities_light_emitting,
                      gGlobalState->entities->entities_light_emitting_count) ||
      findEntityArray(searchEnt,
                      (Entity **)gGlobalState->level_state->entity_floors, 4692,
                      gDebugState.IncludeFloorDecos) ||

      findEntityArray(searchEnt,
                      (Entity **)gGlobalState->level_state->entity_floors_bg,
                      4692) ||

      findEntityArray(searchEnt,
                      (Entity **)gGlobalState->level_state->entity_backgrounds,
                      gGlobalState->level_state->entity_backgrounds_count) ||
      findEntityArray(searchEnt, gGlobalState->_4cstruct->entities, 160));
}

void forEnabledEntities(EnabledEntities &enabledEnts, EntityCallback callback) {

  // Active
  if (enabledEnts.activeEntities) {
    forEntities(enabledEnts.excluded, callback,
                gGlobalState->entities->entities_active,
                gGlobalState->entities->entities_active_count);
  }

  // 1400
  if (enabledEnts.unknown1400) {
    forEntities(enabledEnts.excluded, callback,
                gGlobalState->entities->array_1400,
                gGlobalState->entities->array_1400_count);
  }
  // Foreground
  if (enabledEnts.foregroundEntities) {
    forEntities(enabledEnts.excluded, callback,
                gGlobalState->entities->entities_foreground,
                gGlobalState->entities->array_entities_foreground_count);
  }

  // Light Emitting
  if (enabledEnts.lightEmittingEntities) {
    forEntities(enabledEnts.excluded, callback,
                gGlobalState->entities->entities_light_emitting,
                gGlobalState->entities->entities_light_emitting_count);
  }

  // Floors
  if (enabledEnts.floorEntities) {
    forEntities(enabledEnts.excluded, callback,
                (Entity **)gGlobalState->level_state->entity_floors, 4692,
                gDebugState.IncludeFloorDecos);
  }
  if (enabledEnts.floorBgEntities) {
    forEntities(enabledEnts.excluded, callback,
                (Entity **)gGlobalState->level_state->entity_floors_bg, 4692);
  }

  // Backgrounds
  if (enabledEnts.backgroundEntities) {
    forEntities(enabledEnts.excluded, callback,
                (Entity **)gGlobalState->level_state->entity_backgrounds,
                gGlobalState->level_state->entity_backgrounds_count);
  }

  if (enabledEnts._4cStructEntities) {
    forEntities(enabledEnts.excluded, callback,
                gGlobalState->_4cstruct->entities, 160);
  }
}

float dist(ImVec2 pos1, ImVec2 pos2) {
  return sqrt(pow(pos2.x - pos1.x, 2.0f) + pow(pos2.y - pos1.y, 2.0f));
}

void drawOverlayWindow() {
  ImGuiIO &io = ImGui::GetIO();

  ImGui::SetNextWindowSize(io.DisplaySize);
  ImGui::SetNextWindowPos({0, 0});
  ImGui::Begin(
      "Overlay", NULL,
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
          ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
          ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse |
          ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus |
          ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavInputs |
          ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
  gOverlayDrawList = ImGui::GetWindowDrawList();

  // check if selected entity exists
  if (!findEntity(gSelectedEntityState.Entity)) {
    gSelectedEntityState.Entity = NULL;
  }

  Entity *closestEnt = NULL;
  if (ImGui::IsWindowHovered()) {

    // Teleport Player
    if (Specs::IsMouseClicked(
            gConfig->buttons[Specs::MouseFeatures_Teleport])) {
      auto player = gGlobalState->player1;
      if (player) {
        auto pos = screenToGame(io.MousePos);
        player->x = pos.x;
        player->y = pos.y;
      }
    }

    // Spawn Entity
    auto spawnMouseConfig = gConfig->buttons[Specs::MouseFeatures_SpawnEntity];
    if (gSpawnState.ClickToSpawn && Specs::IsMouseClicked(spawnMouseConfig)) {
      gSpawnState.ClickedAt = io.MousePos;
      gSpawnState.Clicking = true;
    }

    if (gSpawnState.ClickToSpawn && gSpawnState.Clicking &&
        io.MouseDown[spawnMouseConfig.Button]) {
      gOverlayDrawList->AddLine(gSpawnState.ClickedAt, io.MousePos,
                                ImGui::GetColorU32({0.0f, 1.0f, .5f, .9f}));
    }
    if (gSpawnState.ClickToSpawn && gSpawnState.Clicking &&
        io.MouseReleased[spawnMouseConfig.Button]) {
      gSpawnState.Clicking = false;

      if (gSpawnState.SpawnEntityInput > 0) {
        if (io.MouseDownDurationPrev[spawnMouseConfig.Button] > 0.1f) {
          auto gamePos = screenToGame(gSpawnState.ClickedAt);
          auto ent = gGlobalState->SpawnEntity(gamePos.x, gamePos.y,
                                               gSpawnState.SpawnEntityInput,
                                               gSpawnState.AddToActive);
          if ((uint32_t)ent->entity_kind > 0 &&
              (uint32_t)ent->entity_kind < 5) {

            auto activeEnt = (EntityActive *)ent;
            if (ent->entity_type != 108) {
              activeEnt->velocity_x =
                  (io.MousePos.x - gSpawnState.ClickedAt.x) * 0.01f;
            }
            activeEnt->velocity_y =
                -((io.MousePos.y - gSpawnState.ClickedAt.y) * 0.01f);
          }

        } else {
          auto gamePos = screenToGame(io.MousePos);
          gGlobalState->SpawnEntity(gamePos.x, gamePos.y,
                                    gSpawnState.SpawnEntityInput,
                                    gSpawnState.AddToActive);
        }
      }
    }

    // Select Entity
    auto selectEntMouseConfig =
        gConfig->buttons[Specs::MouseFeatures_SelectEntity];
    auto selectEntClicked = Specs::IsMouseClicked(selectEntMouseConfig);

    if (gSelectedEntityState.Clicking &&
        io.MouseReleased[selectEntMouseConfig.Button]) {
      gSelectedEntityState.Clicking = false;
    }

    auto gamePos = screenToGame(io.MousePos);
    if (gDebugState.DrawClosestEntHitbox || gDebugState.DrawClosestEntId ||
        selectEntClicked) {
      float closestEntDist = 1;
      if (selectEntClicked) {
        gSelectedEntityState.Entity = NULL;
        gSelectedEntityState.Clicking = true;
      }
      EntityCallback getClosestEnt = [&](Entity *e) {
        auto eDist = dist(gamePos, ImVec2(e->x, e->y));
        if (eDist < closestEntDist) {
          closestEnt = e;
          closestEntDist = eDist;
        }
      };
      forEnabledEntities(gDebugState.Selection, getClosestEnt);
      if (closestEnt && selectEntClicked) {
        gSelectedEntityState.Entity = closestEnt;
      }
    }

    if (gSelectedEntityState.Entity != NULL && gSelectedEntityState.Clicking &&
        io.MouseDown[selectEntMouseConfig.Button] &&
        io.MouseDownDuration[selectEntMouseConfig.Button] > 0.2f) {
      gSelectedEntityState.Entity->x =
          std::lerp(gSelectedEntityState.Entity->x, gamePos.x, 1.f);
      gSelectedEntityState.Entity->y =
          std::lerp(gSelectedEntityState.Entity->y, gamePos.y, 1.f);
      if ((int)gSelectedEntityState.Entity->entity_kind > 0 &&
          (int)gSelectedEntityState.Entity->entity_kind < 5) {
        auto ent = (EntityActive *)gSelectedEntityState.Entity;
        ent->time_in_air = 0.f;
      }
    }
  }

  gOverlayDrawList->AddText(
      ImGui::GetFont(), ImGui::GetFontSize() + 5, {148.f, 40.f},
      ImGui::GetColorU32({1.0f, 1.0f, 1.0f, 0.05f}), "SpecsHD");

  if (gDebugState.EnableTileBorders) {
    drawTileBorders();
  }

  if (gDebugState.EnableBinBorders) {
    drawBinBorders();
  }

  if (gDebugState.EnablePacifistOverlay) {
    drawPacifistOverlay();
  }

  forEnabledEntities(gDebugState.Hitboxes, &drawEntityHitboxDefault);

  forEnabledEntities(gDebugState.Ids, &drawEntityId);

  if (gSelectedEntityState.Entity != NULL &&
      gDebugState.DrawSelectedEntHitbox) {
    drawEntityHitbox(gSelectedEntityState.Entity,
                     ImGui::GetColorU32({1.0f, 1.0f, 1.0f, .9f}));
  }
  if (closestEnt) {
    if (gDebugState.DrawClosestEntHitbox)
      drawEntityHitbox(closestEnt, ImGui::GetColorU32({0.0f, 1.0f, .5f, .9f}));
    if (gDebugState.DrawClosestEntId) {
      drawEntityId(closestEnt);
    }
  }
  ImGui::End();
}

void drawSpawnTab() {
  auto scrollLock = true;
  if (ImGui::InputInt("Entity ID", &gSpawnState.SpawnEntityInput)) {
    scrollLock = false;
  };
  ImGui::SameLine();
  if (ImGui::Button("Spawn")) {
    if (gSpawnState.SpawnEntityInput > 0) {
      gGlobalState->SpawnEntity(
          gGlobalState->player1->x, gGlobalState->player1->y,
          gSpawnState.SpawnEntityInput, gSpawnState.AddToActive);
    }
  }
  ImGui::Checkbox("Click to spawn", &gSpawnState.ClickToSpawn);
  ImGui::Checkbox("Add to Active List", &gSpawnState.AddToActive);

  ImGui::Separator();
  if (ImGui::InputText("Filter", &gSpawnState.EntityListFilter)) {
    std::transform(gSpawnState.EntityListFilter.begin(),
                   gSpawnState.EntityListFilter.end(),
                   gSpawnState.EntityListFilter.begin(), ::tolower);
  }

  if (ImGui::BeginListBox("##", {-1, -1})) {
    for (auto const &[name, entity_type] : gEntities) {

      std::string lowerName = name;
      std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
                     ::tolower);
      if (!gSpawnState.EntityListFilter.empty() &&
          lowerName.find(gSpawnState.EntityListFilter) == std::string::npos) {
        continue;
      }
      auto label = std::format("{:4}: {}", entity_type, name);
      const bool is_selected = (entity_type == gSpawnState.SpawnEntityInput);
      if (ImGui::Selectable(label.c_str(), is_selected)) {
        gSpawnState.SpawnEntityInput = entity_type;
      }

      if (is_selected) {
        ImGui::SetItemDefaultFocus();
        if (!scrollLock) {
          ImGui::SetScrollHereY();
        }
      }
    }
    ImGui::EndListBox();
  }
}

void warpToLevel(uint32_t level) {
  gGlobalState->level = level;
  gGlobalState->screen_state = 3;
}

void RectFilled(ImVec2 &size, ImU32 col = IM_COL32_WHITE, float rounding = 0.f,
                ImDrawFlags flags = 0) {

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

void Rect(const char *label, ImVec2 &size, ImU32 col = IM_COL32_WHITE,
          float rounding = 0.f, ImDrawFlags flags = 0, float thickness = 1.0f) {

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

void drawLevelTab() {

  auto isDisabled =
      gGlobalState->screen_state != 0 || gGlobalState->play_state != 0;

  if (isDisabled) {
    ImGui::BeginDisabled();
  }
  ImGui::Text("");
  ImGui::SameLine(100.0f);
  if (ImGui::Button("Next Level")) {
    warpToLevel(gGlobalState->level);
  }

  ImGui::Text("Mines");
  ImGui::SameLine(100.0f);
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
  ImGui::SameLine(100.0f);
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
  ImGui::SameLine(100.0f);
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
  ImGui::SameLine(100.0f);
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
  ImGui::SameLine(100.0f);
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

  if (isDisabled) {
    ImGui::EndDisabled();
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

        ImGui::SameLine(0.f, 4.f);
      }
      RectFilled(size, col);
    }
  }

  ImVec2 roomTypeSize = {40.f, 40.f};
  ImGui::Separator();
  if (ImGui::CollapsingHeader("Room Types")) {
    for (auto idx = 0; idx < 48; idx++) {
      auto column = idx % 4;
      if (gGlobalState->is_worm) {
        if (column > 1) {
          continue;
        }
      } else {
        if (idx >= 4 * 4) {
          break;
        }
      }

      auto type = gGlobalState->level_state->room_types[idx];

      auto col = IM_COL32(183, 183, 183, 255);
      if (type >= 1 && type <= 3) {
        // Path
        col = IM_COL32(59, 196, 0, 255);
      }
      if (column > 0) {

        ImGui::SameLine(0.f, 4.f);
      }
      auto label = std::format("{}", type);
      Rect(label.c_str(), roomTypeSize, col);
    }
  }
}

void ensureLockedAmountsForPlayer(EntityPlayer *player, PlayerData &data,
                                  PlayerState *state) {
  if (state->LockHealth) {
    // If you ressurect a player it gets into a bad state.
    if (player->health > 0) {
      player->health = state->LockedHealthAmount;
    }
  }

  if (state->LockBombs) {
    data.bombs = state->LockedBombsAmount;
  }

  if (state->LockRopes) {
    data.ropes = state->LockedRopesAmount;
  }
}

void ensureLockedAmounts() {
  if (gGlobalState->player1) {
    ensureLockedAmountsForPlayer(gGlobalState->player1,
                                 gGlobalState->player1_data, &gPlayersState[0]);
  }

  if (gGlobalState->player2) {
    ensureLockedAmountsForPlayer(gGlobalState->player2,
                                 gGlobalState->player2_data, &gPlayersState[1]);
  }

  if (gGlobalState->player3) {
    ensureLockedAmountsForPlayer(gGlobalState->player3,
                                 gGlobalState->player3_data, &gPlayersState[2]);
  }

  if (gGlobalState->player4) {
    ensureLockedAmountsForPlayer(gGlobalState->player4,
                                 gGlobalState->player4_data, &gPlayersState[3]);
  }
}

void drawPlayerTab(EntityPlayer *player, PlayerData &data, PlayerState *state) {
  if (!player) {
    ImGui::Text("No Player Entity");
    return;
  }

  if (ImGui::Button("Max Health/Bombs/Ropes")) {
    player->health = 99;
    state->LockedHealthAmount = player->health;
    data.bombs = 99;
    state->LockedBombsAmount = data.bombs;
    data.ropes = 99;
    state->LockedRopesAmount = data.ropes;
  }

  ImGui::Text("Locked?");
  ImGui::SameLine(80.0f);
  ImGui::Text("Amount");

  if (ImGui::Checkbox("##LockHealth", &state->LockHealth)) {
    if (state->LockHealth) {
      state->LockedHealthAmount = player->health;
    }
  };
  ImGui::SameLine(80.0f);
  ImGui::PushItemWidth(100);
  if (ImGui::InputInt("Health", &player->health)) {
    player->health = std::clamp(player->health, 0, 99);
    state->LockedHealthAmount = player->health;
  }
  ImGui::PopItemWidth();

  if (ImGui::Checkbox("##LockBombs", &state->LockBombs)) {
    if (state->LockBombs) {
      state->LockedBombsAmount = data.bombs;
    }
  }
  ImGui::SameLine(80.0f);
  ImGui::PushItemWidth(100);
  if (ImGui::InputInt("Bombs", &data.bombs)) {
    data.bombs = std::clamp(data.bombs, 0, 99);
    state->LockedBombsAmount = data.bombs;
  }
  ImGui::PopItemWidth();

  if (ImGui::Checkbox("##LockRopes", &state->LockRopes)) {
    if (state->LockRopes) {
      state->LockedRopesAmount = data.ropes;
    }
  }
  ImGui::SameLine(80.0f);
  ImGui::PushItemWidth(100);
  if (ImGui::InputInt("Ropes", &data.ropes)) {
    data.ropes = std::clamp(data.ropes, 0, 99);
    state->LockedRopesAmount = data.ropes;
  }
  ImGui::PopItemWidth();
  ImGui::Separator();

  ImGui::Checkbox("Compass", &data.has_compass);
  ImGui::Checkbox("Parachute", &data.has_parachute);

  ImGui::Checkbox("Jetpack", &data.has_jetpack);
  ImGui::SameLine();
  if (ImGui::Button("Spawn##Jetpack")) {
    gGlobalState->SpawnEntity(player->x, player->y, 522,
                              gSpawnState.AddToActive);
  }

  ImGui::Checkbox("Climbing Gloves", &data.has_climbing_gloves);
  ImGui::Checkbox("Pitcher's Mitt", &data.has_pitchers_mitt);
  ImGui::Checkbox("Spring Shoes", &data.has_spring_shoes);
  ImGui::Checkbox("Spike Shoes", &data.has_spike_shoes);
  ImGui::Checkbox("Spectacles", &data.has_spectacles);
  ImGui::Checkbox("Kapala", &data.has_kapala);
  ImGui::Checkbox("Hedjet", &data.has_hedjet);
  ImGui::Checkbox("Udjat Eye", &data.has_udjat);
  ImGui::Checkbox("Book of the Dead", &data.has_book_of_dead);
  ImGui::Checkbox("Ankh", &data.has_ankh);
  ImGui::Checkbox("Paste", &data.has_paste);

  ImGui::Checkbox("Cape", &data.has_cape);
  ImGui::SameLine();
  if (ImGui::Button("Spawn##Cape")) {
    gGlobalState->SpawnEntity(player->x, player->y, 521,
                              gSpawnState.AddToActive);
  }

  ImGui::Checkbox("Vlad's Cape", &data.has_vlads_cape);
  ImGui::SameLine();
  if (ImGui::Button("Spawn##VladsCape")) {
    gGlobalState->SpawnEntity(player->x, player->y, 532,
                              gSpawnState.AddToActive);
  }

  ImGui::Checkbox("Crysknife", &data.has_crysknife);
  ImGui::Checkbox("Vlad's Amulet", &data.has_vlads_amulet);
  ImGui::Checkbox("White Flag", &data.has_white_flag);
}

void drawPlayersTab() {

  if (ImGui::BeginTabBar("Players")) {
    if (ImGui::BeginTabItem("Player 1")) {
      drawPlayerTab(gGlobalState->player1, gGlobalState->player1_data,
                    &gPlayersState[0]);
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Player 2")) {
      drawPlayerTab(gGlobalState->player2, gGlobalState->player2_data,
                    &gPlayersState[1]);
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Player 3")) {
      drawPlayerTab(gGlobalState->player3, gGlobalState->player3_data,
                    &gPlayersState[2]);
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Player 4")) {
      drawPlayerTab(gGlobalState->player4, gGlobalState->player4_data,
                    &gPlayersState[3]);
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
}

void drawAudioTab() {

  // auto player = *(&((*gGlobalState).player1) + gGlobalState->flag_player);
  auto player = gGlobalState->player1;
  if (!player) {
    ImGui::Text("Need Player Entity");
    return;
  }

  if (ImGui::BeginListBox("##", {-1, -1})) {
    for (size_t idx = 0; idx < gAudioNames.size(); idx++) {
      if (ImGui::Selectable(gAudioNames[idx], false)) {
        player->PlaySound(gAudioNames[idx]);
      }
    }
    ImGui::EndListBox();
  }
}

void drawToggleEntityTab(const char *preText, EnabledEntities &enabledEnts) {
  ImGui::Checkbox(std::format("{} Active Entities", preText).c_str(),
                  &enabledEnts.activeEntities);
  ImGui::Checkbox(std::format("{} Light Emitting", preText).c_str(),
                  &enabledEnts.lightEmittingEntities);
  ImGui::Checkbox(
      std::format("{} Unknown 1400 (BG + Active?)", preText).c_str(),
      &enabledEnts.unknown1400);
  ImGui::Checkbox(std::format("{} Foreground Entities", preText).c_str(),
                  &enabledEnts.foregroundEntities);
  ImGui::Checkbox(std::format("{} Floor Entities", preText).c_str(),
                  &enabledEnts.floorEntities);
  ImGui::Checkbox(std::format("{} Floor Background Entities", preText).c_str(),
                  &enabledEnts.floorBgEntities);
  ImGui::Checkbox(std::format("{} Background Entities", preText).c_str(),
                  &enabledEnts.backgroundEntities);
  ImGui::Checkbox(std::format("{} 4c Struct Entities", preText).c_str(),
                  &enabledEnts._4cStructEntities);

  ImGui::InputInt(std::format("Exclude Entity {}", preText).c_str(),
                  &enabledEnts.excludeEntityInput);
  if (ImGui::Button(std::format("Exclude##Debug{}", preText).c_str())) {
    if (enabledEnts.excludeEntityInput >= 0) {
      enabledEnts.excluded.insert(enabledEnts.excludeEntityInput);
      enabledEnts.excludeEntityInput = 0;
    }
  }
  ImGui::Separator();
  for (auto ent_type : enabledEnts.excluded) {
    auto label = std::format("Remove {}##Debug{}", ent_type, preText);
    if (ImGui::Button(label.c_str())) {
      enabledEnts.excluded.erase(ent_type);
    }
  }
}

void drawCharBool(const char *label, char &flag) {
  bool bflag = flag ? 1 : 0;
  ImGui::Checkbox(label, &bflag);
  bflag ? flag = 1 : flag = 0;
}

void drawCharBool(const char *label, uint8_t &flag) {
  bool bflag = flag ? 1 : 0;
  ImGui::Checkbox(label, &bflag);
  bflag ? flag = 1 : flag = 0;
}

void drawDebugTab() {
  ImGuiIO &io = ImGui::GetIO();

  auto gameMouse = screenToGame(io.MousePos);
  ImGui::Text("Mouse: %f %f", io.MousePos.x, io.MousePos.y);
  ImGui::Text("Mouse (Game): %f %f", gameMouse.x, gameMouse.y);
  if (gGlobalState->player1) {
    auto screenPlayer =
        gameToScreen({gGlobalState->player1->x, gGlobalState->player1->y});
    ImGui::Text("Player (Screen): %f %f", screenPlayer.x, screenPlayer.y);
  }
  ImGui::Checkbox("Draw Tile Borders", &gDebugState.EnableTileBorders);
  ImGui::Checkbox("Draw Bin Borders", &gDebugState.EnableBinBorders);
  ImGui::Checkbox("Draw Owned Entities", &gDebugState.EnablePacifistOverlay);
  ImGui::Checkbox("Include Floor Decorations", &gDebugState.IncludeFloorDecos);

  if (ImGui::CollapsingHeader("Draw Hitboxes")) {
    drawToggleEntityTab("Show", gDebugState.Hitboxes);
  }
  if (ImGui::CollapsingHeader("Draw Ids")) {
    drawToggleEntityTab("Draw", gDebugState.Ids);
  }
  if (ImGui::CollapsingHeader("Selectable Entities")) {
    ImGui::Checkbox("Draw Selected Entity Hitbox",
                    &gDebugState.DrawSelectedEntHitbox);
    ImGui::Checkbox("Draw Closest Entity Hitbox",
                    &gDebugState.DrawClosestEntHitbox);
    ImGui::Checkbox("Draw Closest Entity Id", &gDebugState.DrawClosestEntId);
    ImGui::Separator();
    drawToggleEntityTab("Enable", gDebugState.Selection);
  }

  if (ImGui::CollapsingHeader("Global State")) {

    ImGui::InputScalar("screen_state", ImGuiDataType_U32,
                       &gGlobalState->screen_state);
    ImGui::InputScalar("play_state", ImGuiDataType_U32,
                       &gGlobalState->play_state);
    ImGui::InputScalar("flag_player", ImGuiDataType_U32,
                       &gGlobalState->flag_player);
    ImGui::InputScalar("level", ImGuiDataType_U32, &gGlobalState->level);
    ImGui::InputScalar("level_track", ImGuiDataType_U32,
                       &gGlobalState->level_track);
    if (ImGui::CollapsingHeader("GlobalState Flags")) {
      drawCharBool("dark_level", gGlobalState->dark_level);
      drawCharBool("altar_spawned", gGlobalState->altar_spawned);
      drawCharBool("idol_spawned", gGlobalState->idol_spawned);
      drawCharBool("damsel_spawned", gGlobalState->damsel_spawned);
      drawCharBool("unknown_flag", gGlobalState->unknown_flag);
      drawCharBool("moai_unopened", gGlobalState->moai_unopened);
      drawCharBool("moai_broke_in", gGlobalState->moai_broke_in);
      drawCharBool("ghost_spawned", gGlobalState->ghost_spawned);
      drawCharBool("rescued_damsel", gGlobalState->rescued_damsel);
      drawCharBool("shopkeeper_triggered", gGlobalState->shopkeeper_triggered);
      drawCharBool("area_had_dark_level", gGlobalState->area_had_dark_level);
      drawCharBool("level_has_udjat", gGlobalState->level_has_udjat);
      drawCharBool("has_spawned_udjat", gGlobalState->has_spawned_udjat);
      drawCharBool("unused_flag", gGlobalState->unused_flag);
      drawCharBool("vault_spawned_in_area",
                   gGlobalState->vault_spawned_in_area);
      drawCharBool("flooded_mines", gGlobalState->flooded_mines);
      drawCharBool("skin_is_crawling", gGlobalState->skin_is_crawling);
      drawCharBool("dead_are_restless", gGlobalState->dead_are_restless);
      drawCharBool("rushing_water", gGlobalState->rushing_water);
      drawCharBool("is_haunted_castle", gGlobalState->is_haunted_castle);
      drawCharBool("at_haunted_castle_exit",
                   gGlobalState->at_haunted_castle_exit);
      drawCharBool("tiki_village", gGlobalState->tiki_village);
      drawCharBool("spawned_black_market_entrance",
                   gGlobalState->spawned_black_market_entrance);
      drawCharBool("unused_flag2", gGlobalState->unused_flag2);
      drawCharBool("spawned_haunted_castle_entrance",
                   gGlobalState->spawned_haunted_castle_entrance);
      drawCharBool("mothership_spawned", gGlobalState->mothership_spawned);
      drawCharBool("moai_spawned", gGlobalState->moai_spawned);
      drawCharBool("is_blackmarket", gGlobalState->is_blackmarket);
      drawCharBool("at_blackmarket_exit", gGlobalState->at_blackmarket_exit);
      drawCharBool("is_wet_fur", gGlobalState->is_wet_fur);
      drawCharBool("is_mothership", gGlobalState->is_mothership);
      drawCharBool("at_mothership_exit", gGlobalState->at_mothership_exit);
      drawCharBool("is_city_of_gold", gGlobalState->is_city_of_gold);
      drawCharBool("at_city_of_gold_exit", gGlobalState->at_city_of_gold_exit);
      drawCharBool("is_worm", gGlobalState->is_worm);
    }
  }
}

size_t sizeofEntityKind(EntityKind entityKind) {
  switch (entityKind) {
  case EntityKind::KIND_FLOOR:
    return sizeof(EntityFloor);
  case EntityKind::KIND_ACTIVE:
    return sizeof(EntityActive);
  case EntityKind::KIND_PLAYER:
    return sizeof(EntityPlayer);
  case EntityKind::KIND_MONSTER:
    return sizeof(EntityMonster);
  case EntityKind::KIND_ITEM:
    return sizeof(EntityItem);
  case EntityKind::KIND_BACKGROUND:
    return sizeof(EntityBackground);
  case EntityKind::KIND_EXPLOSION:
    return sizeof(EntityExplosion);
  case EntityKind::KIND_ENTITY:
    return sizeof(Entity);
  default:
    return 0;
  }
}

void drawSelectedEntityTab() {
  ImGuiIO &io = ImGui::GetIO();
  if (!gSelectedEntityState.Entity) {
    ImGui::Text("No selected entity");
    return;
  }
  ImGui::Text("Address: 0x%X", (uint32_t)gSelectedEntityState.Entity);
  ImGui::Text("Entity ID: %d", gSelectedEntityState.Entity->entity_type);
  ImGui::Text("Entity kind: %d", gSelectedEntityState.Entity->entity_kind);

  if (ImGui::CollapsingHeader("Position, hitbox, etc.")) {
    ImGui::InputFloat("Entity x", &gSelectedEntityState.Entity->x);
    ImGui::InputFloat("Entity y", &gSelectedEntityState.Entity->y);
    ImGui::SliderFloat("width", &gSelectedEntityState.Entity->width, 0.0, 10.0);
    ImGui::SliderFloat("height", &gSelectedEntityState.Entity->height, 0.0,
                       10.0);
    ImGui::SliderFloat("current_z", &gSelectedEntityState.Entity->current_z,
                       0.0, 50.0);
    ImGui::SliderFloat("original_z", &gSelectedEntityState.Entity->original_z,
                       0.0, 50.0);
    ImGui::SliderFloat("alpha", &gSelectedEntityState.Entity->alpha, 0.0, 1.0);
    ImGui::SliderFloat("hitbox up", &gSelectedEntityState.Entity->hitbox_up,
                       0.0, 5.0);
    ImGui::SliderFloat("hitbox down", &gSelectedEntityState.Entity->hitbox_down,
                       0.0, 5.0);
    ImGui::SliderFloat("hitbox x", &gSelectedEntityState.Entity->hitbox_x, 0.0,
                       5.0);
    ImGui::SliderAngle("angle", &gSelectedEntityState.Entity->angle);
  }
  if (ImGui::CollapsingHeader("Flags1")) {
    drawCharBool("flag_deletion", gSelectedEntityState.Entity->flag_deletion);
    drawCharBool("flag_horizontal_flip",
                 gSelectedEntityState.Entity->flag_horizontal_flip);
    drawCharBool("flag_3", gSelectedEntityState.Entity->flag_3);
    drawCharBool("flag_4", gSelectedEntityState.Entity->flag_4);
    drawCharBool("flag_5", gSelectedEntityState.Entity->flag_5);
    drawCharBool("flag_6", gSelectedEntityState.Entity->flag_6);
    drawCharBool("flag_7", gSelectedEntityState.Entity->flag_7);
    drawCharBool("flag_8", gSelectedEntityState.Entity->flag_8);
    drawCharBool("flag_9", gSelectedEntityState.Entity->flag_9);
    drawCharBool("flag_10", gSelectedEntityState.Entity->flag_10);
    drawCharBool("flag_11", gSelectedEntityState.Entity->flag_11);
    drawCharBool("flag_12", gSelectedEntityState.Entity->flag_12);
    drawCharBool("flag_13", gSelectedEntityState.Entity->flag_13);
    drawCharBool("flag_14", gSelectedEntityState.Entity->flag_14);
    drawCharBool("flag_15", gSelectedEntityState.Entity->flag_15);
    drawCharBool("flag_16", gSelectedEntityState.Entity->flag_16);
    drawCharBool("flag_17", gSelectedEntityState.Entity->flag_17);
    drawCharBool("flag_18", gSelectedEntityState.Entity->flag_18);
    drawCharBool("flag_19", gSelectedEntityState.Entity->flag_19);
    drawCharBool("flag_20", gSelectedEntityState.Entity->flag_20);
    drawCharBool("flag_21", gSelectedEntityState.Entity->flag_21);
    drawCharBool("flag_22", gSelectedEntityState.Entity->flag_22);
    drawCharBool("flag_23", gSelectedEntityState.Entity->flag_23);
    drawCharBool("flag_24", gSelectedEntityState.Entity->flag_24);
  }
  if ((uint32_t)gSelectedEntityState.Entity->entity_kind > 0 &&
      (uint32_t)gSelectedEntityState.Entity->entity_kind < 5 &&
      ImGui::CollapsingHeader("EntityActive Stuff")) {
    auto entityActive =
        reinterpret_cast<EntityActive *>(gSelectedEntityState.Entity);
    ImGui::InputInt("Health", &entityActive->health);
    ImGui::InputInt("Favor given", &entityActive->favor_given);
    ImGui::InputFloat("Velocity x", &entityActive->velocity_x);
    ImGui::InputFloat("Velocity y", &entityActive->velocity_y);
    if (ImGui::CollapsingHeader("Flags2")) {
      for (size_t i = 0x1f0; i <= 0x218; ++i) {
        char *addr = ((char *)gSelectedEntityState.Entity) + i;
        drawCharBool(std::format("Flag {:X}", i).c_str(), *addr);
      }
    }
  }
  if (ImGui::CollapsingHeader("Raw Entity Values")) {
    if (ImGui::BeginTable("Raw Bytes", 6)) {

      ImGui::TableSetupColumn("Offset");
      ImGui::TableSetupColumn("Bytes");
      ImGui::TableSetupColumn("Signed");
      ImGui::TableSetupColumn("Unsigned");
      ImGui::TableSetupColumn("Hex");
      ImGui::TableSetupColumn("Float");
      ImGui::TableHeadersRow();

      for (size_t i = 0;
           i < sizeofEntityKind(gSelectedEntityState.Entity->entity_kind);
           i += 4) {

        ImGui::TableNextRow();

        char *addr = ((char *)gSelectedEntityState.Entity) + i;
        ImGui::TableNextColumn();
        ImGui::Text("0x%X", i);
        {
          uint32_t a1, a2, a3, a4;
          a1 = (*(addr)) & (0xFF);
          a2 = (*(addr + 1)) & (0xFF);
          a3 = (*(addr + 2)) & (0xFF);
          a4 = (*(addr + 3)) & (0xFF);
          ImGui::TableNextColumn();
          ImGui::Text("%X %X %X %X", a1, a2, a3, a4);
        }
        ImGui::TableNextColumn();
        ImGui::Text("%d", *(int32_t *)addr);

        ImGui::TableNextColumn();
        ImGui::Text("%u", *(uint32_t *)addr);

        ImGui::TableNextColumn();
        ImGui::Text("0x%X", *(uint32_t *)addr);

        ImGui::TableNextColumn();
        ImGui::Text("%f", *(float *)addr);
      }

      ImGui::EndTable();
    }
  }
  if (gSelectedEntityState.Entity->flag_deletion == 1) {
    gSelectedEntityState.Entity = NULL;
  }
}

void drawSettingsTab() {}

void drawToolWindow() {
  if (!ui::open) {
    return;
  }
  ImGuiIO &io = ImGui::GetIO();

  ImGui::SetNextWindowPos(ImVec2{0.f, 0.f}, ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2{400.f, 500.f}, ImGuiCond_FirstUseEver);
  ImGui::Begin("Specs HD");

  if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)) {
    io.MouseDrawCursor = true;
    io.WantCaptureMouse = true;
  } else {
    io.MouseDrawCursor = false;
    io.WantCaptureMouse = false;
  }

  auto mouse_game = screenToGame(io.MousePos);

  if (ImGui::BeginTabBar("Specs HD")) {
    if (ImGui::BeginTabItem("Spawn")) {
      drawSpawnTab();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Level")) {
      drawLevelTab();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Players")) {
      drawPlayersTab();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Audio")) {
      drawAudioTab();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Debug")) {
      drawDebugTab();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Selected")) {
      drawSelectedEntityTab();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Settings")) {
      drawSettingsTab();
      ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
  }
}

void handleKeyInput() {
  auto keys = gConfig->keys;

  if (Specs::IsKeyPressed(keys[Specs::KeyFeatures_Hide])) {
    ui::open = !ui::open;
  }

  if (Specs::IsKeyPressed(keys[Specs::KeyFeatures_Engine_Pause])) {
    gPaused = !gPaused;
    gPauseAt = gFrame;
  }

  if (gPaused && gFrame > gPauseAt) {
    if (Specs::IsKeyPressed(keys[Specs::KeyFeatures_Engine_Frame_Advance],
                            true)) {
      gGlobalState->pause_update = 0;
      gPauseAt = gFrame + 1;
    } else {
      gGlobalState->pause_update = 1;
    }
  } else {
    gGlobalState->pause_update = 0;
  }
}

void specsOnFrame() {

  gCameraState =
      reinterpret_cast<CameraState *>(*((DWORD *)(gBaseAddress + 0x154510)));
  gGlobalState =
      reinterpret_cast<GlobalState *>(*((DWORD *)(gBaseAddress + 0x15446C)));

  gWindowedMode = static_cast<int>(*((DWORD *)(gBaseAddress + 0x15a52c)));
  gDisplayWidth = static_cast<int>(*((DWORD *)(gBaseAddress + 0x140a8c)));
  gDisplayHeight = static_cast<int>(*((DWORD *)(gBaseAddress + 0x140a90)));

  gGlobalState->N00001004 = 0; // 440629
  gFrame++;

  handleKeyInput();
  ensureLockedAmounts();
  drawOverlayWindow();
  drawToolWindow();
}