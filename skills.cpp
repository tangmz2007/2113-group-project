#include "skills.h"
using namespace std;
const vector<Skill>& getAllSkills() { //a function that returns a reference to a vector of all skills, initialized on first call
    static const vector<Skill> skills = { //id, name, type, branch, value, energy cost, prerequisite skill id, description
        {0,  "Strike",        SkillType::Damage, SkillBranch::Attack,   12, 1, -1, "Deal 12 damage."},
        {1,  "Guard",         SkillType::Block,  SkillBranch::Survival, 10, 1, -1, "Gain 10 block."},
        {2,  "Heal",          SkillType::Heal,   SkillBranch::Survival, 14, 1, -1, "Recover 14 HP."},
        {3,  "Heavy Strike",  SkillType::Damage, SkillBranch::Attack,   24, 2,  0, "Deal 24 damage."},
        {4,  "Bash",          SkillType::Damage, SkillBranch::Tactics,  14, 1,  0, "Deal 14 damage and make enemy vulnerable."},
        {5,  "Poison Dart",   SkillType::Utility,SkillBranch::Tactics,   6, 1,  4, "Deal 6 damage and apply 5 poison."},
        {6,  "Fireball",      SkillType::Damage, SkillBranch::Chaos,    32, 3,  3, "Deal 32 damage."},
        {7,  "Greater Heal",  SkillType::Heal,   SkillBranch::Survival, 28, 2,  2, "Recover 28 HP."},
        {8,  "Fortress",      SkillType::Block,  SkillBranch::Survival, 20, 2,  1, "Gain 20 block. Your block remains this turn."},
        {9,  "Energy Surge",  SkillType::Utility,SkillBranch::Tactics,   0, 0,  4, "Gain 2 energy."},
        {10, "Rage Cut",      SkillType::Damage, SkillBranch::Chaos,    18, 1, -1, "Deal 18 damage."},
        {11, "Execute",       SkillType::Damage, SkillBranch::Chaos,    20, 2, 10, "Deal 20 damage, or 40 if enemy is below 35% HP."},
        {12, "Apparition",    SkillType::Utility,SkillBranch::Survival,  0, 1,  8, "All damage you take this turn becomes 1."},
        {13, "Offering",      SkillType::Utility,SkillBranch::Chaos,     0, 1, 10, "Lose 12 HP and gain 3 energy."},
        {14, "Corruption",    SkillType::Utility,SkillBranch::Chaos,     0, 2, 13, "Lose 3 max HP. Gain 25 HP, 2 energy and 25 block."}
    };
    return skills;
}

bool canUnlockSkill(const Player& p, const Skill& s) { //a function that checks if a skill can be unlocked by the player, based on whether they have the prerequisite skill and don't already have the skill
    if (hasSkill(p, s.id)) return false;
    if (s.prerequisite == -1) return true;
    return hasSkill(p, s.prerequisite);
}
