#include "savesystem.h"
#include "types.h"
#include "player.h"
#include "map.h"
#include <fstream>
#include <iostream>
using namespace std;

bool saveRun(const string& filename, const RunState& rs, const Player& p, const GameMap& map) {
    //saves the current run state, player state and map state to a file. returns true if successful, false otherwise.
    ofstream out(filename);
    if (!out) return false;
    out << rs.mapId << "\n";
    out << rs.valid << ' ' << rs.gameOver << ' ' << rs.curLayer << ' ' << rs.curIndex << "\n";
    out << p.hp << ' ' << p.maxHp << ' ' << p.exp << ' ' << p.level << ' ' << p.skillPoints << ' ' << p.gold << ' '
        << p.maxEnergy << ' ' << p.skillSlots << ' ' << p.potionSlots << ' ' << p.phoenixUsed << ' '
        << p.backpackCount << ' ' << p.grandmaSkipsLeft << ' ' << p.nextBattleDummy << ' ' << p.pendingForgetAfterDummy << "\n";
    out << joinVec(p.learnedSkills) << "\n";
    out << joinVec(p.potions) << "\n";
    out << joinVec(p.relics) << "\n";
    map.save(out);
    return true;
}

bool loadRun(const string& filename, RunState& rs, Player& p, GameMap& map) {
    //loads the run state, player state and map state from a file. returns true if successful, false otherwise.
    //inputs are the filename to load from, and references to the run state, player state and map to load into.
    std::ifstream in(filename);
    if (!in) return false;
    getline(in, rs.mapId);
    if (!(in >> rs.valid >> rs.gameOver >> rs.curLayer >> rs.curIndex)) return false;
    if (!(in >> p.hp >> p.maxHp >> p.exp >> p.level >> p.skillPoints >> p.gold
          >> p.maxEnergy >> p.skillSlots >> p.potionSlots >> p.phoenixUsed
          >> p.backpackCount >> p.grandmaSkipsLeft >> p.nextBattleDummy >> p.pendingForgetAfterDummy)) return false;
    string dummy;
    getline(in, dummy);
    string line;
    getline(in, line); p.learnedSkills = splitInts(line);
    getline(in, line); p.potions = splitInts(line);
    getline(in, line); p.relics = splitInts(line);
    return map.load(in);
}

bool saveMapSnapshot(const string& filename, const GameMap& map) {
    //saves the current map state to a file. returns true if successful, false otherwise.
    //inputs are the filename to save to and the map to save.
    std::ofstream out(filename);
    if (!out) return false;
    return map.save(out);
}

bool loadMapSnapshot(const std::string& filename, GameMap& map) {
    //loads the map state from a file. returns true if successful, false otherwise.
    //inputs are the filename to load from and the map to load into.
    std::ifstream in(filename);
    if (!in) return false;
    return map.load(in);
}

bool restartSameMap(const string& snapshotFile, GameMap& map, Player& p, RunState& rs) {
    //restarts the game on the same map. returns true if successful, false otherwise.
    //inputs are the filename of the map snapshot to load, and references to the map, player and run state to load into.
    if (!loadMapSnapshot(snapshotFile, map)) return false;
    p = makeNewPlayer();
    rs.valid = true;
    rs.gameOver = false;
    rs.curLayer = 0;
    rs.curIndex = 0;
    rs.mapId = "same_map_restart";
    map.revealNode(0, 0);
    return true;
}
