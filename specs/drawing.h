#pragma once

#include <functional>
#include <unordered_set>

#include "state.h"

// Forward declaration (defined in tabs/debug_tab.h)
struct EnabledEntities;

// Coordinate conversion
ImVec2 screenToGame(ImVec2 screen);
ImVec2 gameToScreen(ImVec2 game);

// Entity drawing
void drawPointAtCoord(ImVec2 coord,
                      ImU32 color = ImGui::GetColorU32({255.f, 0.0f, 0.0f,
                                                        0.9f}));
void drawEntityCircle(Entity *ent, float radius,
                      ImU32 color = ImGui::GetColorU32({255.f, 0.0f, 238.0f,
                                                        0.7f}),
                      float x_offset = 0.0f, float y_offset = 0.0f,
                      bool filled = false);
void drawEntityHitbox(Entity *ent,
                      ImU32 color = ImGui::GetColorU32({255.f, 0.0f, 238.0f,
                                                        0.7f}),
                      bool filled = false);
void drawEntityHitboxDefault(Entity *ent);
void drawEntityDetectionRay(Entity *ent, float len, ImU32 color);
void drawEntityId(Entity *ent);
void drawEntityOffsetDebug(Entity *ent);

// UI helpers
bool drawCharBool(const char *label, char &flag);
bool drawCharBool(const char *label, uint8_t &flag);

// Entity iteration
using EntityCallback = std::function<void(Entity *e)>;
void forEntities(const std::unordered_set<uint32_t> &excludedEntities,
                 EntityCallback callback, Entity **entities, size_t count,
                 bool decos = false);
void forEnabledEntities(EnabledEntities &enabledEnts, EntityCallback callback);

// Entity search
bool findEntityArray(Entity *searchEnt, Entity **entities, size_t count,
                     bool decos = false);
bool findEntity(Entity *searchEnt);

// Collision helpers
float dist(ImVec2 pos1, ImVec2 pos2);
bool entityCollidesWithCircle(Entity *targetEnt, float x_pos, float y_pos,
                              float radius);
bool collidesWithEntityCircle(Entity *sourceEnt, Entity *targetEnt,
                              float x_offset = 0.0f, float y_offset = 0.0f);

// Other overlays
void drawPacifistOverlay();
void drawBinBorders();
void drawRoomBorders();
void drawTileBorders();

// Entity kind size
size_t sizeofEntityKind(EntityKind entityKind);

// Main overlay dispatcher
void drawOverlayWindow();
