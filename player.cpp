#include "player.h"
#include "skills.h"
#include "items.h"
#include <iostream>
#include <algorithm>

bool hasRelic(const Player& p, int id) {
    return std::find(p.relics.begin(), p.relics.end(), id) != p.relics.end();
}

int countRelic(const Player& p, int id) {
    return (int)std::count(p.relics.begin(), p.relics.end(), id);
}

int damageRelicBonus(const Player& p) {
    return hasRelic(p, 9) ? 3 : 0;
}

int goldRelicBonus(const Player& p) {
    return hasRelic(p, 10) ? 8 : 0;
}

int potionHealBonus(const Player& p) {
    return hasRelic(p, 11) ? 5 : 0;
}

bool hasSkill(const Player& p, int id) {
    return std::find(p.learnedSkills.begin(), p.learnedSkills.end(), id) != p.learnedSkills.end();
}

void showPlayerStatus(const Player& p) {
    const auto& skills = getAllSkills();
    const auto& pots = getAllPotions();
    const auto& relics = getAllRelics();

    std::cout << "\n===== PLAYER =====\n";
    std::cout << "HP " << p.hp << "/" << p.maxHp << "\n";
    std::cout << "Energy " << p.maxEnergy << "\n";
    std::cout << "Level " << p.level << "  EXP " << p.exp << "/" << p.level * 100 << "\n";
    std::cout << "Skill Points " << p.skillPoints << "\n";
    std::cout << "Gold " << p.gold << "\n";
    std::cout << "Skill Slots " << p.skillSlots << "\n";
    std::cout << "Potion Slots " << p.potionSlots << " (" << p.potions.size() << "/" << p.potionSlots << ")\n";
    if (p.grandmaSkipsLeft > 0) std::cout << "Grandma's Charm skips left: " << p.grandmaSkipsLeft << "\n";

    std::cout << "\nSkills:\n";
    if (p.learnedSkills.empty()) std::cout << " none\n";
    for (int id : p.learnedSkills) {
        const auto& s = skills[id];
        std::cout << " - " << s.name << " [cost " << s.energyCost << ", " << branchName(s.branch) << "] " << s.desc << "\n";
    }

    std::cout << "\nPotions:\n";
    if (p.potions.empty()) std::cout << " none\n";
    for (int i = 0; i < (int)p.potions.size(); ++i) {
        std::cout << " [" << i + 1 << "] " << pots[p.potions[i]].name << " - " << pots[p.potions[i]].desc << "\n";
    }

    std::cout << "\nRelics:\n";
    if (p.relics.empty()) std::cout << " none\n";
    for (int id : p.relics) {
        std::cout << " - " << relics[id].name << " - " << relics[id].desc << "\n";
    }
}

void gainExp(Player& p, int v) {
    p.exp += v;
    std::cout << "Gained " << v << " EXP.\n";
    while (p.exp >= p.level * 100) {
        p.exp -= p.level * 100;
        p.level++;
        p.skillPoints++;
        std::cout << "LEVEL UP! You are now level " << p.level << ". Skill point +1.\n";
        if (askYesNo("Open skill learning menu now?")) {
            p.skillPoints--;
            chooseSkillToLearn(p, false);
        }
    }
}

void chooseSkillToForget(Player& p, bool allowCancel) {
    const auto& skills = getAllSkills();
    if (p.learnedSkills.empty()) {
        std::cout << "No skill can be forgotten.\n";
        return;
    }
    while (true) {
        std::cout << "\nChoose a skill to forget:\n";
        for (int i = 0; i < (int)p.learnedSkills.size(); ++i) {
            std::cout << "[" << i + 1 << "] " << skills[p.learnedSkills[i]].name << "\n";
        }
        int lim = (int)p.learnedSkills.size();
        if (allowCancel) {
            std::cout << "[" << lim + 1 << "] Cancel\n";
            int c = readIntInRange(1, lim + 1);
            if (c == lim + 1) return;
            std::cout << "Forgot " << skills[p.learnedSkills[c - 1]].name << ".\n";
            p.learnedSkills.erase(p.learnedSkills.begin() + (c - 1));
            return;
        }
        int c = readIntInRange(1, lim);
        std::cout << "Forgot " << skills[p.learnedSkills[c - 1]].name << ".\n";
        p.learnedSkills.erase(p.learnedSkills.begin() + (c - 1));
        return;
    }
}

void chooseSkillToLearn(Player& p, bool allowCancel) {
    const auto& skills = getAllSkills();
    std::vector<int> choices;
    for (const auto& s : skills) if (canUnlockSkill(p, s)) choices.push_back(s.id);
    if (choices.empty()) {
        std::cout << "No unlockable skill available.\n";
        return;
    }
    while (true) {
        std::cout << "\nChoose a skill to learn:\n";
        for (int i = 0; i < (int)choices.size(); ++i) {
            const auto& s = skills[choices[i]];
            std::cout << "[" << i + 1 << "] " << s.name << " [cost " << s.energyCost << "] " << s.desc;
            if (s.prerequisite != -1) std::cout << " Requires " << skills[s.prerequisite].name;
            std::cout << "\n";
        }
        int lim = (int)choices.size();
        if (allowCancel) {
            std::cout << "[" << lim + 1 << "] Cancel\n";
            int c = readIntInRange(1, lim + 1);
            if (c == lim + 1) return;
            p.learnedSkills.push_back(choices[c - 1]);
            std::cout << "Learned " << skills[choices[c - 1]].name << "!\n";
            return;
        }
        int c = readIntInRange(1, lim);
        p.learnedSkills.push_back(choices[c - 1]);
        std::cout << "Learned " << skills[choices[c - 1]].name << "!\n";
        return;
    }
}

void unlockSkillsMenu(Player& p) {
    while (p.skillPoints > 0) {
        std::cout << "\n===== SKILL UP =====\n";
        std::cout << "You have " << p.skillPoints << " skill point(s).\n";
        chooseSkillToLearn(p, true);
        if (!askYesNo("Spend another skill point now?")) return;
        p.skillPoints--;
    }
}

void applyRelicPickup(Player& p, int id) {
    const auto& relics = getAllRelics();
    if (id < 0 || id >= (int)relics.size()) return;
    const auto& r = relics[id];

    if (r.type == RelicType::Backpack && p.backpackCount >= 2) {
        std::cout << "Backpack cannot be taken more than twice. Converted to 40 gold.\n";
        p.gold += 40;
        return;
    }
    if (r.type != RelicType::Backpack && hasRelic(p, id)) {
        std::cout << "Duplicate relic converted to 35 gold.\n";
        p.gold += 35;
        return;
    }

    p.relics.push_back(id);
    std::cout << "Obtained relic: " << r.name << " - " << r.desc << "\n";

    switch (r.type) {
        case RelicType::ApplePie:
            p.maxHp += 18; p.hp += 18; break;
        case RelicType::Backpack:
            p.backpackCount++;
            p.skillSlots += 1;
            break;
        case RelicType::GrandmaCharm:
            p.grandmaSkipsLeft += 3;
            break;
        case RelicType::BlueCrystal:
            p.maxEnergy += 1;
            break;
        case RelicType::Notebook:
            chooseSkillToForget(p, true);
            chooseSkillToLearn(p, true);
            break;
        default:
            break;
    }
}

void addPotionToInventory(Player& p, int id) {
    const auto& pots = getAllPotions();
    if ((int)p.potions.size() >= p.potionSlots) {
        std::cout << "Potion inventory full. " << pots[id].name << " is converted to 15 gold.\n";
        p.gold += 15;
        return;
    }
    p.potions.push_back(id);
    std::cout << "Obtained potion: " << pots[id].name << "\n";
}

Player makeNewPlayer() {
    Player p;
    p.learnedSkills = {0, 1, 2, 10};
    return p;
}