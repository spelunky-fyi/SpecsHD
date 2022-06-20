
#include <Windows.h>
#include <format>
#include <unordered_set>

#include "3rdparty/imgui/imgui.h"

#include "hd.h"

// Global States
DWORD gBaseAddress = NULL;
ImDrawList *gOverlayDrawList = NULL;

CameraState *gCameraState = NULL;
GlobalState *gGlobalState = NULL;

bool gEnableTileBorders = false;
bool gEnableBinBorders = false;
bool gEnablePacifistOverlay = false;
bool gEnableActiveEntityIds = false;
bool gEnableFloorEntityIds = false;
bool gEnableFloor2EntityIds = false;
bool gEnableBackgroundEntityIds = false;

bool gEnabledHitboxes = false;

bool gEnabledUnknown1400 = false;
bool gEnabledUnknown5800 = false;
bool gEnabledFlag15 = false;

int gExcludeEntityInput = -1;
std::unordered_set<uint32_t> gExcludedEntities = {};

int gSpawnEntityInput = 0;

void specsOnInit() {
  gBaseAddress = (size_t)GetModuleHandleA(NULL);
  setupOffsets(gBaseAddress);
}

ImVec2 screenToGame(ImVec2 screen) {
  ImGuiIO &io = ImGui::GetIO();
  auto size = io.DisplaySize;

  auto x = (screen.x - (size.x / 2)) * (20 / size.x) + gCameraState->camera_x;
  auto y = (screen.y - (size.y / 2)) * -(20 / size.x) + gCameraState->camera_y;

  return {x, y};
}

ImVec2 gameToScreen(ImVec2 game) {
  ImGuiIO &io = ImGui::GetIO();
  auto size = io.DisplaySize;

  auto x = (game.x - gCameraState->camera_x) / (20 / size.x) + (size.x / 2);
  auto y = (game.y - gCameraState->camera_y) / -(20 / size.x) + (size.y / 2);

  return {x, y};
}

void drawEntityIds(Entity **entities, size_t count) {
  for (size_t idx = 0; idx < count; idx++) {
    auto ent = entities[idx];
    if (!ent) {
      continue;
    }

    if (gExcludedEntities.contains(ent->entity_type)) {
      continue;
    }

    auto screen = gameToScreen({ent->x, ent->y});
    auto out = std::format("{}", ent->entity_type);
    gOverlayDrawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() + 5,
                              ImVec2{screen.x, screen.y}, IM_COL32_WHITE,
                              out.c_str());
  }
}

void drawEntityHitboxes() {
  for (size_t idx = 0; idx < gGlobalState->entities->entities_active_count;
       idx++) {
    auto ent = gGlobalState->entities->entities_active[idx];
    if (!ent) {
      continue;
    }

    auto screen = gameToScreen({ent->x, ent->y});
    ImVec2 topLeft =
        gameToScreen({ent->x - ent->hitbox_x, ent->y + ent->hitbox_up});
    ImVec2 topRight =
        gameToScreen({ent->x + ent->hitbox_x, ent->y + ent->hitbox_up});
    ImVec2 bottomRight =
        gameToScreen({ent->x + ent->hitbox_x, ent->y - ent->hitbox_down});
    ImVec2 bottomLeft =
        gameToScreen({ent->x - ent->hitbox_x, ent->y - ent->hitbox_down});

    gOverlayDrawList->AddQuad(topLeft, topRight, bottomRight, bottomLeft,
                              ImGui::GetColorU32({255.f, 0.0f, 238.0f, 0.7f}),
                              1.f);
  }
}

void drawPacifistOverlay() {
  for (size_t idx = 0; idx < gGlobalState->entities->entities_active_count;
       idx++) {
    auto ent = gGlobalState->entities->entities_active[idx];
    if (!ent) {
      continue;
    }

    if (ent->owner == -99) {
      continue;
    }

    auto screen = gameToScreen({ent->x, ent->y});
    auto out = std::format("{}", ent->owner);
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

void drawOverlayWindow() {
  ImGuiIO &io = ImGui::GetIO();

  if (io.MouseClicked[1]) {
    auto player = gGlobalState->player1;
    if (player) {
      auto pos = screenToGame(io.MousePos);
      player->x = pos.x;
      player->y = pos.y;
    }
  }

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

  gOverlayDrawList->AddText(
      ImGui::GetFont(), ImGui::GetFontSize() + 5, {148.f, 40.f},
      ImGui::GetColorU32({1.0f, 1.0f, 1.0f, 0.05f}), "SpecsHD");

  if (gEnableTileBorders) {
    drawTileBorders();
  }

  if (gEnableBinBorders) {
    drawBinBorders();
  }

  if (gEnablePacifistOverlay) {
    drawPacifistOverlay();
  }

  if (gEnabledHitboxes) {
    drawEntityHitboxes();
  }

  // Active
  if (gEnableActiveEntityIds) {
    drawEntityIds(gGlobalState->entities->entities_active,
                  gGlobalState->entities->entities_active_count);
  }

  // 1400
  if (gEnabledUnknown1400) {
    drawEntityIds(gGlobalState->entities->array_1400,
                  gGlobalState->entities->array_1400_count);
  }
  // 5800
  if (gEnabledUnknown5800) {
    drawEntityIds(gGlobalState->entities->array_5800,
                  gGlobalState->entities->array_5800_count);
  }

  // Flag 15
  if (gEnabledFlag15) {
    drawEntityIds(gGlobalState->entities->entities_active_flag_15,
                  gGlobalState->entities->entities_active_flag_15_count);
  }

  // Floors
  if (gEnableFloorEntityIds) {
    drawEntityIds(gGlobalState->level_state->entity_floors, 4692);
  }
  if (gEnableFloor2EntityIds) {
    drawEntityIds(gGlobalState->level_state->entity_floors2, 4692);
  }

  // Backgrounds
  if (gEnableBackgroundEntityIds) {
    drawEntityIds(gGlobalState->level_state->entity_backgrounds,
                  gGlobalState->level_state->entity_backgrounds_count);
  }

  ImGui::End();
}

void drawToolWindow() {
  ImGuiIO &io = ImGui::GetIO();

  ImGui::SetNextWindowPos(ImVec2{0.f, 0.f}, ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2{400.f, 500.f}, ImGuiCond_FirstUseEver);
  ImGui::Begin("Specs HD");

  if (ImGui::IsWindowHovered()) {
    io.MouseDrawCursor = true;
    io.WantCaptureMouse = true;
  } else {
    io.MouseDrawCursor = false;
    io.WantCaptureMouse = false;
  }

  auto mouse_game = screenToGame(io.MousePos);

  // ImGui::Text("Mouse: %f %f", io.MousePos.x, io.MousePos.y);
  ImGui::Checkbox("Draw Tile Borders", &gEnableTileBorders);
  ImGui::Checkbox("Draw Bin Borders", &gEnableBinBorders);
  ImGui::Checkbox("Draw Owned Entities", &gEnablePacifistOverlay);

  ImGui::Separator();

  ImGui::Checkbox("Draw Hitboxes", &gEnabledHitboxes);

  ImGui::Separator();

  ImGui::Checkbox("Draw Active Entity IDs", &gEnableActiveEntityIds);
  ImGui::Checkbox("Draw Active Flag 15? IDs", &gEnabledFlag15);
  ImGui::Checkbox("Draw Unknown 1400 IDs", &gEnabledUnknown1400);
  ImGui::Checkbox("Draw Unknown 5800 IDs", &gEnabledUnknown5800);
  ImGui::Checkbox("Draw Floor Entity IDs", &gEnableFloorEntityIds);
  ImGui::Checkbox("Draw Floor2 Entity IDs", &gEnableFloor2EntityIds);
  ImGui::Checkbox("Draw Background Entity IDs", &gEnableBackgroundEntityIds);

  ImGui::Separator();

  ImGui::InputInt("Spawn Entity", &gSpawnEntityInput);
  if (ImGui::Button("Spawn")) {
    if (gSpawnEntityInput >= 0) {
      gGlobalState->SpawnEntity(gGlobalState->player1->x,
                                gGlobalState->player1->y, gSpawnEntityInput,
                                true);
      gSpawnEntityInput = 0;
    }
  }

  ImGui::Separator();
  ImGui::InputInt("Exclude Entity", &gExcludeEntityInput);
  if (ImGui::Button("Exclude")) {
    if (gExcludeEntityInput >= 0) {
      gExcludedEntities.insert(gExcludeEntityInput);
      gExcludeEntityInput = 0;
    }
  }

  ImGui::Separator();
  for (auto ent_type : gExcludedEntities) {
    auto label = std::format("Remove {}", ent_type);
    if (ImGui::Button(label.c_str())) {
      gExcludedEntities.erase(ent_type);
    }
  }
}

void specsOnFrame() {

  gCameraState =
      reinterpret_cast<CameraState *>(*((DWORD *)(gBaseAddress + 0x154510)));
  gGlobalState =
      reinterpret_cast<GlobalState *>(*((DWORD *)(gBaseAddress + 0x15446C)));

  gGlobalState->N00001004 = 0; // 440629
  drawOverlayWindow();
  drawToolWindow();
}