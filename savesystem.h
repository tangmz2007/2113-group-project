#ifndef SAVESYSTEM_H
#define SAVESYSTEM_H

#include "player.h"
#include "map.h"
#include <string>

bool saveRun(const std::string& filename, const RunState& rs, const Player& p, const GameMap& map);
bool loadRun(const std::string& filename, RunState& rs, Player& p, GameMap& map);
bool saveMapSnapshot(const std::string& filename, const GameMap& map);
bool loadMapSnapshot(const std::string& filename, GameMap& map);
bool restartSameMap(const std::string& snapshotFile, GameMap& map, Player& p, RunState& rs);

#endif