#ifndef BATTLE_H
#define BATTLE_H

#include "player.h"
#include "monster.h"
#include "items.h"
#include "skills.h"
#include "types.h"

struct BattleContext {
    int energy = 3;
    int playerBlock = 0;
    int nextTurnBlock = 0;
    bool retainBlock = false;
    bool damageCapOne = false;
    bool trapped = false;
    int poisonOnPlayer = 0;
    int weakOnPlayer = 0;
    int vulnerableOnEnemy = 0;
    int poisonOnEnemy = 0;
    double battleDamageMul = 1.0;
    double battleBlockMul = 1.0;
    bool freeFirstSkill = false;
    bool usedFreeFirstSkill = false;
    bool usedSpinsterThisTurn = false;
    ActionType announcedAction = ActionType::Attack;
};

int playerDamageAfterMods(const Player& p, const BattleContext& bc, int base);
int playerBlockAfterMods(const BattleContext& bc, int base);
void damageMonster(MonsterState& m, int dmg);
void damagePlayer(Player& p, BattleContext& bc, MonsterState& m, int dmg);
void applyEnemyPoison(MonsterState& m, BattleContext& bc);
void applyPlayerPoison(Player& p, BattleContext& bc, MonsterState& m);
bool usePotionInBattle(Player& p, BattleContext& bc, MonsterState& m);
bool grantSpinsterEnergy(const Player& p, BattleContext& bc);
void showBattleHUD(const Player& p, const BattleContext& bc, const MonsterState& m);
bool playerTurn(Player& p, BattleContext& bc, MonsterState& m);
void enemyTurn(Player& p, BattleContext& bc, MonsterState& m);
bool doBattle(Player& p, int layer, bool elite, bool boss);

#endif