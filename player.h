#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"
#include <vector>

struct Player {
    int hp = 100;
    int maxHp = 100;
    int exp = 0;
    int level = 1;
    int skillPoints = 0;
    int gold = 60;
    int maxEnergy = 3;
    int skillSlots = 6;
    int potionSlots = 3;
    std::vector<int> learnedSkills;
    std::vector<int> potions;
    std::vector<int> relics;

    bool phoenixUsed = false;
    int backpackCount = 0;
    int grandmaSkipsLeft = 0;
    bool nextBattleDummy = false;
    bool pendingForgetAfterDummy = false;
    Difficulty difficulty = Difficulty::Normal; //difficulty setting for the run
};

struct RunState {
    bool valid = false;
    bool gameOver = false;
    int curLayer = 0;
    int curIndex = 0;
    std::string mapId = "default";
};

Player makeNewPlayer();
void showPlayerStatus(const Player& p);
void gainExp(Player& p, int v);
bool hasRelic(const Player& p, int id);
int countRelic(const Player& p, int id);
int damageRelicBonus(const Player& p);
int goldRelicBonus(const Player& p);
int potionHealBonus(const Player& p);
bool hasSkill(const Player& p, int id);
void chooseSkillToForget(Player& p, bool allowCancel);
void chooseSkillToLearn(Player& p, bool allowCancel);
void unlockSkillsMenu(Player& p);
void applyRelicPickup(Player& p, int id);
void addPotionToInventory(Player& p, int id);

#endif
