#ifndef BATTLE_H
#define BATTLE_H

#include "Player.h"
#include "Enemy.h"
#include <memory>
#include <string>

class Battle {
private:
    Player* player;
    Enemy* currentEnemy;
    bool battleEnded;
    bool playerVictory;
    int turnCount;
    
    // 私有方法
    void startTurn();
    void playerTurn();
    void enemyTurn();
    void endBattle();
    void processEndOfTurnEffects();
    
    // UI辅助方法
    void showBattleStatus() const;
    void showSkills() const;
    int getSkillChoice() const;
    bool askContinue() const;
    void print(const std::string& message) const;
    void printSlow(const std::string& message, int delayMs = 30) const;
    void waitForEnter() const;
    void clearScreen() const;
    
public:
    Battle(Player* p, Enemy* e);
    ~Battle();
    
    // 主战斗循环
    void start();
    
    // Getter
    bool isBattleEnded() const { return battleEnded; }
    bool isPlayerVictory() const { return playerVictory; }
};

#endif // BATTLE_H
