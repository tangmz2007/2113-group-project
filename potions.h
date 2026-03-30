
#ifndef POTIONS_H
#define POTIONS_H
#include <string>
#include <vector>
#include <memory>
using namespace std;

struct Effect {
    int health_restore; // 恢复生命值
    int energy_restore; // 回复能量
    int damage_boost;   // 增加伤害
    int block_boost;    // 增加格挡值

    Effect() : health_restore(0), energy_restore(0), damage_boost(0), block_boost(0) {}
};

class Potion {
public:
    Potion(string name, int cost, string rarity, Effect effect); //common, rare, legendary

    // 获取通用属性
    string getName() const;
    int getCost() const;
    string getRarity() const;

    // 获取药水效果
    Effect getEffect() const;

    // 获取药水描述
    string getDescription() const;

private:
    // 通用属性
    string name;
    int cost;           // 药水价格
    string rarity;      // 稀有度: common, rare, legendary

    // 药水效果
    Effect effect;
};

// 预定义药水获取函数
Potion getHealthPotion();
Potion getEnergyPotion();
Potion getStrengthPotion();
Potion getDefensePotion();
vector<Potion> getAllPotions();

#endif