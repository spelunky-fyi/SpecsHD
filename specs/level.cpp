#include "level.h"
#include "hd.h"
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>

using RoomList = std::map<ROOM_TYPE, std::vector<std::string>>;

RoomList rooms;

void readFileLevel(std::string filename, RoomList &ret) {
  ret.clear();
  std::ifstream levelFile(filename);
  std::string line_str;
  ROOM_TYPE current_room_id;
  std::string current_room = "";
  uint32_t room_line = 0;
  while (std::getline(levelFile, line_str)) {
    line_str.erase(std::remove_if(line_str.begin(), line_str.end(), isspace),
                   line_str.end());
    std::regex remove_comment("//.*");
    line_str = std::regex_replace(line_str, remove_comment, "");
    // std::cout << line_str << "\n";
    if (line_str.rfind("\\.", 0) == 0) {
      std::string room_template = line_str.substr(2, line_str.length());
      auto search = roomMap.find(room_template);
      if (search != roomMap.end()) {
        current_room_id = search->second;
      } else {
        // std::cout << "ERROR\n";
      }
    } else if (line_str.length() == 10) {
      current_room.append(line_str);
      ++room_line;
      if (room_line == 8) {
        ret[current_room_id].push_back(current_room);
        current_room = "";
        room_line = 0;
      }
    }
  }
  levelFile.close();
  /*std::cout << "\n----\n";
  for (const auto& p : ret)
  {
      std::cout << p.first << "\n";
      for (const auto s : p.second) {
          std::cout << s << "\n";
      }
  }*/
}

bool trySetRoom(ROOM_TYPE roomType, char *roomOut) {
  auto search = rooms.find(roomType);
  if (search != rooms.end()) {
    auto room_vec = search->second;
    int i = rand() % room_vec.size();
    room_vec[i].copy(roomOut, 80);
    roomOut[80] = '\0';
    return true;
  }
  return false;
}

bool customRoomGet(int doorRoomType, int roomIndex, char *roomOut,
                             LevelState *levelState) {
  ROOM_TYPE roomType = (ROOM_TYPE)levelState->room_types[roomIndex];
  ROOM_TYPE aboveRoom =
      roomIndex > 3 ? levelState->room_types[roomIndex - 4] : ROOM_TYPE::UNSET;
  if (doorRoomType == 1) {
    if (roomType == ROOM_TYPE::PATH_DROP) {
      return trySetRoom(ROOM_TYPE::ENTRANCE_DROP, roomOut);
    } else {
      return trySetRoom(ROOM_TYPE::ENTRANCE, roomOut);
    }
  } else if (doorRoomType == 2) {
    if (aboveRoom == ROOM_TYPE::PATH_DROP) { // maybe add also if is coffin_drop
                                             // or beehive_down
      return trySetRoom(ROOM_TYPE::EXIT_NOTOP, roomOut);
    } else {
      return trySetRoom(ROOM_TYPE::EXIT, roomOut);
    }
  } else {
    GlobalState *globalState = levelState->global_state;
    if (roomType == ROOM_TYPE::SIDE) {
      if (globalState->level > 1) {
        if (globalState->altar_spawned == 0 && rand() % 14 == 0) {
          globalState->altar_spawned = 1;
          return trySetRoom(ROOM_TYPE::ALTAR, roomOut);
        } else if (globalState->idol_spawned == 0 && 11 < roomIndex &&
                   rand() % 10 == 0) {
          globalState->idol_spawned = 1;
          return trySetRoom(ROOM_TYPE::IDOL, roomOut);
        }
      }
      return trySetRoom(ROOM_TYPE::SIDE, roomOut);
    } else if (roomType == ROOM_TYPE::PATH_DROP &&
               (aboveRoom == ROOM_TYPE::PATH_DROP ||
                aboveRoom == ROOM_TYPE::COFFIN_DROP)) {
      return trySetRoom(ROOM_TYPE::PATH_DROP_NOTOP, roomOut);
    } else if (roomType == ROOM_TYPE::SHOP_LEFT_FACING ||
               roomType == ROOM_TYPE::SHOP_RIGHT_FACING) {
      // the roomType is SHOP_LEFT_FACING or SHOP_RIGHT_FACING at this part of
      // the generation, it's changed later int sum = (roomType ==
      // ROOM_TYPE::SHOP_LEFT_FACING) ? 100 : 10100; roomType =
      // (ROOM_TYPE)((int)levelState->shop_type + sum);

      auto shop_type = levelState->shop_type;
      bool foundRoom;
      if (shop_type != SHOP_TYPE::PRIZE_OR_ANKH) {
        if (foundRoom = trySetRoom(ROOM_TYPE::SHOP, roomOut)) {
          char *shopTile = strchr(roomOut, 'S');
          if (shopTile != NULL) {
            if (roomType == ROOM_TYPE::SHOP_LEFT_FACING &&
                (int)shop_type < (int)SHOP_TYPE::KISSING) {
              *shopTile = '0';
              shopTile[3] = 'S';
            } else if (shop_type == SHOP_TYPE::KISSING) {
              *shopTile = '0';
              shopTile[2] = 'S';
            } else if (shop_type == SHOP_TYPE::HIREDHAND) {
              // three hh, two hh, one hh
              auto replaceTiles = rand() % 100 == 0  ? "0SSS"
                                  : rand() % 20 == 0 ? "0S0S"
                                                     : "00S0";
              strncpy(shopTile, replaceTiles, 4);
            }
          }
        }
      } else {
        if (globalState->is_blackmarket && roomIndex == 11) {
          foundRoom = trySetRoom(ROOM_TYPE::SHOP_ANKH, roomOut);
        } else {
          foundRoom = trySetRoom(ROOM_TYPE::SHOP_PRIZE, roomOut);
        }
      }
      if (foundRoom && roomType == ROOM_TYPE::SHOP_LEFT_FACING) {
        for (size_t i = 0; i < 80; i += 10) {
          std::reverse(roomOut + i, roomOut + i + 9);
        }
      }
      return foundRoom;
      // return trySetRoom(roomType, roomOut);
    } else {
      return trySetRoom(roomType, roomOut);
    }
  }
  return false;
}

void readCustomLevelFile() {
  readFileLevel("level.lvl", rooms);
}
