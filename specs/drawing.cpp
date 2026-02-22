
#include "drawing.h"

#include "entities.h"
#include "inputs.h"
#include "utils.h"
#include "ui.h"

#include "game_hooks.h"
#include "mods/full_spelunky.h"
#include "mods/seeded_mode.h"
#include "seeded_items.h"
#include "tabs/debug_tab.h"
#include "tabs/selected_tab.h"
#include "tabs/spawn_tab.h"

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

void drawPointAtCoord(ImVec2 coord, ImU32 color) {
  auto topLeft = gameToScreen(coord);
  topLeft.x -= 1;
  topLeft.y += 1;

  ImVec2 topRight = {topLeft.x + 2, topLeft.y};
  ImVec2 bottomRight = {topLeft.x + 2, topLeft.y - 2};
  ImVec2 bottomLeft = {topLeft.x, topLeft.y - 2};

  gOverlayDrawList->AddQuad(topLeft, topRight, bottomRight, bottomLeft, color,
                            1.f);
}

void drawEntityCircle(Entity *ent, float radius, ImU32 color, float x_offset,
                      float y_offset, bool filled) {
  auto screen = gameToScreen({ent->x + x_offset, ent->y + y_offset});
  auto screenRadius =
      gameToScreen({ent->x + x_offset + radius, 0}).x - screen.x;

  if (filled) {
    gOverlayDrawList->AddCircleFilled(screen, screenRadius, color, 0);
  } else {
    gOverlayDrawList->AddCircle(screen, screenRadius, color, 0, 1.f);
  }
}

void drawEntityHitbox(Entity *ent, ImU32 color, bool filled) {

  ImVec2 topLeft =
      gameToScreen({ent->x - ent->hitbox_x, ent->y + ent->hitbox_up});
  ImVec2 topRight =
      gameToScreen({ent->x + ent->hitbox_x, ent->y + ent->hitbox_up});
  ImVec2 bottomRight =
      gameToScreen({ent->x + ent->hitbox_x, ent->y - ent->hitbox_down});
  ImVec2 bottomLeft =
      gameToScreen({ent->x - ent->hitbox_x, ent->y - ent->hitbox_down});

  if (filled) {
    gOverlayDrawList->AddQuadFilled(topLeft, topRight, bottomRight, bottomLeft,
                                    color);
  } else {
    gOverlayDrawList->AddQuad(topLeft, topRight, bottomRight, bottomLeft, color,
                              1.f);
  }

  if (gDebugState.IncludeHitboxOrigins) {
    auto ent_origin = gameToScreen({ent->x, ent->y});
    gOverlayDrawList->AddLine(topLeft, ent_origin, color, 1.f);
    gOverlayDrawList->AddLine(topRight, ent_origin, color, 1.f);
    gOverlayDrawList->AddLine(bottomLeft, ent_origin, color, 1.f);
    gOverlayDrawList->AddLine(bottomRight, ent_origin, color, 1.f);
    drawPointAtCoord({ent->x, ent->y}, color);
  }

  if (ent->flag_23 == 1 || ent->entity_kind == EntityKind::KIND_EXPLOSION) {
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

void drawEntityDetectionRay(Entity *ent, float len, ImU32 color) {

  auto factor = 1.001f;
  auto top = 0.25f;
  if (ent->flag_horizontal_flip) {
    gOverlayDrawList->AddQuad(
        gameToScreen({ent->x + ent->hitbox_x * factor - len, ent->y - top}),
        gameToScreen({ent->x + ent->hitbox_x * factor, ent->y - top}),

        gameToScreen({ent->x + ent->hitbox_x * factor,
                      ent->y - ent->hitbox_down * factor}),

        gameToScreen({ent->x + ent->hitbox_x * factor - len,
                      ent->y - ent->hitbox_down * factor}),
        color);
  } else {
    gOverlayDrawList->AddQuad(
        gameToScreen({ent->x - ent->hitbox_x * factor + len, ent->y - top}),
        gameToScreen({ent->x - ent->hitbox_x * factor, ent->y - top}),
        gameToScreen({ent->x - ent->hitbox_x * factor,
                      ent->y - ent->hitbox_down * factor}),
        gameToScreen({ent->x - ent->hitbox_x * factor + len,
                      ent->y - ent->hitbox_down * factor}),
        color);
  }
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

void drawRoomBorders() {
  if (gGlobalState->screen_state == 0) {

    auto room_width = 10.f;
    auto room_height = 8.f;
    auto max_height = 99.5f;
    auto max_width = 42.5f;

    for (auto x = 2.5f; x < 50.f; x += room_width) {
      gOverlayDrawList->AddLine(gameToScreen({x, 3.5f}),
                                gameToScreen({x, max_height}), IM_COL32_WHITE,
                                2.0f);
    }
    for (auto y = 3.5f; y < 120.f; y += room_height) {
      gOverlayDrawList->AddLine(gameToScreen({2.5, y}),
                                gameToScreen({max_width, y}), IM_COL32_WHITE,
                                2.0f);
    }

    for (auto idx = 0; idx < 48; idx++) {
      auto column = idx % 4;
      auto row = idx / 4;
      auto type = gGlobalState->level_state->room_types[idx];

      auto y = max_height - (row * room_height);
      auto x = 2.5f + (column * room_width);

      auto out = std::format("Type: {}", type);
      auto screen = gameToScreen({x + 0.25f, y - 0.25f});
      gOverlayDrawList->AddText(ImGui::GetFont(), ImGui::GetFontSize() + 8,
                                ImVec2{screen.x, screen.y}, IM_COL32_WHITE,
                                out.c_str());
    }
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

void drawEntityOffsetDebug(Entity *ent) {

  auto entry =
      gDebugState
          .DrawEntityOffsets[std::pair{ent->entity_kind, ent->entity_type}];
  if (entry.empty()) {
    return;
  }

  auto idx = 0;
  auto font = ImGui::GetFont();
  auto fontSize = ImGui::GetFontSize() + 5;
  auto screen = gameToScreen({ent->x - ent->hitbox_x, ent->y + ent->hitbox_up});
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

void forEntities(std::unordered_set<uint32_t> excludedEntities,
                 EntityCallback callback, Entity **entities, size_t count,
                 bool decos) {
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
                     bool decos) {
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

bool entityCollidesWithCircle(Entity *targetEnt, float x_pos, float y_pos,
                              float radius) {
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

bool collidesWithEntityCircle(Entity *sourceEnt, Entity *targetEnt,
                              float x_offset, float y_offset) {

  return entityCollidesWithCircle(targetEnt, sourceEnt->x + x_offset,
                                  sourceEnt->y + y_offset,
                                  sourceEnt->hitbox_down);
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

static void handleTeleportInput() {
  ImGuiIO &io = ImGui::GetIO();
  if (Specs::IsMouseClicked(gConfig->buttons[Specs::MouseFeatures_Teleport])) {
    auto player = gGlobalState->player1;
    if (player) {
      auto pos = screenToGame(io.MousePos);
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
          auto gamePos = screenToGame(gSpawnState.ClickedAt);
          Entity *ent;

          if (spawnEntityConfig.entityType == 0) {
            ent = gGlobalState->SpawnHiredHand(gamePos.x, gamePos.y, 90);
          } else {
            ent = gGlobalState->SpawnEntity(gamePos.x, gamePos.y,
                                            spawnEntityConfig.entityType,
                                            spawnEntityConfig.activeEntity);
          }

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
          if (spawnEntityConfig.entityType == 0) {
            gGlobalState->SpawnHiredHand(gamePos.x, gamePos.y, 90);
          } else {
            gGlobalState->SpawnEntity(gamePos.x, gamePos.y,
                                      spawnEntityConfig.entityType,
                                      spawnEntityConfig.activeEntity);
          }
        }
      }
    }
  }
}

static Entity *handleSelectInput() {
  ImGuiIO &io = ImGui::GetIO();
  Entity *closestEnt = NULL;

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

  return closestEnt;
}

static void drawOlmecCrushProbes() {
  for (size_t idx = 0; idx < gGlobalState->entities->entities_active_count;
       idx++) {
    auto ent = (EntityActive *)gGlobalState->entities->entities_active[idx];

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
  if (gGlobalState->level > 4 && gGlobalState->level < 9 &&
      gGlobalState->is_worm == 0 && gGlobalState->is_blackmarket == 0 &&
      gGlobalState->level_state->alt_exit_x > 0 &&
      gGlobalState->level_state->alt_exit_y > 0) {

    auto color = ImGui::GetColorU32({255.f, 0.0f, 0.0f, 0.25f});
    auto replace_color = ImGui::GetColorU32({255.0f, 255.0f, 0.0f, 0.25f});
    auto bm_color = ImGui::GetColorU32({0.f, 255.0f, 0.0f, 0.25f});

    if (gGlobalState->player1) {
      drawEntityCircle(gGlobalState->player1, 10.f, bm_color);
    }

    for (auto e_idx = 0; e_idx < ELIGIBLE_FLOORS_FOR_BM_COUNT; e_idx++) {

      auto idx = ELIGIBLE_FLOORS_FOR_BM[e_idx];
      auto floor = gGlobalState->level_state->entity_floors[idx];
      if (!floor) {
        continue;
      }

      // BM, draw green
      if (floor->x == gGlobalState->level_state->alt_exit_x &&
          floor->y == gGlobalState->level_state->alt_exit_y) {
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
  for (size_t idx = 0; idx < gGlobalState->entities->entities_active_count;
       idx++) {
    auto ent = (EntityActive *)gGlobalState->entities->entities_active[idx];

    if (!ent)
      continue;
    if (ent->entity_kind != EntityKind::KIND_PLAYER)
      continue;

    auto player = (EntityPlayer *)ent;

    if (gDebugState.DrawHHFollowerLink && player->follower) {
      auto follower_color = ImGui::GetColorU32({255.f, 0.0f, 0.0f, 0.9f});
      auto start = gameToScreen({player->x, player->y});
      auto end = gameToScreen({player->follower->x, player->follower->y});
      start.y += 4;
      end.y += 4;
      gOverlayDrawList->AddLine(start, end, follower_color, 1.f);
    }

    if (gDebugState.DrawHHFollowingLink && player->following) {
      auto following_color = ImGui::GetColorU32({0.f, 255.0f, 0.0f, 0.9f});
      auto start = gameToScreen({player->x, player->y});
      auto end = gameToScreen({player->following->x, player->following->y});
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

  if (gGlobalState->player1) {
    drawPointAtCoord({gGlobalState->player1->x, gGlobalState->player1->y});
  }
  for (size_t idx = 0; idx < gGlobalState->entities->entities_active_count;
       idx++) {
    auto ent = (EntityActive *)gGlobalState->entities->entities_active[idx];

    if (ent) {
      auto entityRoom = gGlobalState->level_state
                            ->room_types[GetRoomForPosition(ent->x, ent->y)];
      if (ent->entity_type == 1002) { // Spider
        gOverlayDrawList->AddQuad(
            gameToScreen({ent->x - 0.4f, ent->y}),
            gameToScreen({ent->x + 0.4f, ent->y}),
            gameToScreen({ent->x + 0.4f, ent->y - 7.0f}),
            gameToScreen({ent->x - 0.4f, ent->y - 7.0f}), color);
      } else if (ent->entity_type == 1003) { // Bat
        gOverlayDrawList->PathArcToFast(gameToScreen({ent->x, ent->y - 0.2f}),
                                        gameToScreen({ent->x + 6.0f, 0.f}).x -
                                            gameToScreen({ent->x, 0.f}).x,
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
                gameToScreen({ent->x - 5.0f, ent->y + 12.0f}),
                gameToScreen({ent->x, ent->y + 12.0f}),
                gameToScreen({ent->x, ent->y - 12.0f}),
                gameToScreen({ent->x - 5.0f, ent->y - 12.0f}), color);
          } else {
            gOverlayDrawList->AddQuad(
                gameToScreen({ent->x, ent->y + 12.0f}),
                gameToScreen({ent->x + 5.0f, ent->y + 12.0f}),
                gameToScreen({ent->x + 5.0f, ent->y - 12.0f}),
                gameToScreen({ent->x, ent->y - 12.0f}), color);
          }
        }
      } else if (ent->entity_type == 1007 ||
                 ent->entity_type == 1021) { // Blue / Orange Frog
        drawEntityCircle(ent, 8.f, color);
      } else if (ent->entity_type == 1010) { // UFO
        gOverlayDrawList->AddLine(gameToScreen({ent->x - 8.0f, ent->y}),
                                  gameToScreen({ent->x + 8.0f, ent->y}),
                                  color);
        gOverlayDrawList->AddLine(
            gameToScreen({ent->x - 5.25f, ent->y - 6.0f}),
            gameToScreen({ent->x + 5.25f, ent->y - 6.0f}), color);
        drawEntityCircle(ent, 8.f, color);
        gOverlayDrawList->AddQuad(
            gameToScreen({ent->x - 0.5f, ent->y}),
            gameToScreen({ent->x + 0.5f, ent->y}),
            gameToScreen({ent->x + 0.5f, ent->y - 8.0f}),
            gameToScreen({ent->x - 0.5f, ent->y - 8.0f}), color);
      } else if (ent->entity_type == 1012) { // Skeleton
        gOverlayDrawList->PathArcToFast(gameToScreen({ent->x, ent->y}),
                                        gameToScreen({ent->x + 4.0f, 0.f}).x -
                                            gameToScreen({ent->x, 0.f}).x,
                                        0, 6);
        gOverlayDrawList->PathStroke(color, ImDrawFlags_Closed, 1.0f);
      } else if (ent->entity_type == 1013) { // Piranha
        drawEntityCircle(ent, 6.f, waterColor);
      } else if (ent->entity_type == 1023) { // Old Bitey
        drawEntityCircle(ent, 5.f, waterColor);
      } else if (ent->entity_type == 1014) { // Mummy
        gOverlayDrawList->AddLine(
            gameToScreen({ent->x - 5.2f, ent->y - 3.0f}),
            gameToScreen({ent->x + 5.2f, ent->y - 3.0f}), color);
        gOverlayDrawList->AddLine(
            gameToScreen({ent->x - 5.2f, ent->y + 3.0f}),
            gameToScreen({ent->x + 5.2f, ent->y + 3.0f}), color);
        drawEntityCircle(ent, 6.f, color);
      } else if (ent->entity_type == 1015) { // Monkey
        if (ent->field71_0x203 != 0) {
          gOverlayDrawList->PathArcToFast(
              gameToScreen({ent->x, ent->y}),
              gameToScreen({ent->x + 4.0f, 0.f}).x -
                  gameToScreen({ent->x, 0.f}).x,
              0, 6);
          gOverlayDrawList->PathStroke(color, ImDrawFlags_Closed, 1.0f);
        } else {
          drawEntityCircle(ent, 4.f, color);
        }
      } else if (ent->entity_type == 1020 ||
                 ent->entity_type == 1028) { // Vampire or Vlad
        if (entityRoom < 0x13 || entityRoom > 0x15) {
          gOverlayDrawList->PathArcToFast(
              gameToScreen({ent->x, ent->y - 0.2f}),
              gameToScreen({ent->x + 6.0f, 0.f}).x -
                  gameToScreen({ent->x, 0.f}).x,
              0, 6);
          gOverlayDrawList->PathStroke(color, ImDrawFlags_Closed, 1.0f);
        } else {
          gOverlayDrawList->AddQuad(
              gameToScreen({ent->x - 1.0f, ent->y - 0.2f}),
              gameToScreen({ent->x + 1.0f, ent->y - 0.2f}),
              gameToScreen({ent->x + 1.0f, ent->y - 6.0f}),
              gameToScreen({ent->x - 1.0f, ent->y - 6.0f}), color);
        }
      } else if (ent->entity_type == 1018) { // Giant Spider
        gOverlayDrawList->AddQuad(
            gameToScreen({ent->x - 1.0f, ent->y}),
            gameToScreen({ent->x + 1.0f, ent->y}),
            gameToScreen({ent->x + 1.0f, ent->y - 8.0f}),
            gameToScreen({ent->x - 1.0f, ent->y - 8.0f}), color);
      } else if (ent->entity_type == 1019) { // Jiang Shi
        drawEntityCircle(ent, 8.f, color);
      } else if (ent->entity_type == 1024) { // Scarab
        drawEntityCircle(ent, 6.f, color);
        gOverlayDrawList->AddLine(
            gameToScreen({ent->x - 5.95f, ent->y - 0.2f}),
            gameToScreen({ent->x + 5.95f, ent->y - 0.2f}), color);
      } else if (ent->entity_type == 1046) { // Worm Egg
        drawEntityCircle(ent, 6.f, color);
        gOverlayDrawList->AddLine(gameToScreen({ent->x - 5.6f, ent->y + 2.f}),
                                  gameToScreen({ent->x + 5.6f, ent->y + 2.f}),
                                  color);
      } else if (ent->entity_type == 1025) { // Yeti King
        drawEntityCircle(ent, 6.f, color);
      } else if (ent->entity_type == 1030) { // Imp
        gOverlayDrawList->AddQuad(
            gameToScreen({ent->x - 1.0f, ent->y}),
            gameToScreen({ent->x + 1.0f, ent->y}),
            gameToScreen({ent->x + 1.0f, ent->y - 8.0f}),
            gameToScreen({ent->x - 1.0f, ent->y - 8.0f}), color);
      } else if (ent->entity_type == 1038) { // Giant Frog
        drawEntityCircle(ent, 6.f, color);
      } else if (ent->entity_type == 1031) { // Blue Devil
        drawEntityDetectionRay(ent, 6.0f, wallColor);
        gOverlayDrawList->AddQuad(
            gameToScreen({ent->x - ent->hitbox_x, ent->y + ent->hitbox_up}),
            gameToScreen(
                {ent->x - ent->hitbox_x, ent->y + ent->hitbox_up + 5.0f}),
            gameToScreen(
                {ent->x + ent->hitbox_x, ent->y + ent->hitbox_up + 5.0f}),
            gameToScreen({ent->x + ent->hitbox_x, ent->y + ent->hitbox_up}),
            wallColor);
      } else if (ent->entity_type == 1033) { // Anubis
        drawEntityCircle(ent, 11.f, color);
        drawEntityCircle(ent, 8.f, color);
        drawEntityCircle(ent, 4.f, color);
      } else if (ent->entity_type == 1040) { // Alien Tank
        drawEntityCircle(ent, 6.f, color);
        gOverlayDrawList->AddLine(gameToScreen({ent->x, ent->y - 2.0f}),
                                  gameToScreen({ent->x, ent->y + 6.0f}),
                                  color);
        if (ent->flag_horizontal_flip) {
          gOverlayDrawList->AddLine(
              gameToScreen({ent->x - 5.6f, ent->y - 2.0f}),
              gameToScreen({ent->x, ent->y - 2.0f}), color);
        } else {
          gOverlayDrawList->AddLine(
              gameToScreen({ent->x, ent->y - 2.0f}),
              gameToScreen({ent->x + 5.6f, ent->y - 2.0f}), color);
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
          gOverlayDrawList->AddQuad(gameToScreen({ent->x - 1.0f, 200.0}),
                                    gameToScreen({ent->x + 1.0f, 200.0}),
                                    gameToScreen({ent->x + 1.0f, ent->y}),
                                    gameToScreen({ent->x - 1.0f, ent->y}),
                                    color, 1.f);
          gOverlayDrawList->AddQuad(gameToScreen({14.0, 110.0}),
                                    gameToScreen({31.0, 110.0}),
                                    gameToScreen({31.0, 88.0}),
                                    gameToScreen({14.0, 88.0}), color, 1.f);
          gOverlayDrawList->AddQuad(gameToScreen({-10.0, 110.0}),
                                    gameToScreen({14.0, 110.0}),
                                    gameToScreen({14.0, 93.0}),
                                    gameToScreen({-10.0, 93.0}), color, 1.f);
          gOverlayDrawList->AddQuad(gameToScreen({31.0, 110.0}),
                                    gameToScreen({60.0, 110.0}),
                                    gameToScreen({60.0, 93.0}),
                                    gameToScreen({31.0, 93.0}), color, 1.f);
        }
      } else if (ent->entity_type == 1057) { // Yama Hand
        drawEntityCircle(ent, 6.f, color);
        gOverlayDrawList->AddLine(gameToScreen({-10.0f, ent->y - 2.0f}),
                                  gameToScreen({60.0f, ent->y - 2.0f}),
                                  color);
      } else if (ent->entity_type == 1058) { // Turret
        drawEntityCircle(ent, 6.f, color);
        gOverlayDrawList->AddLine(gameToScreen({ent->x - 6.0f, ent->y}),
                                  gameToScreen({ent->x + 6.0f, ent->y}),
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
            if (floor_idx >= 0 && floor_idx < 4692) {
              auto floor =
                  gGlobalState->level_state->entity_floors[floor_idx];

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
             idx < gGlobalState->entities->entities_active_count; idx++) {
          auto collision_ent =
              (EntityActive *)gGlobalState->entities->entities_active[idx];

          if (collision_ent && collision_ent != ent &&
              collision_ent->field49_0x1ed != 0 &&
              (collision_ent->field50_0x1ee != 0 ||
               collision_ent->field63_0x1fb != 0) &&
              collision_ent->entity_type != 137) {
            auto x_distance = abs(ent->x - collision_ent->x);
            auto y_distance = abs(ent->y - collision_ent->y);
            auto distance = x_distance * x_distance + y_distance * y_distance;

            if (distance <= 16.0f &&
                entityCollidesWithCircle(collision_ent, ent->x, ent->y,
                                         1.6f)) {
              drawEntityHitbox(collision_ent, bombColor, true);
            }
          }
        }
      }
    }
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

  Entity *closestEnt = NULL;
  if (ImGui::IsWindowHovered()) {
    handleTeleportInput();
    handleSpawnInput();
    closestEnt = handleSelectInput();
  }

  gOverlayDrawList->AddText(ImGui::GetFont(), 32.f, {148.f, 40.f},
                            ImGui::GetColorU32({1.0f, 1.0f, 1.0f, 0.05f}),
                            "SpecsHD");

  if (gModsState.SeededMode) {
    auto out = std::format("Seed: {}", lastSeed, ImGui::GetFontSize());
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
  if (gDebugState.BlackMarketTrainer && gGlobalState->screen_state == 0 &&
      gGlobalState->play_state == 0) {
    drawBlackMarketTrainer();
  }
  if (gDebugState.DrawHHFollowerLink || gDebugState.DrawHHFollowingLink) {
    drawHHLinks();
  }
  if (gDebugState.DrawEnemyDetection) {
    drawEnemyDetection();
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
