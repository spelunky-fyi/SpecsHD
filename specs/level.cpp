#include <iostream>
#include <regex>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include "level.h"
#include "hd.h"

DWORD GetBaseAddress()
{
    return (DWORD)GetModuleHandleW(NULL);
}

using PreSpawnRoomsFunc = void* __stdcall (LevelState* levelState);
using RoomList = std::map<ROOM_TYPE, std::vector<std::string>>;

PreSpawnRoomsFunc* spawnBordertilesRoomsEtc = {nullptr};
DWORD getRoomGet = { GETROOM_GET_ROOM_OFF + GetBaseAddress() };
DWORD getRoomSpawn = { GETROOM_SPAWN_ROOM_OFF + GetBaseAddress() };
//GlobalState* globalState = { (GlobalState*)(*(DWORD*)(GLOBAL_STATE_OFF + GetBaseAddress())) };

RoomList rooms;

void readFileLevel(std::string filename, RoomList& ret) {
    ret.clear();
    std::ifstream levelFile(filename);
    std::string line_str;
    ROOM_TYPE current_room_id;
    std::string current_room = "";
    uint32_t room_line = 0;
    while (std::getline(levelFile, line_str))
    {
        line_str.erase(std::remove_if(line_str.begin(), line_str.end(), isspace), line_str.end());
        std::regex remove_comment("//.*");
        line_str = std::regex_replace(line_str, remove_comment, "");
        //std::cout << line_str << "\n";
        if (line_str.rfind("\\.", 0) == 0) {
            std::string room_template = line_str.substr(2, line_str.length());
            auto search = roomMap.find(room_template);
            if (search != roomMap.end()) {
                current_room_id = search->second;
            }
            else {
                //std::cout << "ERROR\n";
            }
        }
        else if (line_str.length() == 10) {
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

bool trySetRoom(ROOM_TYPE roomType, char* roomOut) {
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

bool __stdcall customRoomGet(int doorRoomType, int roomIndex, char* roomOut, LevelState* levelState) {
    ROOM_TYPE roomType = (ROOM_TYPE)levelState->room_types[roomIndex];
    ROOM_TYPE aboveRoom = roomIndex > 3 ? levelState->room_types[roomIndex - 4] : ROOM_TYPE::UNSET;
    if (doorRoomType == 1) {
        if (roomType == ROOM_TYPE::PATH_DROP) {
            return trySetRoom(ROOM_TYPE::ENTRANCE_DROP, roomOut);
        }
        else {
            return trySetRoom(ROOM_TYPE::ENTRANCE, roomOut);
        }
    }
    else if (doorRoomType == 2) {
        if (aboveRoom == ROOM_TYPE::PATH_DROP) {
            return trySetRoom(ROOM_TYPE::EXIT_NOTOP, roomOut);
        }
        else {
            return trySetRoom(ROOM_TYPE::EXIT, roomOut);
        }
    }
    else {
        GlobalState* globalState = levelState->global_state;
        if (roomType == ROOM_TYPE::SIDE) {
            if (globalState->level > 1)
            {
                if (globalState->altar_spawned == 0 && rand() % 14 == 0) {
                    globalState->altar_spawned = 1;
                    return trySetRoom(ROOM_TYPE::ALTAR, roomOut);
                }
                else if (globalState->idol_spawned == 0 && 11 < roomIndex && rand() % 10 == 0)
                {
                    globalState->idol_spawned = 1;
                    return trySetRoom(ROOM_TYPE::IDOL, roomOut);
                }
            }
            return trySetRoom(ROOM_TYPE::SIDE, roomOut);
        }
        else if (roomType == ROOM_TYPE::PATH_DROP &&
            (aboveRoom == ROOM_TYPE::PATH_DROP || aboveRoom == ROOM_TYPE::COFFIN_DROP)) {
            return trySetRoom(ROOM_TYPE::PATH_DROP_NOTOP, roomOut);
        }
        else if (roomType == ROOM_TYPE::SHOP_LEFT_FACING ||
                 roomType == ROOM_TYPE::SHOP_RIGHT_FACING)
        {
            //todo: more than 1 hh shop
            //use SHOP_SIDE_FACING if is a normal item shop, use the others if not
            int sum = (roomType == ROOM_TYPE::SHOP_LEFT_FACING) ? 100 : 10100;
            return trySetRoom((int)levelState->shop_type < (int)SHOP_TYPE::KISSING
                                ? roomType
                                : (ROOM_TYPE)((int)levelState->shop_type + sum),
                              roomOut);
        }
        else {
            return trySetRoom(roomType, roomOut);
        }
    }
    return false;
}

void __stdcall preSpawnRoomsHook(LevelState *levelState) {
    readFileLevel("level.lvl", rooms);
    spawnLevelTilesOriginal(levelState);
}

void __declspec(naked) spawnLevelTilesHook() {
    __asm {
        ;//original func code {
        sub esp, 0xbc
        push ebx
        mov ebx, dword ptr ss : [esp + 0xD0]
        push ebp
        mov ebp, dword ptr ss : [esp + 0xC8]
        mov eax, dword ptr ss : [ebp + ebx * 0x4 + 0xA5EC]
        push esi
        mov esi, dword ptr ss : [ebp + 0x1715C]
        push edi
        ;//}
        lea edi, dword ptr ss : [esp + 0x74] //room_dest
        push eax

        push ebp ;//level_state
        push edi ;//room_dest
        push ebx ;//room_index
        push edx ;//entrance_or_exit
        call customRoomGet
        ;//add esp, 0x10
        test al, al
        pop eax
        jz ifZero
        jmp getRoomSpawn
        ifZero:
        cmp dword ptr ds : [esi + 0x5C] , 0x16 ;//original func code
        jmp getRoomGet
    }
}