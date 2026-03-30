#ifndef PLAYER_H
#define PLAYER_H

#include "skill.h"
#include <vector>
#include "showData.h"
#include <string>
using namespace std;

class Player : public Character {
public:
    int HP;
    int maxEnergy;
    int currentEnergy;
    int level;
    int experience;
    int gold;
    vector<skill_node> skills;
	vector<string> inventory; // 存储物品名称的背包
    

public:
    Player();

    //energy system;
    void resetEnergy();
    int getEnergy();
    void gainMaxEnergy(int amount);
    void gainEnergy(int amount);

    //leveling system
    void gainExperience(int amount);
	void levelUp();

    //experience tree
    void learnSkill(skill_node skill[]);


    //gold system
    void earnGold(int amount)
    {
		gold += amount;
    }
    void spendGold(int amount)
    {
        if (gold >= amount)
        {
			gold -= amount;
        }
        else
        {
            showdata::showCenteredBox("No enough money !!!");
        }
    }

    //display
    void displayStatus() const;
    void showenergybar()const;
    



}





#endif
