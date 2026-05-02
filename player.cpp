#include "player.h"
#include "skills.h"
#include "items.h"
#include "showdata.h" // showdata integrated: render player info in a centered window
#include <iostream>
#include <algorithm>
#include <sstream>
using namespace std;
static const char* ANSI_RED = "\033[31m";
static const char* ANSI_BLUE = "\033[34m";
static const char* ANSI_ORANGE = "\033[38;5;208m"; // orange on ANSI-capable terminals
static const char* ANSI_RESET = "\033[0m";
bool hasRelic(const Player& p, int id) {//checks if the player has a relic with the given id
    //inputs are player and relic id, output is true if the player has a relic with the given id, false otherwise
    return find(p.relics.begin(), p.relics.end(), id) != p.relics.end();
}

int countRelic(const Player& p, int id) {//counts how many copies of a relic with the given id the player has
    //input player and relic id, output the number of copies of the relic with the given id
    return (int)count(p.relics.begin(), p.relics.end(), id);
}

int damageRelicBonus(const Player& p) {//returns the damage bonus from relics, currently only from Thorned Armor
    //input player
    return hasRelic(p, 9) ? 3 : 0;
}

int goldRelicBonus(const Player& p) {//returns the gold bonus from relics, currently only from War Banner
    //input player
    return hasRelic(p, 10) ? 8 : 0;
}

int potionHealBonus(const Player& p) {//returns the healing bonus from potions, currently only from Alchemy Ring
    //input player
    return hasRelic(p, 11) ? 5 : 0;
}

bool hasSkill(const Player& p, int id) {//checks if the player has learned a skill with the given id
    //input player and skill id, output bool
    return find(p.learnedSkills.begin(), p.learnedSkills.end(), id) != p.learnedSkills.end();
}

void showPlayerStatus(const Player& p) {//displays the player's current status, including HP, energy, level, EXP, gold, skills, potions and relics
    //input player, output is the player's current status
    const auto& skills = getAllSkills();
    const auto& pots = getAllPotions();
    const auto& relics = getAllRelics();

    stringstream ss;
    ss << "===== PLAYER =====\n";
    ss << "HP " << ANSI_RED << p.hp << "/" << p.maxHp << ANSI_RESET << "         "<<"\n";
    ss << "Energy " << ANSI_ORANGE << p.maxEnergy << ANSI_RESET << "              "<<"\n";
    ss << "Level " << p.level << "  EXP " << p.exp << "/" << p.level * 100 << "\n";
    ss << "Skill Points " << p.skillPoints << "\n";
    ss << "Gold " << p.gold << "\n";
    ss << "Skill Slots " << p.skillSlots << "\n";
    ss << "Potion Slots " << p.potionSlots << " (" << p.potions.size() << "/" << p.potionSlots << ")\n";
    if (p.grandmaSkipsLeft > 0) ss << "Grandma's Charm skips left: " << p.grandmaSkipsLeft << "\n";

    ss << "\nSkills:\n";
    if (p.learnedSkills.empty()) ss << " none\n";
    for (int id : p.learnedSkills) {
        const auto& s = skills[id];
        ss << " - " << s.name << " [cost " << s.energyCost << ", " << branchName(s.branch) << "] " << s.desc << "\n";
    }

    ss << "\nPotions:\n";
    if (p.potions.empty()) ss << " none\n";
    for (int i = 0; i < (int)p.potions.size(); ++i) {
        ss << " [" << i + 1 << "] " << pots[p.potions[i]].name << " - " << pots[p.potions[i]].desc << "\n";
    }

    ss << "\nRelics:\n";
    if (p.relics.empty()) ss << " none\n";
    for (int id : p.relics) {
        ss << " - " << relics[id].name << " - " << relics[id].desc << "\n";
    }

    // showdata: display player stats inside a centered ASCII box
    showdata::showCenteredBox(ss.str(), true);
}

void gainExp(Player& p, int v) {//gains the given amount of EXP and handles level up
    //input player and amount of EXP to gain, output player's new EXP and level
    p.exp += v;
    cout << "Gained " << v << " EXP.\n";
    while (p.exp >= p.level * 100) {
        p.exp -= p.level * 100;
        p.level++;
        p.skillPoints++;
        cout << "LEVEL UP! You are now level " << p.level << ". Skill point +1.\n";
        if (askYesNo("Open skill learning menu now?")) {
            p.skillPoints--;
            chooseSkillToLearn(p, false);
        }
    }
}

void chooseSkillToForget(Player& p, bool allowCancel) {//allows the player to choose a skill to forget
    //input player and a boolean whether to allow canceling, output is the player's new learned skills after forgetting one
    const auto& skills = getAllSkills();
    if (p.learnedSkills.empty()) {
        cout << "No skill can be forgotten.\n";
        return;
    }
    while (true) {
        cout << "\nChoose a skill to forget:\n";
        for (int i = 0; i < (int)p.learnedSkills.size(); ++i) {
            cout << "[" << i + 1 << "] " << skills[p.learnedSkills[i]].name << "\n";
        }
        int lim = (int)p.learnedSkills.size();
        if (allowCancel) {
            cout << "[" << lim + 1 << "] Cancel\n";
            int c = readIntInRange(1, lim + 1);
            if (c == lim + 1) return;
            cout << "Forgot " << skills[p.learnedSkills[c - 1]].name << ".\n";
            p.learnedSkills.erase(p.learnedSkills.begin() + (c - 1));
            return;
        }
        int c = readIntInRange(1, lim);
        cout << "Forgot " << skills[p.learnedSkills[c - 1]].name << ".\n";
        p.learnedSkills.erase(p.learnedSkills.begin() + (c - 1));
        return;
    }
}

void chooseSkillToLearn(Player& p, bool allowCancel) {//allows the player to choose a skill to learn
    //input player and a boolean whether to allow canceling, output is the player's new learned skills after learning one
    const auto& skills = getAllSkills();
    std::vector<int> choices;
    for (const auto& s : skills) if (canUnlockSkill(p, s)) choices.push_back(s.id);
    if (choices.empty()) {
        cout << "No unlockable skill available.\n";
        return;
    }
    while (true) {
        cout << "\nChoose a skill to learn:\n";
        for (int i = 0; i < (int)choices.size(); ++i) {
            const auto& s = skills[choices[i]];
            cout << "[" << i + 1 << "] " << s.name << " [cost " << s.energyCost << "] " << s.desc;
            if (s.prerequisite != -1) cout << " Requires " << skills[s.prerequisite].name;
            cout << "\n";
        }
        int lim = (int)choices.size();
        if (allowCancel) {
            cout << "[" << lim + 1 << "] Cancel\n";
            int c = readIntInRange(1, lim + 1);
            if (c == lim + 1) return;
            p.learnedSkills.push_back(choices[c - 1]);
            cout << "Learned " << skills[choices[c - 1]].name << "!\n";
            return;
        }
        int c = readIntInRange(1, lim);
        p.learnedSkills.push_back(choices[c - 1]);
        cout << "Learned " << skills[choices[c - 1]].name << "!\n";
        return;
    }
}

void unlockSkillsMenu(Player& p) {//opens the skill learning menu if the player has unspent skill points, allows the player to spend them
    //input player
    while (p.skillPoints > 0) {
        cout << "\n===== SKILL UP =====\n";
        cout << "You have " << p.skillPoints << " skill point(s).\n";
        chooseSkillToLearn(p, true);
        if (!askYesNo("Spend another skill point now?")) return;
        p.skillPoints--;
    }
}

void applyRelicPickup(Player& p, int id) {//applies the effect of picking up a relic with the given id to the player
    //input player and relic id, output is the player's new status after picking up the relic
    const auto& relics = getAllRelics();
    if (id < 0 || id >= (int)relics.size()) return;
    const auto& r = relics[id];

    if (r.type == RelicType::Backpack && p.backpackCount >= 2) {
        cout << "Backpack cannot be taken more than twice. Converted to 40 gold.\n";
        p.gold += 40;
        return;
    }
    if (r.type != RelicType::Backpack && hasRelic(p, id)) {
        cout << "Duplicate relic converted to 35 gold.\n";
        p.gold += 35;
        return;
    }

    p.relics.push_back(id);
    cout << "Obtained relic: " << r.name << " - " << r.desc << "\n";

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

void addPotionToInventory(Player& p, int id) {//applies the effect of picking up a potion with the given id to the player, which is adding the potion to the player's inventory if there is space, otherwise converting it to gold
    //input player and potion id, output is the player's new potion inventory after picking up the potion
    const auto& pots = getAllPotions();
    if ((int)p.potions.size() >= p.potionSlots) {
        cout << "Potion inventory full. " << pots[id].name << " is converted to 15 gold.\n";
        p.gold += 15;
        return;
    }
    p.potions.push_back(id);
    cout << "Obtained potion: " << pots[id].name << "\n";
}

Player makeNewPlayer() {//creates a new player with default stats and starting skills
    Player p;
    p.learnedSkills = {0, 1, 2, 10};//start with some basic skills: Strike, Defend, Charge and Bash
    return p;
}
