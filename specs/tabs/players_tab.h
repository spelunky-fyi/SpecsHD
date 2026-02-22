#pragma once

#include <cstdint>

struct PlayerState {
  bool LockHealth = false;
  int LockedHealthAmount = 0;

  bool LockBombs = false;
  int LockedBombsAmount = 0;

  bool LockRopes = false;
  int LockedRopesAmount = 0;

  bool LockHeldItemId = false;
  int LockedHeldItemId = 0;

  bool LockHeldItemMetadata = false;
  int LockedHeldItemMetadata = 0;

  bool LockHiredHandCount = false;
  int LockedHiredHandCount = 0;

  bool LockCompass = false;
  bool LockedCompassValue = false;

  bool LockParachute = false;
  bool LockedParachuteValue = false;

  bool LockJetpack = false;
  bool LockedJetpackValue = false;

  bool LockClimbingGloves = false;
  bool LockedClimbingGlovesValue = false;

  bool LockPitchersMitt = false;
  bool LockedPitchersMittValue = false;

  bool LockSpringShoes = false;
  bool LockedSpringShoesValue = false;

  bool LockSpikeShoes = false;
  bool LockedSpikeShoesValue = false;

  bool LockSpectacles = false;
  bool LockedSpectaclesValue = false;

  bool LockKapala = false;
  bool LockedKapalaValue = false;

  bool LockHedjet = false;
  bool LockedHedjetValue = false;

  bool LockUdjatEye = false;
  bool LockedUdjatEyeValue = false;

  bool LockBookOfTheDead = false;
  bool LockedBookOfTheDeadValue = false;

  bool LockAnkh = false;
  bool LockedAnkhValue = false;

  bool LockPaste = false;
  bool LockedPasteValue = false;

  bool LockCape = false;
  bool LockedCapeValue = false;

  bool LockVladsCape = false;
  bool LockedVladsCapeValue = false;

  bool LockCrysknife = false;
  bool LockedCrysknifeValue = false;

  bool LockVladsAmulet = false;
  bool LockedVladsAmuletValue = false;

  bool LockWhiteFlag = false;
  bool LockedWhiteFlagValue = false;
};

extern PlayerState gPlayersState[4];

void ensureLockedAmounts();
void drawPlayersTab();
