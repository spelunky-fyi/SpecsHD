
#include "tunnel_man.h"

std::vector<Patch> gTunnelManPatches = {
    // Put back stolen bytes
    {0x569a5, {}, {0x83, 0xbf, 0x34, 0x01, 0x00, 0x00, 0x1d}},
};

void resetTunnelManState() {
  gGlobalState->player1_data.health = 2;
  gGlobalState->player1_data.health2 = 2;
  gGlobalState->player1_data.bombs = 0;
  gGlobalState->player1_data.ropes = 0;
  if (gGlobalState->player1) {
    gGlobalState->player1->health = 2;
  }
}

void onRunningFrameTunnelMan() {
  if (!gGlobalState->player1) {
    return;
  }

  if (gGlobalState->player1->holding_entity &&
      gGlobalState->player1->holding_entity->entity_type == 510) {

    if (gGlobalState->player1->field27_0x198 < 1) {
      auto mattock = gGlobalState->player1->holding_entity;
      mattock->flag_deletion = 1;
      mattock->holder_entity = NULL;
      gGlobalState->player1->holding_entity = NULL;
      gGlobalState->player1->player_data->held_item_id = 0;
    }
  }
}

Entity *postSpawnEntityTunnelMan(Entity *ent) {
  if (gGlobalState && gGlobalState->screen_state == 0 &&
      gGlobalState->play_state == 0) {
    if (ent && ent->entity_type == 109) {
      ent->alpha = 0.0;
      ent->flag_deletion = 1;
      auto new_ent = gGlobalState->SpawnEntity(ent->x, ent->y, 510, true);

      return new_ent;
    }
  }
  return ent;
}
