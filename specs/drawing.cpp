
#include "drawing.h"

#include "inputs.h"
#include <hddll/entities.h>
#include <hddll/ui.h>
#include <hddll/utils.h>

#include "game_hooks.h"
#include "mods/full_spelunky.h"
#include "mods/seeded_mode.h"
#include "seeded_items.h"
#include "tabs/debug_tab.h"
#include "tabs/selected_tab.h"
#include "tabs/spawn_tab.h"

void drawPointAtCoord(ImVec2 coord, ImU32 color) {
  auto topLeft = hddll::gameToScreen(coord);
  topLeft.x -= 1;
  topLeft.y += 1;

  ImVec2 topRight = {topLeft.x + 2, topLeft.y};
  ImVec2 bottomRight = {topLeft.x + 2, topLeft.y - 2};
  ImVec2 bottomLeft = {topLeft.x, topLeft.y - 2};

  gOverlayDrawList->AddQuad(topLeft, topRight, bottomRight, bottomLeft, color,
                            1.f);
}

void drawEntityCircle(hddll::Entity *ent, float radius, ImU32 color,
                      float x_offset, float y_offset, bool filled) {
  auto screen = hddll::gameToScreen({ent->x + x_offset, ent->y + y_offset});
  auto screenRadius =
      hddll::gameToScreen({ent->x + x_offset + radius, 0}).x - screen.x;

  if (filled) {
    gOverlayDrawList->AddCircleFilled(screen, screenRadius, color, 0);
  } else {
    gOverlayDrawList->AddCircle(screen, screenRadius, color, 0, 1.f);
  }
}

void drawEntityHitbox(hddll::Entity *ent, ImU32 color, bool filled) {

  ImVec2 topLeft =
      hddll::gameToScreen({ent->x - ent->hitbox_x, ent->y + ent->hitbox_up});
  ImVec2 topRight =
      hddll::gameToScreen({ent->x + ent->hitbox_x, ent->y + ent->hitbox_up});
  ImVec2 bottomRight =
      hddll::gameToScreen({ent->x + ent->hitbox_x, ent->y - ent->hitbox_down});
  ImVec2 bottomLeft =
      hddll::gameToScreen({ent->x - ent->hitbox_x, ent->y - ent->hitbox_down});

  if (filled) {
    gOverlayDrawList->AddQuadFilled(topLeft, topRight, bottomRight, bottomLeft,
                                    color);
  } else {
    gOverlayDrawList->AddQuad(topLeft, topRight, bottomRight, bottomLeft, color,
                              1.f);
  }

  if (gDebugState.IncludeHitboxOrigins) {
    auto ent_origin = hddll::gameToScreen({ent->x, ent->y});
    gOverlayDrawList->AddLine(topLeft, ent_origin, color, 1.f);
    gOverlayDrawList->AddLine(topRight, ent_origin, color, 1.f);
    gOverlayDrawList->AddLine(bottomLeft, ent_origin, color, 1.f);
    gOverlayDrawList->AddLine(bottomRight, ent_origin, color, 1.f);
    drawPointAtCoord({ent->x, ent->y}, color);
  }

  if (ent->flag_23 == 1 ||
      ent->entity_kind == hddll::EntityKind::KIND_EXPLOSION) {
    drawEntityCircle(ent, ent->hitbox_down, color, 0.0f, 0.0f, filled);
  }
}

bool drawCharBool(const char *label, char &flag) {
  bool bflag = flag ? 1 : 0;
  bool res = ImGui::Checkbox(label, &bflag);
  bflag ? flag = 1 : flag = 0;
  return res;
}

bool drawCharBool(const char *label, uint8_t &flag) {
  bool bflag = flag ? 1 : 0;
  bool res = ImGui::Checkbox(label, &bflag);
  bflag ? flag = 1 : flag = 0;
  return res;
}

void drawEntityDetectionRay(hddll::Entity *ent, float len, ImU32 color) {

  auto factor = 1.001f;
  auto top = 0.25f;
  if (ent->flag_horizontal_flip) {
    gOverlayDrawList->AddQuad(
        hddll::gameToScreen(
            {ent->x + ent->hitbox_x * factor - len, ent->y - top}),
        hddll::gameToScreen({ent->x + ent->hitbox_x * factor, ent->y - top}),

        hddll::gameToScreen({ent->x + ent->hitbox_x * factor,
                             ent->y - ent->hitbox_down * factor}),

        hddll::gameToScreen({ent->x + ent->hitbox_x * factor - len,
                             ent->y - ent->hitbox_down * factor}),
        color);
  } else {
    gOverlayDrawList->AddQuad(
        hddll::gameToScreen(
            {ent->x - ent->hitbox_x * factor + len, ent->y - top}),
        hddll::gameToScreen({ent->x - ent->hitbox_x * factor, ent->y - top}),
        hddll::gameToScreen({ent->x - ent->hitbox_x * factor,
                             ent->y - ent->hitbox_down * factor}),
        hddll::gameToScreen({ent->x - ent->hitbox_x * factor + len,
                             ent->y - ent->hitbox_down * factor}),
        color);
  }
}
void drawEntityHitboxDefault(hddll::Entity *ent) { drawEntityHitbox(ent); }

void drawPacifistOverlay() {
  for (size_t idx = 0;
       idx < hddll::gGlobalState->entities->entities_active_count; idx++) {
    auto ent = hddll::gGlobalState->entities->entities_active[idx];
    if (!ent) {
      continue;
    }

    if (ent->owner == hddll::Ownership::Unowned) {
      continue;
    }

    auto screen = hddll::gameToScreen({ent->x, ent->y});
    auto out = std::format("{}", (int)ent->owner);
    gOverlayDrawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() + 2.f,
                              ImVec2{screen.x, screen.y}, IM_COL32_WHITE,
                              out.c_str());
  }
}

void drawBinBorders() {
  for (auto x = -4.f; x < 12.f * 4.f; x += 4.f) {
    gOverlayDrawList->AddLine(hddll::gameToScreen({x, -4.f}),
                              hddll::gameToScreen({x, 120.f}), IM_COL32_WHITE);
  }
  for (auto y = -4.f; y < 26.f * 4.f; y += 4.f) {
    gOverlayDrawList->AddLine(hddll::gameToScreen({-4.f, y}),
                              hddll::gameToScreen({50.f, y}), IM_COL32_WHITE);
  }
}

void drawRoomBorders() {
  if (hddll::gGlobalState->screen_state == 0) {

    auto room_width = 10.f;
    auto room_height = 8.f;
    auto max_height = 99.5f;
    auto max_width = 42.5f;

    for (auto x = 2.5f; x < 50.f; x += room_width) {
      gOverlayDrawList->AddLine(hddll::gameToScreen({x, 3.5f}),
                                hddll::gameToScreen({x, max_height}),
                                IM_COL32_WHITE, 2.0f);
    }
    for (auto y = 3.5f; y < 120.f; y += room_height) {
      gOverlayDrawList->AddLine(hddll::gameToScreen({2.5, y}),
                                hddll::gameToScreen({max_width, y}),
                                IM_COL32_WHITE, 2.0f);
    }

    for (auto idx = 0; idx < 48; idx++) {
      auto column = idx % 4;
      auto row = idx / 4;
      auto type = hddll::gGlobalState->level_state->room_types[idx];

      auto y = max_height - (row * room_height);
      auto x = 2.5f + (column * room_width);

      auto out = std::format("Type: {}", type);
      auto screen = hddll::gameToScreen({x + 0.25f, y - 0.25f});
      gOverlayDrawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() + 8,
                                ImVec2{screen.x, screen.y}, IM_COL32_WHITE,
                                out.c_str());
    }
  }
}

void drawTileBorders() {
  for (auto x = -4.f; x < 12.f * 4.f; x += 1.f) {
    gOverlayDrawList->AddLine(hddll::gameToScreen({x + -.5f, -4.5f}),
                              hddll::gameToScreen({x + -.5f, 120.5f}),
                              IM_COL32_WHITE, 0.5f);
  }
  for (auto y = -4.f; y < 26.f * 4.f; y += 1.f) {
    gOverlayDrawList->AddLine(hddll::gameToScreen({-4.5f, y + 0.5f}),
                              hddll::gameToScreen({50.5f, y + 0.5f}),
                              IM_COL32_WHITE, 0.5f);
  }
}

void drawEntityId(hddll::Entity *ent) {
  auto screen = hddll::gameToScreen({ent->x, ent->y});
  auto out = std::format("{}", ent->entity_type);
  gOverlayDrawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() + 5,
                            ImVec2{screen.x, screen.y}, IM_COL32_WHITE,
                            out.c_str());
}

void drawEntityOffsetDebug(hddll::Entity *ent) {

  auto it = gDebugState.DrawEntityOffsets.find(
      std::pair{ent->entity_kind, ent->entity_type});
  if (it == gDebugState.DrawEntityOffsets.end() || it->second.empty()) {
    return;
  }
  auto &entry = it->second;

  auto idx = 0;
  auto font = ImGui::GetFont();
  auto fontSize = ImGui::GetFontSize() + 5;
  auto screen =
      hddll::gameToScreen({ent->x - ent->hitbox_x, ent->y + ent->hitbox_up});
  screen.y -= fontSize + 8;

  for (auto offset_pair : entry) {

    auto offset = offset_pair.first;
    auto dataType = offset_pair.second;

    char *addr = ((char *)ent) + offset;
    auto text = std::format("0x{:X}: ", offset);

    if (dataType == DataType_Byte) {
      text.append(std::format("{:02X}", *addr));
    } else if (dataType == DataType_Dword_Unsigned) {
      text.append(std::format("{:d}", *(uint32_t *)addr));
    } else if (dataType == DataType_Dword_Signed) {
      text.append(std::format("{:d}", *(int32_t *)addr));
    } else if (dataType == DataType_Dword_Hex) {
      text.append(std::format("0x{:08X}", *(uint32_t *)addr));
    } else if (dataType == DataType_Float) {
      text.append(std::format("{:f}", *(float *)addr));
    }

    gOverlayDrawList->AddText(
        font, fontSize, ImVec2{screen.x, screen.y - (idx * (fontSize + 2))},
        IM_COL32_WHITE, text.c_str());
    idx++;
  }
}

void forEntities(const std::unordered_set<uint32_t> &excludedEntities,
                 EntityCallback callback, hddll::Entity **entities,
                 size_t count, bool decos) {
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

bool findEntityArray(hddll::Entity *searchEnt, hddll::Entity **entities,
                     size_t count, bool decos) {
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

bool findEntity(hddll::Entity *searchEnt) {
  if (searchEnt == NULL) {
    return false;
  }
  return (
      findEntityArray(searchEnt, hddll::gGlobalState->entities->entities_active,
                      hddll::gGlobalState->entities->entities_active_count) ||
      findEntityArray(searchEnt, hddll::gGlobalState->entities->array_1400,
                      hddll::gGlobalState->entities->array_1400_count) ||
      findEntityArray(
          searchEnt, hddll::gGlobalState->entities->entities_foreground,
          hddll::gGlobalState->entities->array_entities_foreground_count) ||
      findEntityArray(
          searchEnt, hddll::gGlobalState->entities->entities_light_emitting,
          hddll::gGlobalState->entities->entities_light_emitting_count) ||
      findEntityArray(
          searchEnt,
          (hddll::Entity **)hddll::gGlobalState->level_state->entity_floors,
          hddll::ENTITY_FLOORS_COUNT, gDebugState.IncludeFloorDecos) ||

      findEntityArray(
          searchEnt,
          (hddll::Entity **)hddll::gGlobalState->level_state->entity_floors_bg,
          hddll::ENTITY_FLOORS_COUNT) ||

      findEntityArray(
          searchEnt,
          (hddll::Entity **)
              hddll::gGlobalState->level_state->entity_backgrounds,
          hddll::gGlobalState->level_state->entity_backgrounds_count) ||
      findEntityArray(searchEnt, hddll::gGlobalState->_4cstruct->entities,
                      160));
}

void forEnabledEntities(EnabledEntities &enabledEnts, EntityCallback callback) {

  // Active
  if (enabledEnts.activeEntities) {
    forEntities(enabledEnts.excluded, callback,
                hddll::gGlobalState->entities->entities_active,
                hddll::gGlobalState->entities->entities_active_count);
  }

  // 1400
  if (enabledEnts.unknown1400) {
    forEntities(enabledEnts.excluded, callback,
                hddll::gGlobalState->entities->array_1400,
                hddll::gGlobalState->entities->array_1400_count);
  }
  // Foreground
  if (enabledEnts.foregroundEntities) {
    forEntities(enabledEnts.excluded, callback,
                hddll::gGlobalState->entities->entities_foreground,
                hddll::gGlobalState->entities->array_entities_foreground_count);
  }

  // Light Emitting
  if (enabledEnts.lightEmittingEntities) {
    forEntities(enabledEnts.excluded, callback,
                hddll::gGlobalState->entities->entities_light_emitting,
                hddll::gGlobalState->entities->entities_light_emitting_count);
  }

  // Floors
  if (enabledEnts.floorEntities) {
    forEntities(
        enabledEnts.excluded, callback,
        (hddll::Entity **)hddll::gGlobalState->level_state->entity_floors,
        hddll::ENTITY_FLOORS_COUNT, gDebugState.IncludeFloorDecos);
  }
  if (enabledEnts.floorBgEntities) {
    forEntities(
        enabledEnts.excluded, callback,
        (hddll::Entity **)hddll::gGlobalState->level_state->entity_floors_bg,
        hddll::ENTITY_FLOORS_COUNT);
  }

  // Backgrounds
  if (enabledEnts.backgroundEntities) {
    forEntities(
        enabledEnts.excluded, callback,
        (hddll::Entity **)hddll::gGlobalState->level_state->entity_backgrounds,
        hddll::gGlobalState->level_state->entity_backgrounds_count);
  }

  if (enabledEnts._4cStructEntities) {
    forEntities(enabledEnts.excluded, callback,
                hddll::gGlobalState->_4cstruct->entities, 160);
  }
}

float dist(ImVec2 pos1, ImVec2 pos2) {
  return sqrt(pow(pos2.x - pos1.x, 2.0f) + pow(pos2.y - pos1.y, 2.0f));
}

bool entityCollidesWithCircle(hddll::Entity *targetEnt, float x_pos,
                              float y_pos, float radius) {
  auto target_edge_x_left = targetEnt->x - targetEnt->hitbox_x;
  auto target_edge_x_right = targetEnt->x + targetEnt->hitbox_x;
  auto target_x = target_edge_x_left;

  auto target_edge_y_top = targetEnt->y + targetEnt->hitbox_up;
  auto target_edge_y_bottom = targetEnt->y - targetEnt->hitbox_down;
  auto target_y = target_edge_y_top;

  auto source_x = x_pos;
  auto source_y = y_pos;

  if (target_x <= source_x) {
    target_x = target_edge_x_right;
    if (source_x <= target_edge_x_right) {
      target_x = source_x;
    }
  }

  if (source_y <= target_y) {
    target_y = target_edge_y_bottom;
    if (target_edge_y_bottom <= source_y) {
      target_y = source_y;
    }
  }

  auto distance =
      pow((target_x - source_x), 2.0) + pow((target_y - source_y), 2.0);

  return distance < radius;
}

bool collidesWithEntityCircle(hddll::Entity *sourceEnt,
                              hddll::Entity *targetEnt, float x_offset,
                              float y_offset) {

  return entityCollidesWithCircle(targetEnt, sourceEnt->x + x_offset,
                                  sourceEnt->y + y_offset,
                                  sourceEnt->hitbox_down);
}

size_t sizeofEntityKind(hddll::EntityKind entityKind) {
  switch (entityKind) {
  case hddll::EntityKind::KIND_FLOOR:
    return sizeof(hddll::EntityFloor);
  case hddll::EntityKind::KIND_ACTIVE:
    return sizeof(hddll::EntityActive);
  case hddll::EntityKind::KIND_PLAYER:
    return sizeof(hddll::EntityPlayer);
  case hddll::EntityKind::KIND_MONSTER:
    return sizeof(hddll::EntityMonster);
  case hddll::EntityKind::KIND_ITEM:
    return sizeof(hddll::EntityItem);
  case hddll::EntityKind::KIND_BACKGROUND:
    return sizeof(hddll::EntityBackground);
  case hddll::EntityKind::KIND_EXPLOSION:
    return sizeof(hddll::EntityExplosion);
  case hddll::EntityKind::KIND_ENTITY:
    return sizeof(hddll::Entity);
  default:
    return 0;
  }
}

static void handleTeleportInput() {
  ImGuiIO &io = ImGui::GetIO();
  if (Specs::IsMouseClicked(gConfig->buttons[Specs::MouseFeatures_Teleport])) {
    auto player = hddll::gGlobalState->player1;
    if (player) {
      auto pos = hddll::screenToGame(io.MousePos);
      player->x = pos.x;
      player->y = pos.y;
    }
  }
}

static void handleSpawnInput() {
  ImGuiIO &io = ImGui::GetIO();
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

    for (auto const &spawnEntityConfig : gSpawnState.SpawnEntityInputs) {
      if (spawnEntityConfig.entityType >= 0) {
        if (io.MouseDownDurationPrev[spawnMouseConfig.Button] > 0.1f) {
          auto gamePos = hddll::screenToGame(gSpawnState.ClickedAt);
          hddll::Entity *ent;

          if (spawnEntityConfig.entityType == 0) {
            ent = hddll::gGlobalState->SpawnHiredHand(gamePos.x, gamePos.y, 90);
          } else {
            ent = hddll::gGlobalState->SpawnEntity(
                gamePos.x, gamePos.y, spawnEntityConfig.entityType,
                spawnEntityConfig.activeEntity);
          }

          if (ent && (uint32_t)ent->entity_kind > 0 &&
              (uint32_t)ent->entity_kind < 5) {

            auto activeEnt = (hddll::EntityActive *)ent;
            if (ent->entity_type != 108) {
              activeEnt->velocity_x =
                  (io.MousePos.x - gSpawnState.ClickedAt.x) * 0.01f;
            }
            activeEnt->velocity_y =
                -((io.MousePos.y - gSpawnState.ClickedAt.y) * 0.01f);
          }
        } else {
          auto gamePos = hddll::screenToGame(io.MousePos);
          if (spawnEntityConfig.entityType == 0) {
            hddll::gGlobalState->SpawnHiredHand(gamePos.x, gamePos.y, 90);
          } else {
            hddll::gGlobalState->SpawnEntity(gamePos.x, gamePos.y,
                                             spawnEntityConfig.entityType,
                                             spawnEntityConfig.activeEntity);
          }
        }
      }
    }
  }
}

static hddll::Entity *handleSelectInput() {
  ImGuiIO &io = ImGui::GetIO();
  hddll::Entity *closestEnt = NULL;

  auto selectEntMouseConfig =
      gConfig->buttons[Specs::MouseFeatures_SelectEntity];
  auto selectEntClicked = Specs::IsMouseClicked(selectEntMouseConfig);

  if (gSelectedEntityState.Clicking &&
      io.MouseReleased[selectEntMouseConfig.Button]) {
    gSelectedEntityState.Clicking = false;
  }

  auto gamePos = hddll::screenToGame(io.MousePos);
  if (gDebugState.DrawClosestEntHitbox || gDebugState.DrawClosestEntId ||
      selectEntClicked) {
    float closestEntDist = 1;
    if (selectEntClicked) {
      gSelectedEntityState.Entity = NULL;
      gSelectedEntityState.Clicking = true;
    }
    EntityCallback getClosestEnt = [&](hddll::Entity *e) {
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
      auto ent = (hddll::EntityActive *)gSelectedEntityState.Entity;
      ent->time_in_air = 0.f;
    }
  }

  return closestEnt;
}

static void drawOlmecCrushProbes() {
  for (size_t idx = 0;
       idx < hddll::gGlobalState->entities->entities_active_count; idx++) {
    auto ent = (hddll::EntityActive *)
                   hddll::gGlobalState->entities->entities_active[idx];

    if (!ent || ent->entity_type != 1055) {
      continue;
    }

    auto probe_idx = 0.0f;
    do {
      auto x = ent->x - ent->hitbox_x + probe_idx * 0.5f;
      auto y = ent->y - ent->hitbox_down - 0.5f;

      drawPointAtCoord({x, y});

      probe_idx++;
    } while (probe_idx < 8);
  }
}

static void drawBlackMarketTrainer() {
  if (hddll::gGlobalState->level > 4 && hddll::gGlobalState->level < 9 &&
      hddll::gGlobalState->is_worm == 0 &&
      hddll::gGlobalState->is_blackmarket == 0 &&
      hddll::gGlobalState->level_state->alt_exit_x > 0 &&
      hddll::gGlobalState->level_state->alt_exit_y > 0) {

    auto color = ImGui::GetColorU32({255.f, 0.0f, 0.0f, 0.25f});
    auto replace_color = ImGui::GetColorU32({255.0f, 255.0f, 0.0f, 0.25f});
    auto bm_color = ImGui::GetColorU32({0.f, 255.0f, 0.0f, 0.25f});

    if (hddll::gGlobalState->player1) {
      drawEntityCircle(hddll::gGlobalState->player1, 10.f, bm_color);
    }

    for (auto e_idx = 0; e_idx < ELIGIBLE_FLOORS_FOR_BM_COUNT; e_idx++) {

      auto idx = ELIGIBLE_FLOORS_FOR_BM[e_idx];
      auto floor = hddll::gGlobalState->level_state->entity_floors[idx];
      if (!floor) {
        continue;
      }

      // BM, draw green
      if (floor->x == hddll::gGlobalState->level_state->alt_exit_x &&
          floor->y == hddll::gGlobalState->level_state->alt_exit_y) {
        drawEntityHitbox(floor, bm_color, true);
        continue;
      }

      // Check for replaced entities
      if (floor->entity_type != 0x2389 && floor->entity_type != 0x2387) {
        drawEntityHitbox(floor, replace_color, true);
        continue;
      }

      drawEntityHitbox(floor, color, true);
    }
  }
}

static void drawHHLinks() {
  for (size_t idx = 0;
       idx < hddll::gGlobalState->entities->entities_active_count; idx++) {
    auto ent = (hddll::EntityActive *)
                   hddll::gGlobalState->entities->entities_active[idx];

    if (!ent)
      continue;
    if (ent->entity_kind != hddll::EntityKind::KIND_PLAYER)
      continue;

    auto player = (hddll::EntityPlayer *)ent;

    if (gDebugState.DrawHHFollowerLink && player->follower) {
      auto follower_color = ImGui::GetColorU32({255.f, 0.0f, 0.0f, 0.9f});
      auto start = hddll::gameToScreen({player->x, player->y});
      auto end =
          hddll::gameToScreen({player->follower->x, player->follower->y});
      start.y += 4;
      end.y += 4;
      gOverlayDrawList->AddLine(start, end, follower_color, 1.f);
    }

    if (gDebugState.DrawHHFollowingLink && player->following) {
      auto following_color = ImGui::GetColorU32({0.f, 255.0f, 0.0f, 0.9f});
      auto start = hddll::gameToScreen({player->x, player->y});
      auto end =
          hddll::gameToScreen({player->following->x, player->following->y});
      start.y -= 4;
      end.y -= 4;
      gOverlayDrawList->AddLine(start, end, following_color, 1.f);
    }
  }
}

static void drawEnemyDetection() {
  auto color = ImGui::GetColorU32({0.0f, 1.0f, .5f, .9f});
  auto wallColor = ImGui::GetColorU32({0.9f, 0.9f, 0.0f, .9f});
  auto waterColor = ImGui::GetColorU32({0.0f, 0.0f, 1.0f, .9f});
  auto bombColor = ImGui::GetColorU32({255.f, 0.0f, 0.0f, 0.25f});

  if (hddll::gGlobalState->player1) {
    drawPointAtCoord(
        {hddll::gGlobalState->player1->x, hddll::gGlobalState->player1->y});
  }
  for (size_t idx = 0;
       idx < hddll::gGlobalState->entities->entities_active_count; idx++) {
    auto ent = (hddll::EntityActive *)
                   hddll::gGlobalState->entities->entities_active[idx];

    if (ent) {
      auto entityRoom =
          hddll::gGlobalState->level_state
              ->room_types[hddll::GetRoomForPosition(ent->x, ent->y)];
      if (ent->entity_type == 1002) { // Spider
        gOverlayDrawList->AddQuad(
            hddll::gameToScreen({ent->x - 0.4f, ent->y}),
            hddll::gameToScreen({ent->x + 0.4f, ent->y}),
            hddll::gameToScreen({ent->x + 0.4f, ent->y - 7.0f}),
            hddll::gameToScreen({ent->x - 0.4f, ent->y - 7.0f}), color);
      } else if (ent->entity_type == 1003) { // Bat
        gOverlayDrawList->PathArcToFast(
            hddll::gameToScreen({ent->x, ent->y - 0.2f}),
            hddll::gameToScreen({ent->x + 6.0f, 0.f}).x -
                hddll::gameToScreen({ent->x, 0.f}).x,
            0, 6);
        gOverlayDrawList->PathStroke(color, ImDrawFlags_Closed, 1.0f);
      } else if ( // Caveman / Hawk Man / Croc Man / Green Knight /
                  // Scorpion / Tiki
          ent->entity_type == 1004 || ent->entity_type == 1011 ||
          ent->entity_type == 1044 || ent->entity_type == 1045 ||
          ent->entity_type == 1029 || ent->entity_type == 1041) {
        drawEntityDetectionRay(ent, 6.0f, wallColor);
      } else if (ent->entity_type == 1042) { // Scorpion Fly
        drawEntityDetectionRay(ent, 6.0f, wallColor);
        drawEntityCircle(ent, 6.f, color);
      } else if (ent->entity_type == 1006) { // Shopkeeper
        if (ent->field8_0x14c == 0 || ent->field8_0x14c == 1) {
          drawEntityCircle(ent, 2.f, color);
          drawEntityCircle(ent, 1.f, color);
        }
        if (ent->field8_0x14c == 7 || ent->field8_0x14c == 8) {
          drawEntityCircle(ent, 6.f, color);
        }
        drawEntityCircle(ent, 20.f, color);
        if (ent->field8_0x14c == 5) {
          drawEntityCircle(ent, 2.f, color);
          drawEntityCircle(ent, 12.f, color);
          if (ent->flag_horizontal_flip) {
            gOverlayDrawList->AddQuad(
                hddll::gameToScreen({ent->x - 5.0f, ent->y + 12.0f}),
                hddll::gameToScreen({ent->x, ent->y + 12.0f}),
                hddll::gameToScreen({ent->x, ent->y - 12.0f}),
                hddll::gameToScreen({ent->x - 5.0f, ent->y - 12.0f}), color);
          } else {
            gOverlayDrawList->AddQuad(
                hddll::gameToScreen({ent->x, ent->y + 12.0f}),
                hddll::gameToScreen({ent->x + 5.0f, ent->y + 12.0f}),
                hddll::gameToScreen({ent->x + 5.0f, ent->y - 12.0f}),
                hddll::gameToScreen({ent->x, ent->y - 12.0f}), color);
          }
        }
      } else if (ent->entity_type == 1007 ||
                 ent->entity_type == 1021) { // Blue / Orange Frog
        drawEntityCircle(ent, 8.f, color);
      } else if (ent->entity_type == 1010) { // UFO
        gOverlayDrawList->AddLine(hddll::gameToScreen({ent->x - 8.0f, ent->y}),
                                  hddll::gameToScreen({ent->x + 8.0f, ent->y}),
                                  color);
        gOverlayDrawList->AddLine(
            hddll::gameToScreen({ent->x - 5.25f, ent->y - 6.0f}),
            hddll::gameToScreen({ent->x + 5.25f, ent->y - 6.0f}), color);
        drawEntityCircle(ent, 8.f, color);
        gOverlayDrawList->AddQuad(
            hddll::gameToScreen({ent->x - 0.5f, ent->y}),
            hddll::gameToScreen({ent->x + 0.5f, ent->y}),
            hddll::gameToScreen({ent->x + 0.5f, ent->y - 8.0f}),
            hddll::gameToScreen({ent->x - 0.5f, ent->y - 8.0f}), color);
      } else if (ent->entity_type == 1012) { // Skeleton
        gOverlayDrawList->PathArcToFast(
            hddll::gameToScreen({ent->x, ent->y}),
            hddll::gameToScreen({ent->x + 4.0f, 0.f}).x -
                hddll::gameToScreen({ent->x, 0.f}).x,
            0, 6);
        gOverlayDrawList->PathStroke(color, ImDrawFlags_Closed, 1.0f);
      } else if (ent->entity_type == 1013) { // Piranha
        drawEntityCircle(ent, 6.f, waterColor);
      } else if (ent->entity_type == 1023) { // Old Bitey
        drawEntityCircle(ent, 5.f, waterColor);
      } else if (ent->entity_type == 1014) { // Mummy
        gOverlayDrawList->AddLine(
            hddll::gameToScreen({ent->x - 5.2f, ent->y - 3.0f}),
            hddll::gameToScreen({ent->x + 5.2f, ent->y - 3.0f}), color);
        gOverlayDrawList->AddLine(
            hddll::gameToScreen({ent->x - 5.2f, ent->y + 3.0f}),
            hddll::gameToScreen({ent->x + 5.2f, ent->y + 3.0f}), color);
        drawEntityCircle(ent, 6.f, color);
      } else if (ent->entity_type == 1015) { // Monkey
        if (ent->field71_0x203 != 0) {
          gOverlayDrawList->PathArcToFast(
              hddll::gameToScreen({ent->x, ent->y}),
              hddll::gameToScreen({ent->x + 4.0f, 0.f}).x -
                  hddll::gameToScreen({ent->x, 0.f}).x,
              0, 6);
          gOverlayDrawList->PathStroke(color, ImDrawFlags_Closed, 1.0f);
        } else {
          drawEntityCircle(ent, 4.f, color);
        }
      } else if (ent->entity_type == 1020 ||
                 ent->entity_type == 1028) { // Vampire or Vlad
        if (entityRoom < 0x13 || entityRoom > 0x15) {
          gOverlayDrawList->PathArcToFast(
              hddll::gameToScreen({ent->x, ent->y - 0.2f}),
              hddll::gameToScreen({ent->x + 6.0f, 0.f}).x -
                  hddll::gameToScreen({ent->x, 0.f}).x,
              0, 6);
          gOverlayDrawList->PathStroke(color, ImDrawFlags_Closed, 1.0f);
        } else {
          gOverlayDrawList->AddQuad(
              hddll::gameToScreen({ent->x - 1.0f, ent->y - 0.2f}),
              hddll::gameToScreen({ent->x + 1.0f, ent->y - 0.2f}),
              hddll::gameToScreen({ent->x + 1.0f, ent->y - 6.0f}),
              hddll::gameToScreen({ent->x - 1.0f, ent->y - 6.0f}), color);
        }
      } else if (ent->entity_type == 1018) { // Giant Spider
        gOverlayDrawList->AddQuad(
            hddll::gameToScreen({ent->x - 1.0f, ent->y}),
            hddll::gameToScreen({ent->x + 1.0f, ent->y}),
            hddll::gameToScreen({ent->x + 1.0f, ent->y - 8.0f}),
            hddll::gameToScreen({ent->x - 1.0f, ent->y - 8.0f}), color);
      } else if (ent->entity_type == 1019) { // Jiang Shi
        drawEntityCircle(ent, 8.f, color);
      } else if (ent->entity_type == 1024) { // Scarab
        drawEntityCircle(ent, 6.f, color);
        gOverlayDrawList->AddLine(
            hddll::gameToScreen({ent->x - 5.95f, ent->y - 0.2f}),
            hddll::gameToScreen({ent->x + 5.95f, ent->y - 0.2f}), color);
      } else if (ent->entity_type == 1046) { // Worm Egg
        drawEntityCircle(ent, 6.f, color);
        gOverlayDrawList->AddLine(
            hddll::gameToScreen({ent->x - 5.6f, ent->y + 2.f}),
            hddll::gameToScreen({ent->x + 5.6f, ent->y + 2.f}), color);
      } else if (ent->entity_type == 1025) { // Yeti King
        drawEntityCircle(ent, 6.f, color);
      } else if (ent->entity_type == 1030) { // Imp
        gOverlayDrawList->AddQuad(
            hddll::gameToScreen({ent->x - 1.0f, ent->y}),
            hddll::gameToScreen({ent->x + 1.0f, ent->y}),
            hddll::gameToScreen({ent->x + 1.0f, ent->y - 8.0f}),
            hddll::gameToScreen({ent->x - 1.0f, ent->y - 8.0f}), color);
      } else if (ent->entity_type == 1038) { // Giant Frog
        drawEntityCircle(ent, 6.f, color);
      } else if (ent->entity_type == 1031) { // Blue Devil
        drawEntityDetectionRay(ent, 6.0f, wallColor);
        gOverlayDrawList->AddQuad(
            hddll::gameToScreen(
                {ent->x - ent->hitbox_x, ent->y + ent->hitbox_up}),
            hddll::gameToScreen(
                {ent->x - ent->hitbox_x, ent->y + ent->hitbox_up + 5.0f}),
            hddll::gameToScreen(
                {ent->x + ent->hitbox_x, ent->y + ent->hitbox_up + 5.0f}),
            hddll::gameToScreen(
                {ent->x + ent->hitbox_x, ent->y + ent->hitbox_up}),
            wallColor);
      } else if (ent->entity_type == 1033) { // Anubis
        drawEntityCircle(ent, 11.f, color);
        drawEntityCircle(ent, 8.f, color);
        drawEntityCircle(ent, 4.f, color);
      } else if (ent->entity_type == 1040) { // Alien Tank
        drawEntityCircle(ent, 6.f, color);
        gOverlayDrawList->AddLine(hddll::gameToScreen({ent->x, ent->y - 2.0f}),
                                  hddll::gameToScreen({ent->x, ent->y + 6.0f}),
                                  color);
        if (ent->flag_horizontal_flip) {
          gOverlayDrawList->AddLine(
              hddll::gameToScreen({ent->x - 5.6f, ent->y - 2.0f}),
              hddll::gameToScreen({ent->x, ent->y - 2.0f}), color);
        } else {
          gOverlayDrawList->AddLine(
              hddll::gameToScreen({ent->x, ent->y - 2.0f}),
              hddll::gameToScreen({ent->x + 5.6f, ent->y - 2.0f}), color);
        }
      } else if (ent->entity_type == 1016) { // Alien Lord
        drawEntityCircle(ent, 8.f, color);
      } else if (ent->entity_type == 1048) { // Alien Queen
        drawEntityCircle(ent, 12.f, color);
      } else if (ent->entity_type == 1051) { // Succubus
        drawEntityCircle(ent, 4.f, color);
        drawEntityDetectionRay(ent, 6.0f, wallColor);
      } else if (ent->entity_type == 1052 ||
                 ent->entity_type == 1053) { // Horse Head / Ox Face
        drawEntityCircle(ent, 4.f, color);
      } else if (ent->entity_type == 1054) { // Anubis 2
        drawEntityCircle(ent, 8.f, color);
      } else if (ent->entity_type == 1055) { // Olmec
        drawEntityCircle(ent, 10.f, color);
      } else if (ent->entity_type == 1056) { // Yama Head
        if (ent->field75_0x207 == 0) {
          drawEntityCircle(ent, 6.f, color);
        } else {
          gOverlayDrawList->AddQuad(
              hddll::gameToScreen({ent->x - 1.0f, 200.0}),
              hddll::gameToScreen({ent->x + 1.0f, 200.0}),
              hddll::gameToScreen({ent->x + 1.0f, ent->y}),
              hddll::gameToScreen({ent->x - 1.0f, ent->y}), color, 1.f);
          gOverlayDrawList->AddQuad(hddll::gameToScreen({14.0, 110.0}),
                                    hddll::gameToScreen({31.0, 110.0}),
                                    hddll::gameToScreen({31.0, 88.0}),
                                    hddll::gameToScreen({14.0, 88.0}), color,
                                    1.f);
          gOverlayDrawList->AddQuad(hddll::gameToScreen({-10.0, 110.0}),
                                    hddll::gameToScreen({14.0, 110.0}),
                                    hddll::gameToScreen({14.0, 93.0}),
                                    hddll::gameToScreen({-10.0, 93.0}), color,
                                    1.f);
          gOverlayDrawList->AddQuad(hddll::gameToScreen({31.0, 110.0}),
                                    hddll::gameToScreen({60.0, 110.0}),
                                    hddll::gameToScreen({60.0, 93.0}),
                                    hddll::gameToScreen({31.0, 93.0}), color,
                                    1.f);
        }
      } else if (ent->entity_type == 1057) { // Yama Hand
        drawEntityCircle(ent, 6.f, color);
        gOverlayDrawList->AddLine(hddll::gameToScreen({-10.0f, ent->y - 2.0f}),
                                  hddll::gameToScreen({60.0f, ent->y - 2.0f}),
                                  color);
      } else if (ent->entity_type == 1058) { // Turret
        drawEntityCircle(ent, 6.f, color);
        gOverlayDrawList->AddLine(hddll::gameToScreen({ent->x - 6.0f, ent->y}),
                                  hddll::gameToScreen({ent->x + 6.0f, ent->y}),
                                  color);
      } else if (ent->entity_type == 107 ||
                 ent->entity_type == 92) { // Bomb / Landmine
        drawEntityCircle(ent, sqrt(3.75f), color);
        drawEntityCircle(ent, sqrt(1.6f), color);

        auto x_pos = (int)(ent->x + 0.5);
        auto y_pos = (int)(ent->y + 0.5);
        auto x_offset = -3;
        do {
          auto y_count = 7;
          auto y_offset = (99 - y_pos) * 0x2e;
          do {
            auto floor_idx = x_offset + x_pos + y_offset;
            if (floor_idx >= 0 && floor_idx < hddll::ENTITY_FLOORS_COUNT) {
              auto floor =
                  hddll::gGlobalState->level_state->entity_floors[floor_idx];

              if (floor && (floor->flag_6 != 0 || floor->flag_9 != 0) &&
                  floor->flag_4 == 0) {
                auto x_distance = abs(ent->x - floor->x);
                auto y_distance = abs(ent->y - floor->y);
                auto distance =
                    x_distance * x_distance + y_distance * y_distance;

                if (distance < 3.75) {
                  drawEntityHitbox(floor, bombColor, true);
                }
              }
            }

            y_count--;
            y_offset += 0x2e;
          } while (y_count > 0);
          x_offset++;
        } while (x_offset < 4);

        for (size_t idx = 0;
             idx < hddll::gGlobalState->entities->entities_active_count;
             idx++) {
          auto collision_ent =
              (hddll::EntityActive *)
                  hddll::gGlobalState->entities->entities_active[idx];

          if (collision_ent && collision_ent != ent &&
              collision_ent->field49_0x1ed != 0 &&
              (collision_ent->field50_0x1ee != 0 ||
               collision_ent->field63_0x1fb != 0) &&
              collision_ent->entity_type != 137) {
            auto x_distance = abs(ent->x - collision_ent->x);
            auto y_distance = abs(ent->y - collision_ent->y);
            auto distance = x_distance * x_distance + y_distance * y_distance;

            if (distance <= 16.0f &&
                entityCollidesWithCircle(collision_ent, ent->x, ent->y, 1.6f)) {
              drawEntityHitbox(collision_ent, bombColor, true);
            }
          }
        }
      }
    }
  }
}

// Returns the AIBot of the currently selected entity, or nullptr if the
// selected entity is not a player or has no AIBot (i.e. is not a hired hand /
// CPU spelunker).
static hddll::AIBot *selectedAIBot() {
  auto ent = gSelectedEntityState.Entity;
  if (!ent || ent->entity_kind != hddll::EntityKind::KIND_PLAYER) {
    return nullptr;
  }
  return reinterpret_cast<hddll::EntityPlayer *>(ent)->ai_bot;
}

// Draws the bot's current A* path: the forward-linked PathNode chain from
// pPath_current_node -> pNext ... -> pPath_start_node (the goal). Also draws a
// line to flTarget_x/y, the world position the path was computed toward.
static void drawAIBotPath(hddll::AIBot *bot) {
  auto pathColor = ImGui::GetColorU32({0.2f, 0.8f, 1.0f, 0.9f});
  auto nodeColor = ImGui::GetColorU32({0.2f, 0.8f, 1.0f, 0.6f});
  auto activeColor = ImGui::GetColorU32({1.0f, 1.0f, 0.0f, 1.0f});
  auto goalColor = ImGui::GetColorU32({0.0f, 1.0f, 0.3f, 1.0f});
  auto targetColor = ImGui::GetColorU32({1.0f, 0.4f, 0.0f, 0.9f});

  if (bot->entity) {
    auto from = hddll::gameToScreen(
        {bot->entity->x, bot->entity->y});
    auto to = hddll::gameToScreen({bot->flTarget_x, bot->flTarget_y});
    gOverlayDrawList->AddLine(from, to, targetColor, 1.0f);
    drawPointAtCoord({bot->flTarget_x, bot->flTarget_y}, targetColor);
  }

  if (!bot->bPathfind_found_path) {
    return;
  }

  auto node = bot->pPath_current_node;
  int guard = 0;
  while (node && guard++ < hddll::AIBOT_GRID_W * hddll::AIBOT_GRID_H) {
    auto nodeScreen = hddll::gameToScreen(
        {(float)node->nWorld_x, (float)node->nWorld_y});
    auto next = node->pNext;
    if (next) {
      auto nextScreen = hddll::gameToScreen(
          {(float)next->nWorld_x, (float)next->nWorld_y});
      gOverlayDrawList->AddLine(nodeScreen, nextScreen, pathColor, 2.0f);
    }

    ImU32 color = nodeColor;
    float radius = 3.0f;
    if (node == bot->pPath_active_node) {
      color = activeColor;
      radius = 5.0f;
    }
    if (node == bot->pPath_start_node) {
      color = goalColor;
      radius = 5.0f;
    }
    gOverlayDrawList->AddCircleFilled(nodeScreen, radius, color);

    if (node == bot->pPath_start_node) {
      break;
    }
    node = next;
  }
}

// Colors each cell of the bot's 21x17 local pathfinding grid by its flags so
// you can see what the bot considers dangerous / blocked / a pit / etc.
static void drawAIBotGrid(hddll::AIBot *bot) {
  auto dangerColor = ImGui::GetColorU32({1.0f, 0.0f, 0.0f, 0.35f});
  auto blockedColor = ImGui::GetColorU32({0.5f, 0.5f, 0.5f, 0.30f});
  auto pitColor = ImGui::GetColorU32({0.0f, 0.3f, 1.0f, 0.30f});
  auto lockedColor = ImGui::GetColorU32({1.0f, 0.5f, 0.0f, 0.30f});
  auto edgeColor = ImGui::GetColorU32({1.0f, 1.0f, 0.0f, 0.25f});

  for (int i = 0; i < hddll::AIBOT_GRID_W * hddll::AIBOT_GRID_H; i++) {
    auto &node = bot->pPathfind_grid[i];

    ImU32 color;
    if (node.bDanger_flag) {
      color = dangerColor;
    } else if (node.bBlocked_flag) {
      color = blockedColor;
    } else if (node.bPit_flag) {
      color = pitColor;
    } else if (node.bLocked_flag) {
      color = lockedColor;
    } else if (node.bEdge_flag) {
      color = edgeColor;
    } else {
      continue;
    }

    auto topLeft = hddll::gameToScreen(
        {(float)node.nWorld_x - 0.5f, (float)node.nWorld_y + 0.5f});
    auto bottomRight = hddll::gameToScreen(
        {(float)node.nWorld_x + 0.5f, (float)node.nWorld_y - 0.5f});
    gOverlayDrawList->AddRectFilled(topLeft, bottomRight, color);
  }
}

// Picks an overlay color for a perception-list entity based on its kind so
// players / monsters / items are easy to tell apart.
static ImU32 perceptionColorForKind(hddll::EntityKind kind) {
  switch (kind) {
  case hddll::EntityKind::KIND_PLAYER:
    return ImGui::GetColorU32({0.2f, 1.0f, 0.3f, 0.8f});
  case hddll::EntityKind::KIND_MONSTER:
    return ImGui::GetColorU32({1.0f, 0.2f, 0.2f, 0.8f});
  case hddll::EntityKind::KIND_ITEM:
    return ImGui::GetColorU32({1.0f, 0.9f, 0.2f, 0.8f});
  default:
    return ImGui::GetColorU32({1.0f, 1.0f, 1.0f, 0.6f});
  }
}

// Draws the bot's perception list (every entity it can currently "see"), its
// current item/hunt target, and the anchor / wander destination.
static void drawAIBotTargets(hddll::AIBot *bot) {
  if (!bot->entity) {
    return;
  }
  auto botScreen = hddll::gameToScreen(
      {bot->entity->x, bot->entity->y});

  for (int i = 0; i < 128; i++) {
    auto e = bot->pPerception_list[i];
    if (!e) {
      break;
    }
    auto color = perceptionColorForKind(e->entity_kind);
    drawEntityCircle(e, 0.4f, color);
    gOverlayDrawList->AddLine(botScreen, hddll::gameToScreen({e->x, e->y}),
                              color & 0x40FFFFFF, 1.0f);
  }

  if (bot->pCurrent_item_target && findEntity(bot->pCurrent_item_target)) {
    auto target = bot->pCurrent_item_target;
    auto color = ImGui::GetColorU32({1.0f, 0.2f, 0.2f, 1.0f});
    gOverlayDrawList->AddLine(botScreen,
                              hddll::gameToScreen({target->x, target->y}),
                              color, 2.0f);
    drawEntityHitbox(target, color);
  }

  if (bot->ai_state == hddll::AiState::AI_STATE_MOVE_TO_ANCHOR) {
    auto color = ImGui::GetColorU32({0.6f, 0.0f, 1.0f, 0.9f});
    auto anchorScreen =
        hddll::gameToScreen({bot->flAnchor_x, bot->flAnchor_y});
    gOverlayDrawList->AddLine(botScreen, anchorScreen, color, 1.0f);
    gOverlayDrawList->AddCircle(anchorScreen, 6.0f, color, 0, 2.0f);
  }

  if (bot->ai_state == hddll::AiState::AI_STATE_WANDER) {
    auto color = ImGui::GetColorU32({0.0f, 1.0f, 1.0f, 0.9f});
    drawPointAtCoord({(float)bot->nWander_target_x_int,
                      (float)bot->nWander_target_y_int},
                     color);
  }
}

// Draws the bot's current AiState / CombatAction as a label above it, plus an
// arrow showing the movement input (left_right / up_down) it is feeding the
// engine this frame.
static void drawAIBotStateLabel(hddll::AIBot *bot) {
  if (!bot->entity) {
    return;
  }
  auto screen = hddll::gameToScreen({bot->entity->x, bot->entity->y});
  auto text = std::format("{}\n{}", bot->StateName(), bot->CombatActionName());
  gOverlayDrawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() + 2.f,
                            {screen.x + 8.f, screen.y - 28.f}, IM_COL32_WHITE,
                            text.c_str());

  auto input = bot->entity->pPlayer_input;
  if (input && (input->left_right != 0 || input->up_down != 0)) {
    // left_right / up_down are -32 / 0 / +32; up is negative on screen.
    ImVec2 dir = {(float)input->left_right / 32.0f,
                  -(float)input->up_down / 32.0f};
    ImVec2 tip = {screen.x + dir.x * 26.0f, screen.y + dir.y * 26.0f};
    auto arrowColor = ImGui::GetColorU32({1.0f, 0.5f, 0.0f, 1.0f});
    gOverlayDrawList->AddLine(screen, tip, arrowColor, 2.5f);
    gOverlayDrawList->AddCircleFilled(tip, 3.5f, arrowColor);
  }
}

void drawAIBotOverlay() {
  auto bot = selectedAIBot();
  if (!bot) {
    return;
  }
  if (gDebugState.DrawAIBotGrid) {
    drawAIBotGrid(bot);
  }
  if (gDebugState.DrawAIBotPath) {
    drawAIBotPath(bot);
  }
  if (gDebugState.DrawAIBotTargets) {
    drawAIBotTargets(bot);
  }
  if (gDebugState.DrawAIBotStateLabel) {
    drawAIBotStateLabel(bot);
  }
}

void trackSelectedAIBot() {
  auto &log = gAIBotDebugState;
  auto bot = selectedAIBot();
  if (!bot) {
    log.trackedBot = nullptr;
    log.hasLast = false;
    return;
  }

  // Reset the log whenever the tracked bot changes.
  if (log.trackedBot != bot) {
    log.trackedBot = bot;
    log.logCount = 0;
    log.logHead = 0;
    log.hasLast = false;
  }

  bool changed = !log.hasLast || log.lastState != bot->ai_state ||
                 log.lastCombatAction != bot->combat_action;
  if (changed) {
    log.log[log.logHead] = {gFrame, bot->ai_state, bot->combat_action};
    log.logHead = (log.logHead + 1) % AIBotDebugState::LogCapacity;
    if (log.logCount < AIBotDebugState::LogCapacity) {
      log.logCount++;
    }
    log.lastState = bot->ai_state;
    log.lastCombatAction = bot->combat_action;
    log.hasLast = true;
  }
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

  if (!findEntity(gSelectedEntityState.Entity)) {
    gSelectedEntityState.Entity = NULL;
  }

  trackSelectedAIBot();

  hddll::Entity *closestEnt = NULL;
  if (ImGui::IsWindowHovered()) {
    handleTeleportInput();
    handleSpawnInput();
    closestEnt = handleSelectInput();
  }

  gOverlayDrawList->AddText(ImGui::GetFont(), 32.f, {148.f, 40.f},
                            ImGui::GetColorU32({1.0f, 1.0f, 1.0f, 0.05f}),
                            "SpecsHD");

  if (gModsState.SeededMode) {
    auto out = std::format("Seed: {}", lastSeed);
    gOverlayDrawList->AddText(
        ImGui::GetFont(), 32.f, {io.DisplaySize.x - 348.f, 40.f},
        ImGui::GetColorU32({1.0f, 1.0f, 1.0f, 0.8f}), out.c_str());
  }

  if (gModsState.TheFullSpelunky && gFullSpelunkyState.showCharacterOverlay) {
    drawCharacterOverlay();
  }

  if (gDebugState.EnableTileBorders) {
    drawTileBorders();
  }
  if (gDebugState.EnableBinBorders) {
    drawBinBorders();
  }
  if (gDebugState.EnableRoomBorders) {
    drawRoomBorders();
  }
  if (gDebugState.EnablePacifistOverlay) {
    drawPacifistOverlay();
  }
  if (gDebugState.EnableSeededCrateOverlay) {
    drawSeededCrateOverlay();
  }
  if (gDebugState.EnableSeededPotOverlay) {
    drawSeededPotOverlay();
  }
  if (gDebugState.EnableSeededChestOverlay) {
    drawSeededChestOverlay();
  }
  if (gDebugState.EnableSeededKaliRewards) {
    drawSeededKaliRewardsOverlay();
  }

  forEnabledEntities(gDebugState.Hitboxes, &drawEntityHitboxDefault);
  forEnabledEntities(gDebugState.Ids, &drawEntityId);

  if (gDebugState.DrawEntityOffsets.size() > 0) {
    forEnabledEntities(gAllEntities, &drawEntityOffsetDebug);
  }

  if (gDebugState.ShowOlmecCrushProbes) {
    drawOlmecCrushProbes();
  }
  if (gDebugState.BlackMarketTrainer &&
      hddll::gGlobalState->screen_state == 0 &&
      hddll::gGlobalState->play_state == 0) {
    drawBlackMarketTrainer();
  }
  if (gDebugState.DrawHHFollowerLink || gDebugState.DrawHHFollowingLink) {
    drawHHLinks();
  }
  if (gDebugState.DrawEnemyDetection) {
    drawEnemyDetection();
  }
  if (gDebugState.DrawAIBotPath || gDebugState.DrawAIBotGrid ||
      gDebugState.DrawAIBotTargets || gDebugState.DrawAIBotStateLabel) {
    drawAIBotOverlay();
  }

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
