#ifndef SKILLS_H
#define SKILLS_H

#include "types.h"
#include "player.h"
#include <vector>
using namespace std;
struct Skill { //id, name, type, branch, value, energy cost, prerequisite skill id, description/
    int id = 0;
    string name;
    SkillType type = SkillType::Damage;
    SkillBranch branch = SkillBranch::Attack;
    int value = 0;
    int energyCost = 1;
    int prerequisite = -1;
    string desc;
};

const vector<Skill>& getAllSkills();
bool canUnlockSkill(const Player& p, const Skill& s);

#endif
