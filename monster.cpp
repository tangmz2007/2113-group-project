#include "monster.h"
#include "player.h"
#include "types.h"
#include <vector>
#include <algorithm>
using namespace std;
MonsterTemplate makeMonsterTemplate(MonsterKind kind) {
    //returns a monster template based on the monster kind. inputs are monster kind, output is the corresponding monster template
    switch (kind) {
        case MonsterKind::Dummy://kind, name, elite, maxHp, attack, blockValue, attackGrow, blockGrow, rewardExp, rewardGold, actions
            return {kind, "Dummy", false, 60, 0, 0, 0, 0, 12, 12, {ActionType::Block}};
        case MonsterKind::Slime:
            return {kind, "Slime", false, 75, 20, 20, 5, 0, 32, 24, {ActionType::Attack, ActionType::Block, ActionType::Trap, ActionType::BuffAttack}};
        case MonsterKind::Golem:
            return {kind, "Golem", true, 150, 30, 40, 0, 8, 65, 50, {ActionType::Attack, ActionType::Block, ActionType::BuffBlock, ActionType::DigDeep, ActionType::ExplosiveCharge}};
        case MonsterKind::Witch:
            return {kind, "Witch", false, 95, 18, 12, 3, 0, 42, 35, {ActionType::PoisonSpit, ActionType::WeakCurse, ActionType::Heal, ActionType::Attack}};
        case MonsterKind::BanditCaptain:
            return {kind, "Bandit Captain", true, 130, 22, 18, 4, 2, 60, 62, {ActionType::Attack, ActionType::MultiHit, ActionType::Drain, ActionType::Roar}};
        case MonsterKind::IronBeast:
            return {kind, "Iron Beast", true, 170, 28, 24, 2, 4, 72, 64, {ActionType::Attack, ActionType::Block, ActionType::MultiHit, ActionType::DigDeep}};
        case MonsterKind::AbyssBoss:
            return {kind, "Abyss Lord", true, 260, 34, 30, 4, 3, 150, 160, {ActionType::Attack, ActionType::MultiHit, ActionType::WeakCurse, ActionType::ExplosiveCharge, ActionType::Roar, ActionType::Drain}};
    }
    return makeMonsterTemplate(MonsterKind::Slime);
}

MonsterState makeMonsterState(const MonsterTemplate& mt, Difficulty d) {
    //takes in a monster template and difficulty, returns a monster state with the same base and initialized hp, attack and block value.
    //applies difficulty multiplier to hp and attack
    double multiplier = getDifficultyMultiplier(d);
    MonsterState ms;
    ms.base = mt;
    ms.hp = (int)(mt.maxHp * multiplier);
    ms.attack = (int)(mt.attack * multiplier);
    ms.blockValue = (int)(mt.blockValue * multiplier);
    return ms;
}

MonsterState generateMonster(int layer, bool elite, bool boss, const Player& p) {
    //generates a monster state based on the layer number, whether it's an elite battle and whether it's a boss battle.
    if (p.nextBattleDummy) return makeMonsterState(makeMonsterTemplate(MonsterKind::Dummy), p.difficulty);
    if (boss) return makeMonsterState(makeMonsterTemplate(MonsterKind::AbyssBoss), p.difficulty);
    if (elite) {
        std::vector<MonsterKind> pool = {MonsterKind::Golem, MonsterKind::BanditCaptain, MonsterKind::IronBeast};
        return makeMonsterState(makeMonsterTemplate(pool[randint(0, (int)pool.size() - 1)]), p.difficulty);
    }
    vector<MonsterKind> pool;
    if (layer <= 2) pool = {MonsterKind::Slime, MonsterKind::Witch};
    else if (layer <= 5) pool = {MonsterKind::Slime, MonsterKind::Witch, MonsterKind::BanditCaptain};
    else pool = {MonsterKind::Witch, MonsterKind::Slime, MonsterKind::IronBeast};
    return makeMonsterState(makeMonsterTemplate(pool[randint(0, (int)pool.size() - 1)]), p.difficulty);
}

ActionType chooseMonsterAction(const MonsterState& m) {
    //chooses a random action from the monster's action list. output is a randomly chosen action from the monster's action list.
    return m.base.actions[randint(0, (int)m.base.actions.size() - 1)];
}

string actionIntentText(const MonsterState& m, ActionType a) {
    //returns a string describing the monster's intent based on the action it plans to take. 
    //inputs are the monster state and the action it plans to take, output is a string describing the monster's intent.
    switch (a) {
        case ActionType::Attack: return m.base.name + " intends to ATTACK for " + std::to_string(m.attack) + ".";
        case ActionType::Block: return m.base.name + " intends to gain " + std::to_string(m.blockValue) + " block.";
        case ActionType::BuffAttack: return m.base.name + " intends to increase its attack.";
        case ActionType::BuffBlock: return m.base.name + " intends to increase its block power.";
        case ActionType::Trap: return m.base.name + " intends to TRAP you.";
        case ActionType::PoisonSpit: return m.base.name + " intends to spit poison.";
        case ActionType::Heal: return m.base.name + " intends to heal itself.";
        case ActionType::DigDeep: return m.base.name + " intends to dig deep and fortify.";
        case ActionType::ExplosiveCharge: return m.base.name + " intends to charge a huge explosion.";
        case ActionType::MultiHit: return m.base.name + " intends to strike 3 times.";
        case ActionType::WeakCurse: return m.base.name + " intends to weaken you.";
        case ActionType::Drain: return m.base.name + " intends to drain life.";
        case ActionType::Roar: return m.base.name + " intends to roar and empower itself.";
    }
    return "Enemy intent unknown.";
}
