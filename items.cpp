#include "items.h"

const std::vector<Potion>& getAllPotions() {
    static const std::vector<Potion> potions = {
        {0, "Caffeine",        PotionType::Caffeine,       50,  "Restore your energy to maximum."},
        {1, "Serum",           PotionType::Serum,         120,  "Gain 25 HP."},
        {2, "Adrenaline",      PotionType::Adrenaline,     60,  "+15% damage for this battle."},
        {3, "Liquid Metal",    PotionType::LiquidMetal,    55,  "+20% block for this battle."},
        {4, "Entrench",        PotionType::Entrench,      140,  "Triple current block, keep it, and gain the same amount next turn."},
        {5, "Oblivion",        PotionType::Oblivion,      250,  "Replace learned skills with starter skills. Next battle becomes a dummy fight, then forget one learned skill."},
        {6, "Healing Draught", PotionType::HealingDraught, 30,  "Recover 15 HP."},
        {7, "Energy Potion",   PotionType::EnergyPotion,   35,  "Gain 2 energy immediately."}
    };
    return potions;
}

const std::vector<Relic>& getAllRelics() {
    static const std::vector<Relic> relics = {
        {0,  "Apple Pie",        RelicType::ApplePie,       85,  "Upon pickup, raise your max HP by 18."},
        {1,  "Whiskey Bottle",   RelicType::WhiskeyBottle,  75,  "Gain 3 HP at the end of every battle."},
        {2,  "Backpack",         RelicType::Backpack,       95,  "Upon pickup, gain one skill slot. Can be picked up twice."},
        {3,  "Grandma's Charm", RelicType::GrandmaCharm,   80,  "The next 3 normal monster rooms are skipped."},
        {4,  "Spinster",         RelicType::Spinster,       70,  "Once each turn, when you reach exactly 0 energy, gain 3 energy."},
        {5,  "Thorned Armor",    RelicType::ThornedArmor,   85,  "Whenever you take damage, deal 5 back."},
        {6,  "Silver Fork",      RelicType::SilverFork,     95,  "Whenever you kill an enemy, gain 3 max HP."},
        {7,  "Notebook",         RelicType::Notebook,      100,  "Upon pickup, forget one skill and learn one skill."},
        {8,  "Blue Crystal",     RelicType::BlueCrystal,   105,  "Max energy +1."},
        {9,  "War Banner",       RelicType::WarBanner,      90,  "All damage +3."},
        {10, "Lucky Coin",       RelicType::LuckyCoin,      70,  "Battle gold +8."},
        {11, "Alchemy Ring",     RelicType::AlchemyRing,    82,  "Healing potions restore 5 more HP."},
        {12, "Phoenix Feather",  RelicType::PhoenixFeather,110,  "The first time you would die, survive with 25 HP."},
        {13, "Clockwork Heart",  RelicType::ClockworkHeart,108,  "The first skill you use each turn costs 0 energy."}
    };
    return relics;
}