#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"
#include <vector>
#include <memory>
#include <string>
using namespace std;

class Player : public Character {
private:
    int maxEnergy;
    int currentEnergy;
    int level;
    int experience;
    int gold;
    

public:
    Player(string name);
    
    //energy system;
    void resetEnergy()
    {
        currentEnergy = maxEnergy;
    }
    int useEnergy(int cost);
    int getEnergy() const
    {
        return currentEnergy;
    }
    void gainMainEnergy(int amount);
    void gainEnergy(int amount);

    //leveling system
    void gainExperience(int amount);
    void levelUp();

    //experience tree
    void learnSkill(shared_ptr<Skill> skill);
    vector<shared_ptr<Skill>> getSkills() const
    {
        return skills;
    }

    //gold system
    void earnGold(int amount);
    void spendGold(int amount);

    //display
    void displayStatus() const;
    void showenergybar()const;
    



}





#endif
