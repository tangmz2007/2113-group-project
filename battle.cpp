#include "battle.h"
#include "items.h"
#include "showdata.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <sstream>
using namespace std;

int playerDamageAfterMods(const Player& p, const BattleContext& bc, int base) {
    // damage relics and buffs/debuffs are applied multiplicatively, and damage is floored at the end
    //inputs are player, battle context and base damage, output is the final damage after applying relics and buffs/debuffs
    int dmg = base + damageRelicBonus(p);
    if (bc.weakOnPlayer > 0) dmg = (int)floor(dmg * 0.75);
    dmg = (int)floor(dmg * bc.battleDamageMul + 1e-9);
    return max(0, dmg);
}

int playerBlockAfterMods(const BattleContext& bc, int base) {
    //block relics and buffs/debuffs are applied multiplicatively, and block is floored at the end
    //inputs are battle context and base block, output is the final block after applying relics
    return max(0, (int)floor(base * bc.battleBlockMul + 1e-9));
}

void damageMonster(MonsterState& m, int dmg) {
    //monster block is applied before monster HP, and if the monster is charging an explosion, it will lose its explosion if it takes any real HP damage
    //inputs are the monster state and the damage to be dealt, output is the updated monster
    int blocked = min(m.block, dmg);
    m.block -= blocked;
    dmg -= blocked;
    if (dmg > 0) {
        m.hp -= dmg;
        if (m.chargingExplosion) {
            m.chargingExplosion = false;
            cout << m.base.name << " loses real HP and its explosion is cancelled!\n";
        }
    }
}

void damagePlayer(Player& p, BattleContext& bc, MonsterState& m, int dmg) {
    //player block is applied before player HP, and if the player has the damage cap relic, damage is capped at 1
    //inputs are player, battle context, monster state and damage to be dealt, output is the updated player and battle context
    if (bc.damageCapOne && dmg > 1) dmg = 1;
    int blocked = min(bc.playerBlock, dmg);
    bc.playerBlock -= blocked;
    dmg -= blocked;
    if (dmg <= 0) {
        cout << "You block all damage.\n";
        return;
    }
    p.hp -= dmg;
    cout << "You take " << dmg << " damage.\n";
    if (hasRelic(p, 5) && m.hp > 0) {
        m.hp -= 5;
        cout << "Thorned Armor reflects 5 damage back!\n";
    }
    if (p.hp <= 0 && hasRelic(p, 12) && !p.phoenixUsed) {
        p.phoenixUsed = true;
        p.hp = 25;
        cout << "Phoenix Feather saves you! You return to 25 HP.\n";
    }
}

void applyEnemyPoison(MonsterState& m, BattleContext& bc) {
    //poison damage is applied at the end of the turn, and reduces by 1 each turn until it reaches 0
    //input is the monster state and battle context, output is the updated monster state and battle context
    if (bc.poisonOnEnemy > 0) {
        cout << m.base.name << " suffers " << bc.poisonOnEnemy << " poison damage.\n";
        m.hp -= bc.poisonOnEnemy;
        bc.poisonOnEnemy = max(0, bc.poisonOnEnemy - 1);
    }
}

void applyPlayerPoison(Player& p, BattleContext& bc, MonsterState& m) {
    //poison damage is applied at the end of the turn, and reduces by 1 each turn until it reaches 0
    //input is the player, battle context and monster state, output is the updated player and battle context
    if (bc.poisonOnPlayer > 0) {
        cout << "You suffer " << bc.poisonOnPlayer << " poison damage.\n";
        damagePlayer(p, bc, m, bc.poisonOnPlayer);
        bc.poisonOnPlayer = max(0, bc.poisonOnPlayer - 1);
    }
}

bool usePotionInBattle(Player& p, BattleContext& bc, MonsterState& m) {
    //allows the player to choose a potion to use in battle, and applies the potion's effect. returns true if a potion was used, false if the player canceled or has no potions.
    //input is the player, battle context and monster state, output is the updated player and battle context after using the potion, or no change if the player canceled or has no potions
    if (p.potions.empty()) {
        cout << "No potions available.\n";
        return false;
    }
    const auto& pots = getAllPotions();
    cout << "\nChoose a potion:\n";
    for (int i = 0; i < (int)p.potions.size(); ++i) {
        cout << "[" << i + 1 << "] " << pots[p.potions[i]].name << " - " << pots[p.potions[i]].desc << "\n";
    }
    cout << "[" << p.potions.size() + 1 << "] Cancel\n";
    int c = readIntInRange(1, (int)p.potions.size() + 1);
    if (c == (int)p.potions.size() + 1) return false;

    int idx = c - 1;
    PotionType type = pots[p.potions[idx]].type;
    string name = pots[p.potions[idx]].name;
    p.potions.erase(p.potions.begin() + idx);

    cout << "Used " << name << ".\n";
    switch (type) {
        case PotionType::Caffeine:
            bc.energy = p.maxEnergy;
            break;
        case PotionType::Serum:
            p.hp = min(p.maxHp, p.hp + 25 + potionHealBonus(p));
            break;
        case PotionType::Adrenaline:
            bc.battleDamageMul += 0.15;
            break;
        case PotionType::LiquidMetal:
            bc.battleBlockMul += 0.20;
            break;
        case PotionType::Entrench: {
            int cur = bc.playerBlock;
            bc.playerBlock *= 3;
            bc.retainBlock = true;
            bc.nextTurnBlock += bc.playerBlock;
            cout << "Your block is tripled from " << cur << " to " << bc.playerBlock << ".\n";
            break;
        }
        case PotionType::Oblivion:
            p.learnedSkills = {0, 1, 2, 10};
            sort(p.learnedSkills.begin(), p.learnedSkills.end());
            p.learnedSkills.erase(unique(p.learnedSkills.begin(), p.learnedSkills.end()), p.learnedSkills.end());
            p.nextBattleDummy = true;
            p.pendingForgetAfterDummy = true;
            cout << "Your learned skills are reset to starter skills. Your next battle will be against a dummy.\n";
            break;
        case PotionType::HealingDraught:
            p.hp = min(p.maxHp, p.hp + 15 + potionHealBonus(p));
            break;
        case PotionType::EnergyPotion:
            bc.energy += 2;
            break;
    }
    return true;
}

bool grantSpinsterEnergy(const Player& p, BattleContext& bc) {
    //if the player has the Spinster's Brooch relic, when they would lose their last energy for the turn, they gain 3 energy instead. returns true if energy was granted, false otherwise.  
    //inputs are player and battle context, output is the updated battle context with energy granted if the player has the relic and lost their last energy, or no change if not
    if (hasRelic(p, 4) && bc.energy == 0 && !bc.usedSpinsterThisTurn) {
        bc.energy += 3;
        bc.usedSpinsterThisTurn = true;
        cout << "Spinster triggers! Gain 3 energy.\n";
        return true;
    }
    return false;
}
static const char* ANSI_RED = "\033[31m";
static const char* ANSI_BLUE = "\033[34m";
static const char* ANSI_ORANGE = "\033[38;5;208m"; // orange on ANSI-capable terminals
static const char* ANSI_RESET = "\033[0m";

void showBattleHUD(const Player& p, const BattleContext& bc, const MonsterState& m) {
    //displays the battle HUD, showing player HP, block and energy, monster HP, block and statuses, and player statuses if any. 
    // also shows the monster's announced action and intent
    //inputs are player, battle context and monster state, output is the displayed battle HUD
    stringstream ss;
    ss << "Your HP " << ANSI_RED << p.hp << "/" << p.maxHp << ANSI_RESET
       << "  Block " << ANSI_BLUE << bc.playerBlock << ANSI_RESET
       << "  Energy " << ANSI_ORANGE << bc.energy << ANSI_RESET << "\n";
    ss << m.base.name << " HP " << ANSI_RED << max(0, m.hp) << "/" << m.base.maxHp << ANSI_RESET
       << "  Block " << ANSI_BLUE << m.block << ANSI_RESET;
    if (bc.vulnerableOnEnemy > 0) ss << "  Vulnerable(" << bc.vulnerableOnEnemy << ")";
    if (bc.poisonOnEnemy > 0) ss << "  Poison(" << bc.poisonOnEnemy << ")";
    ss << "\n";
    if (bc.poisonOnPlayer > 0 || bc.weakOnPlayer > 0 || bc.trapped || bc.damageCapOne) {
        ss << "Player statuses:";
        if (bc.poisonOnPlayer > 0) ss << " Poison(" << bc.poisonOnPlayer << ")";
        if (bc.weakOnPlayer > 0) ss << " Weak(" << bc.weakOnPlayer << ")";
        if (bc.trapped) ss << " Trapped";
        if (bc.damageCapOne) ss << " Apparition";
        ss << "\n";
    }

    // showdata: display battle hud inside a centered box
    showdata::showCenteredBox(ss.str(), false);
}

bool playerTurn(Player& p, BattleContext& bc, MonsterState& m) {
    //handles the player's turn, allowing them to choose actions until they end their turn or die. returns true if the player is still alive after their turn, false if they died. 
    //inputs are player, battle context and monster state, output is the updated player, battle context and monster state after the player's turn, and whether the player is still alive
    const auto& skills = getAllSkills();
    vector<int> activeSkills = p.learnedSkills;
    if ((int)activeSkills.size() > p.skillSlots) activeSkills.resize(p.skillSlots);
    vector<int> used(activeSkills.size(), 0);

    while (true) {
        if (m.hp <= 0) return true;
        showBattleHUD(p, bc, m);
        cout << actionIntentText(m, bc.announcedAction) << "\n";
        cout << "1 Use skill\n2 Use potion\n3 End turn\n";
        int act = readIntInRange(1, 3);

        if (act == 3) return true;

        if (act == 2) {
            usePotionInBattle(p, bc, m);
            if (m.hp <= 0) return true;
            continue;
        }

        if (activeSkills.empty()) {
            cout << "No usable skills.\n";
            continue;
        }

        cout << "\nChoose a skill:\n";
        for (int i = 0; i < (int)activeSkills.size(); ++i) {
            const auto& s = skills[activeSkills[i]];
            cout << "[" << i + 1 << "] " << s.name << " cost " << s.energyCost << " - " << s.desc
                      << " (used " << used[i] << "/2)\n";
        }
        cout << "[" << activeSkills.size() + 1 << "] Cancel\n";
        int c = readIntInRange(1, (int)activeSkills.size() + 1);
        if (c == (int)activeSkills.size() + 1) continue;
        int idx = c - 1;
        const auto& s = skills[activeSkills[idx]];

        if (used[idx] >= 2) {
            cout << "This skill has already been used twice this turn.\n";
            continue;
        }
        if (bc.trapped && (s.type == SkillType::Damage || s.id == 4 || s.id == 5 || s.id == 6 || s.id == 10 || s.id == 11)) {
            cout << "You are trapped and cannot use attack skills this turn.\n";
            continue;
        }

        int cost = s.energyCost;
        if (hasRelic(p, 13) && !bc.usedFreeFirstSkill) cost = 0;
        if (bc.energy < cost) {
            cout << "Not enough energy.\n";
            continue;
        }

        bc.energy -= cost;
        bc.usedFreeFirstSkill = true;
        used[idx]++;
        if (bc.energy == 0) grantSpinsterEnergy(p, bc);

        if (s.id == 0) {
            int dmg = playerDamageAfterMods(p, bc, s.value);
            if (bc.vulnerableOnEnemy > 0) dmg = (int)floor(dmg * 1.5);
            damageMonster(m, dmg);
            cout << s.name << " deals " << dmg << " damage.\n";
        } else if (s.id == 1) {
            int blk = playerBlockAfterMods(bc, s.value);
            bc.playerBlock += blk;
            cout << s.name << " gains " << blk << " block.\n";
        } else if (s.id == 2) {
            int before = p.hp;
            p.hp = min(p.maxHp, p.hp + s.value);
            cout << s.name << " heals " << p.hp - before << " HP.\n";
        } else if (s.id == 3) {
            int dmg = playerDamageAfterMods(p, bc, s.value);
            if (bc.vulnerableOnEnemy > 0) dmg = (int)floor(dmg * 1.5);
            damageMonster(m, dmg);
            cout << s.name << " deals " << dmg << " damage.\n";
        } else if (s.id == 4) {
            int dmg = playerDamageAfterMods(p, bc, s.value);
            if (bc.vulnerableOnEnemy > 0) dmg = (int)floor(dmg * 1.5);
            damageMonster(m, dmg);
            bc.vulnerableOnEnemy = max(bc.vulnerableOnEnemy, 2);
            cout << s.name << " deals " << dmg << " damage and makes the enemy vulnerable.\n";
        } else if (s.id == 5) {
            int dmg = playerDamageAfterMods(p, bc, s.value);
            if (bc.vulnerableOnEnemy > 0) dmg = (int)floor(dmg * 1.5);
            damageMonster(m, dmg);
            bc.poisonOnEnemy += 5;
            cout << s.name << " deals " << dmg << " damage and applies 5 poison.\n";
        } else if (s.id == 6) {
            int dmg = playerDamageAfterMods(p, bc, s.value);
            if (bc.vulnerableOnEnemy > 0) dmg = (int)floor(dmg * 1.5);
            damageMonster(m, dmg);
            cout << s.name << " explodes for " << dmg << " damage.\n";
        } else if (s.id == 7) {
            int before = p.hp;
            p.hp = min(p.maxHp, p.hp + s.value);
            cout << s.name << " heals " << p.hp - before << " HP.\n";
        } else if (s.id == 8) {
            int blk = playerBlockAfterMods(bc, s.value);
            bc.playerBlock += blk;
            bc.retainBlock = true;
            cout << s.name << " gains " << blk << " block and lets you keep block this turn.\n";
        } else if (s.id == 9) {
            bc.energy += 2;
            cout << s.name << " grants 2 energy.\n";
        } else if (s.id == 10) {
            int dmg = playerDamageAfterMods(p, bc, s.value);
            if (bc.vulnerableOnEnemy > 0) dmg = (int)floor(dmg * 1.5);
            damageMonster(m, dmg);
            cout << s.name << " deals " << dmg << " damage.\n";
        } else if (s.id == 11) {
            int dmg = (m.hp * 100 <= m.base.maxHp * 35) ? 40 : 20;
            dmg = playerDamageAfterMods(p, bc, dmg);
            if (bc.vulnerableOnEnemy > 0) dmg = (int)floor(dmg * 1.5);
            damageMonster(m, dmg);
            cout << s.name << " deals " << dmg << " damage.\n";
        } else if (s.id == 12) {
            bc.damageCapOne = true;
            cout << s.name << " is active. This turn, incoming damage becomes 1.\n";
        } else if (s.id == 13) {
            p.hp -= 12;
            if (p.hp <= 0 && hasRelic(p, 12) && !p.phoenixUsed) {
                p.phoenixUsed = true;
                p.hp = 25;
                cout << "Phoenix Feather saves you from Offering.\n";
            }
            bc.energy += 3;
            cout << s.name << " costs 12 HP and grants 3 energy.\n";
        } else if (s.id == 14) {
            p.maxHp = max(20, p.maxHp - 3);
            p.hp = min(p.maxHp, p.hp + 25);
            bc.energy += 2;
            bc.playerBlock += playerBlockAfterMods(bc, 25);
            cout << s.name << " lowers max HP by 3, then grants 25 HP, 2 energy and 25 block.\n";
        }

        if (bc.energy == 0) grantSpinsterEnergy(p, bc);
        if (m.hp <= 0) return true;
    }
}

void enemyTurn(Player& p, BattleContext& bc, MonsterState& m) {
    //handles the enemy's turn, applying their announced action and any relevant effects. 
    //inputs are player, battle context and monster state, output is the updated player, battle context and monster state after the enemy's turn
    ActionType a = bc.announcedAction;
    cout << "\n===== ENEMY TURN =====\n";

    if (m.hp <= 0) return;

    switch (a) {
        case ActionType::Attack:
            damagePlayer(p, bc, m, m.attack);
            break;
        case ActionType::Block:
            m.block += m.blockValue;
            cout << m.base.name << " gains " << m.blockValue << " block.\n";
            break;
        case ActionType::BuffAttack:
            m.attack += max(1, m.base.attackGrow);
            cout << m.base.name << " increases attack to " << m.attack << ".\n";
            break;
        case ActionType::BuffBlock:
            m.blockValue += max(1, m.base.blockGrow);
            cout << m.base.name << " increases block power to " << m.blockValue << ".\n";
            break;
        case ActionType::Trap:
            bc.trapped = true;
            cout << m.base.name << " traps you. You cannot use attack skills next turn.\n";
            break;
        case ActionType::PoisonSpit:
            bc.poisonOnPlayer += 4;
            cout << m.base.name << " spits poison. You gain 4 poison.\n";
            break;
        case ActionType::Heal:
            m.hp = min(m.base.maxHp, m.hp + 16);
            cout << m.base.name << " heals 16 HP.\n";
            break;
        case ActionType::DigDeep:
            m.block += 80;
            m.retainBlock = true;
            cout << m.base.name << " digs deep and gains 80 block.\n";
            break;
        case ActionType::ExplosiveCharge:
            if (!m.chargingExplosion) {
                m.chargingExplosion = true;
                m.chargedHp = m.hp;
                cout << m.base.name << " begins charging a 50-damage explosion! Break through its defense before next impact.\n";
            } else {
                if (m.hp < m.chargedHp) {
                    m.chargingExplosion = false;
                    cout << m.base.name << " lost HP during the charge. The explosion fizzles.\n";
                } else {
                    cout << m.base.name << " unleashes the explosion!\n";
                    damagePlayer(p, bc, m, 50);
                    m.chargingExplosion = false;
                }
            }
            break;
        case ActionType::MultiHit:
            cout << m.base.name << " attacks 3 times!\n";
            for (int i = 0; i < 3 && p.hp > 0; ++i) damagePlayer(p, bc, m, max(1, m.attack / 2));
            break;
        case ActionType::WeakCurse:
            bc.weakOnPlayer = max(bc.weakOnPlayer, 2);
            cout << m.base.name << " curses you with weakness for 2 turns.\n";
            break;
        case ActionType::Drain:
            cout << m.base.name << " drains life!\n";
            damagePlayer(p, bc, m, 14);
            m.hp = min(m.base.maxHp, m.hp + 10);
            cout << m.base.name << " heals 10 HP.\n";
            break;
        case ActionType::Roar:
            m.attack += 3;
            m.block += 10;
            cout << m.base.name << " roars and gains 3 attack and 10 block.\n";
            break;
    }

    if (!m.retainBlock) m.block = 0;
    else m.retainBlock = false;
}

bool doBattle(Player& p, int layer, bool elite, bool boss) {
    //handles the entire battle flow, from generating the monster to handling turns to giving rewards. returns true if the player wins, false if they lose.
    //inputs are player, layer number, whether it's an elite battle and whether it's a boss battle, output is whether the player wins the battle, and the updated player after the battle
    MonsterState m = generateMonster(layer, elite, boss, p);
    if (p.nextBattleDummy && m.base.kind == MonsterKind::Dummy) {
        cout << "\nOblivion changes this fight into a dummy battle.\n";
        p.nextBattleDummy = false;
    }

    BattleContext bc;
    bc.energy = p.maxEnergy;
    bc.freeFirstSkill = hasRelic(p, 13);

    cout << "\n===== BATTLE: " << m.base.name << " =====\n";
    if (elite) cout << "This is an elite battle.\n";
    if (boss) cout << "This is the final boss.\n";

    while (p.hp > 0 && m.hp > 0) {
        if (!bc.retainBlock) bc.playerBlock = 0;
        bc.playerBlock += bc.nextTurnBlock;
        bc.nextTurnBlock = 0;
        bc.retainBlock = false;
        bc.damageCapOne = false;
        bc.usedFreeFirstSkill = false;
        bc.usedSpinsterThisTurn = false;
        bc.energy = p.maxEnergy;
        bc.announcedAction = chooseMonsterAction(m);

        applyEnemyPoison(m, bc);
        if (m.hp <= 0) break;
        applyPlayerPoison(p, bc, m);
        if (p.hp <= 0) break;

        bool continueBattle = playerTurn(p, bc, m);
        (void)continueBattle;
        if (m.hp <= 0 || p.hp <= 0) break;

        enemyTurn(p, bc, m);
        if (bc.vulnerableOnEnemy > 0) bc.vulnerableOnEnemy--;
        if (bc.weakOnPlayer > 0) bc.weakOnPlayer--;
        bc.trapped = false;
    }

    if (p.hp <= 0) {
        cout << "You died in battle.\n";
        return false;
    }

    cout << "Victory!\n";
    gainExp(p, m.base.rewardExp);
    int goldGain = m.base.rewardGold + goldRelicBonus(p) + (elite ? 18 : 0) + (boss ? 40 : 0);
    p.gold += goldGain;
    cout << "Gained " << goldGain << " gold.\n";

    if (hasRelic(p, 1)) {
        int before = p.hp;
        p.hp = min(p.maxHp, p.hp + 3);
        cout << "Whiskey Bottle heals " << p.hp - before << " HP after battle.\n";
    }
    if (hasRelic(p, 6)) {
        p.maxHp += 3;
        p.hp += 3;
        cout << "Silver Fork raises max HP by 3.\n";
    }
    if (p.pendingForgetAfterDummy && m.base.kind == MonsterKind::Dummy) {
        p.pendingForgetAfterDummy = false;
        cout << "Oblivion aftermath: choose one learned skill to forget.\n";
        chooseSkillToForget(p, false);
    }

    return true;
}
