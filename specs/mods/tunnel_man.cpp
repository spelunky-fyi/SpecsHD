
#include "tunnel_man.h"

std::vector<hddll::Patch> gTunnelManPatches = {
    // Put back stolen bytes
    {0x569a5, {}, {0x83, 0xbf, 0x34, 0x01, 0x00, 0x00, 0x1d}},
};

void resetTunnelManState() {
  hddll::gGlobalState->player1_data.health = 2;
  hddll::gGlobalState->player1_data.health2 = 2;
  hddll::gGlobalState->player1_data.bombs = 0;
  hddll::gGlobalState->player1_data.ropes = 0;
  if (hddll::gGlobalState->player1) {
    hddll::gGlobalState->player1->health = 2;
  }
}

void onRunningFrameTunnelMan() {
  if (!hddll::gGlobalState->player1) {
    return;
  }

  if (hddll::gGlobalState->player1->holding_entity &&
      hddll::gGlobalState->player1->holding_entity->entity_type == 510) {

    if (hddll::gGlobalState->player1->field27_0x198 < 1) {
      auto mattock = hddll::gGlobalState->player1->holding_entity;
      mattock->flag_deletion = 1;
      mattock->holder_entity = NULL;
      hddll::gGlobalState->player1->holding_entity = NULL;
      hddll::gGlobalState->player1->player_data->held_item_id = 0;
    }
  }
}

hddll::Entity *postSpawnEntityTunnelMan(hddll::Entity *ent) {
  if (hddll::gGlobalState && hddll::gGlobalState->screen_state == 0 &&
      hddll::gGlobalState->play_state == 0) {
    if (ent && ent->entity_type == 109) {
      ent->alpha = 0.0;
      ent->flag_deletion = 1;
      auto new_ent =
          hddll::gGlobalState->SpawnEntity(ent->x, ent->y, 510, true);

      return new_ent;
    }
  }
  return ent;
}
