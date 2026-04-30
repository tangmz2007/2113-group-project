#ifndef MONSTER_H
#define MONSTER_H

#include "types.h"
#include "player.h"
#include <vector>
using namespace std;
struct MonsterTemplate {
    MonsterKind kind = MonsterKind::Slime;
    string name;
    bool elite = false;
    int maxHp = 50;
    int attack = 10;
    int blockValue = 10;
    int attackGrow = 0;
    int blockGrow = 0;
    int rewardExp = 30;
    int rewardGold = 20;
    std::vector<ActionType> actions;
};

struct MonsterState {
    MonsterTemplate base;
    int hp = 0;
    int block = 0;
    int attack = 0;
    int blockValue = 0;
    int poison = 0;
    bool retainBlock = false;
    bool chargingExplosion = false;
    int chargedHp = 0;
};

MonsterTemplate makeMonsterTemplate(MonsterKind kind);
MonsterState makeMonsterState(const MonsterTemplate& mt, Difficulty d);
MonsterState generateMonster(int layer, bool elite, bool boss, const Player& p);
ActionType chooseMonsterAction(const MonsterState& m);
string actionIntentText(const MonsterState& m, ActionType a);

#endif
