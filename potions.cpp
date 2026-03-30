#include "potions.h"

Potion::Potion(string name, int cost, string rarity, Effect effect)
    : name(name), cost(cost), rarity(rarity), effect(effect) {
}

string Potion::getName() const {
    return name;
}

int Potion::getCost() const {
    return cost;
}

string Potion::getRarity() const {
    return rarity;
}

Effect Potion::getEffect() const {
    return effect;
}

string Potion::getDescription() const {
    string desc = name + " (" + rarity + ") - Cost: " + to_string(cost) + "\n";
    desc += "Effects: ";

    if (effect.health_restore > 0) desc += "Restore " + to_string(effect.health_restore) + " HP ";
    if (effect.energy_restore > 0) desc += "Restore " + to_string(effect.energy_restore) + " Energy ";
    if (effect.damage_boost > 0) desc += "+" + to_string(effect.damage_boost) + " Damage ";
    if (effect.block_boost > 0) desc += "+" + to_string(effect.block_boost) + " Block ";

    if (effect.health_restore == 0 && effect.energy_restore == 0 && effect.damage_boost == 0 && effect.block_boost == 0) {
        desc += "No effects";
    }

    return desc;
}

// 初始的几个基础药水
Potion getHealthPotion() {
    Effect effect;
    effect.health_restore = 50;
    return Potion("Health Potion", 25, "common", effect);
}

Potion getEnergyPotion() {
    Effect effect;
    effect.energy_restore = 80;
    return Potion("Energy Potion", 20, "common", effect);
}

Potion getStrengthPotion() {
    Effect effect;
    effect.damage_boost = 10;
    return Potion("Strength Potion", 40, "common", effect);
}

Potion getDefensePotion() {
    Effect effect;
    effect.block_boost = 25;
    return Potion("Defense Potion", 35, "common", effect);
}

vector<Potion> getAllPotions() {
    vector<Potion> potions;
    potions.push_back(getHealthPotion());
    potions.push_back(getEnergyPotion());
    potions.push_back(getStrengthPotion());
    potions.push_back(getDefensePotion());
    return potions;
}