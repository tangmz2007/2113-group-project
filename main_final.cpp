#include "player.h"
#include "map.h"
#include "savesystem.h"
#include "battle.h"
#include "items.h"
#include "types.h"
#include <iostream>
#include <algorithm>
#include <random>
#include "showdata.h"
#include <ctime>

using namespace std;

void local_seed_generator(unsigned int& seed) {
    //allows user to choose a random seed or input a fixed seed. 
    //outputs the seed being used.
    cout << "Generating game map...\n";
    cout << "press 1 for random seed, 0 for fixed seed: " << flush;
    int seed_choice;
    cin >> seed_choice;
    if (seed_choice == 0) {
        cout << "enter seed value: " << flush;
        cin >> seed;
    } else {
        seed = (unsigned int)time(NULL);
        cout << "Using random seed: " << seed << "\n";
    }
    srand(seed);
}

void chooseDifficulty(Player& p) {
    //allows user to choose the difficulty level for the run.
    cout << "\nChoose difficulty:\n";
    cout << "1 Easy (Monsters have 75% stats)\n";
    cout << "2 Normal (Monsters have 100% stats)\n";
    cout << "3 Hard (Monsters have 135% stats)\n";
    int choice = readIntInRange(1, 3);
    switch (choice) {
        case 1:
            p.difficulty = Difficulty::Easy;
            cout << "Difficulty set to EASY.\n";
            break;
        case 2:
            p.difficulty = Difficulty::Normal;
            cout << "Difficulty set to NORMAL.\n";
            break;
        case 3:
            p.difficulty = Difficulty::Hard;
            cout << "Difficulty set to HARD.\n";
            break;
    }
}
void doShop(Player& p);
void doCampfire(Player& p);
void doChest(Player& p);
void doEvent(Player& p);
bool handleCurrentNode(Player& p, GameMap& map, RunState& rs);

void doShop(Player& p) {
    //handles the shop node. allows the player to buy potions and relics, or leave.
    //input player, outputs the result of the shop interaction
    const auto& pots = getAllPotions();
    const auto& relics = getAllRelics();
    while (true) {
        cout << "\n===== SHOP =====\nGold: " << p.gold << "\n";
        cout << "1 Buy potion\n2 Buy relic\n3 Leave\n";
        int c = readIntInRange(1, 3);
        if (c == 3) return;
        if (c == 1) {
            cout << "\nPotions for sale:\n";
            for (int i = 0; i < (int)pots.size(); ++i) {
                cout << "[" << i + 1 << "] " << pots[i].name << " - Price " << pots[i].price << " - " << pots[i].desc << "\n";
            }
            cout << "[" << pots.size() + 1 << "] Cancel\n";
            int x = readIntInRange(1, (int)pots.size() + 1);
            if (x == (int)pots.size() + 1) continue;
            const auto& pot = pots[x - 1];
            if (p.gold < pot.price) {
                cout << "Not enough gold.\n";
                continue;
            }
            p.gold -= pot.price;
            addPotionToInventory(p, pot.id);
        } else {
            vector<int> pool;
            for (int i = 0; i < (int)relics.size(); ++i) {
                if (relics[i].type == RelicType::Backpack) {
                    if (p.backpackCount < 2) pool.push_back(i);
                } else if (!hasRelic(p, i)) pool.push_back(i);
            }
            if (pool.empty()) {
                cout << "No relic available.\n";
                continue;
            }
            // 1. 生成真正的随机种子
            std::random_device rd;
            std::mt19937 rng(rd());
            std::shuffle(pool.begin(), pool.end(), rng);
            int offerCount = std::min(4, (int)pool.size());
            cout << "\nRelic offers:\n";
            for (int i = 0; i < offerCount; ++i) {
                const auto& r = relics[pool[i]];
                cout << "[" << i + 1 << "] " << r.name << " - Price " << r.price << " - " << r.desc << "\n";
            }
            cout << "[" << offerCount + 1 << "] Cancel\n";
            int x = readIntInRange(1, offerCount + 1);
            if (x == offerCount + 1) continue;
            const auto& r = relics[pool[x - 1]];
            if (p.gold < r.price) {
                cout << "Not enough gold.\n";
                continue;
            }
            p.gold -= r.price;
            applyRelicPickup(p, r.id);
        }
    }
}

void doCampfire(Player& p) {
    //handles the campfire node. allows the player to rest, train or meditate.
    //input player, outputs the result of the campfire interaction
    std::cout << "\n===== CAMPFIRE =====\n";
    std::cout << "1 Rest (+30 HP)\n2 Train (+35 EXP)\n3 Meditate (forget one skill, then learn one skill)\n";
    int c = readIntInRange(1, 3);
    if (c == 1) {
        int before = p.hp;
        p.hp = min(p.maxHp, p.hp + 30);
        std::cout << "Recovered " << p.hp - before << " HP.\n";
    } else if (c == 2) {
        gainExp(p, 35);
    } else {
        chooseSkillToForget(p, true);
        chooseSkillToLearn(p, true);
    }
}

void doChest(Player& p) {
    //handles the chest node. gives the player a random reward of either potions, relics or gold.
    //input player, outputs the result of opening the chest
    cout << "\n===== CHEST =====\n";
    int r = randint(1, 100);
    if (r <= 35) {
        addPotionToInventory(p, randint(0, (int)getAllPotions().size() - 1));
        addPotionToInventory(p, randint(0, (int)getAllPotions().size() - 1));
    } else if (r <= 75) {
        vector<int> pool;
        for (int i = 0; i < (int)getAllRelics().size(); ++i) {
            if (getAllRelics()[i].type == RelicType::Backpack) {
                if (p.backpackCount < 2) pool.push_back(i);
            } else if (!hasRelic(p, i)) pool.push_back(i);
        }
        if (pool.empty()) {
            p.gold += 45;
            cout << "The chest turns into 45 gold.\n";
            return;
        }
        int id = pool[randint(0, (int)pool.size() - 1)];
        applyRelicPickup(p, id);
    } else {
        int goldGain = randint(35, 70);
        p.gold += goldGain;
        cout << "You find " << goldGain << " gold.\n";
    }
}

void doEvent(Player& p) {
    //handles the event node. triggers a random event that can have various effects on the player.
    //input player, outputs the result of the event
    cout << "\n===== EVENT =====\n";
    int e = randint(1, 5);
    if (e == 1) {
        cout << "A shrine offers power for blood.\n";
        cout << "1 Lose 10 max HP, gain 2 skill points\n2 Leave\n";
        int c = readIntInRange(1, 2);
        if (c == 1) {
            p.maxHp = std::max(30, p.maxHp - 10);
            p.hp = std::min(p.hp, p.maxHp);
            p.skillPoints += 2;
            cout << "You gain 2 skill points.\n";
        }
    } else if (e == 2) {
        cout << "A trader offers strange brews.\n";
        addPotionToInventory(p, randint(0, (int)getAllPotions().size() - 1));
    } else if (e == 3) {
        cout << "An old warrior shares techniques.\n";
        chooseSkillToLearn(p, true);
    } else if (e == 4) {
        cout << "You find a loose purse. Gain 40 gold.\n";
        p.gold += 40;
    } else {
        cout << "A cursed idol grants immediate strength.\n";
        p.hp = std::min(p.maxHp, p.hp + 15);
        p.gold += 20;
        cout << "Heal 15 HP and gain 20 gold.\n";
    }
}

bool handleCurrentNode(Player& p, GameMap& map, RunState& rs) {
    //handles the current node the player is on. 
    //inputs are the player, the game map and the run state. outputs whether the player can continue playing (true) or if they should return to the main menu (false).
    const Node& cur = map.getNode(rs.curLayer, rs.curIndex);

    if (!(rs.curLayer == 0 && cur.actualType == NodeType::Start)) {
        bool alive = true;
        if (cur.actualType == NodeType::Monster) {
            if (p.grandmaSkipsLeft > 0) {
                p.grandmaSkipsLeft--;
                cout << "Grandma's Charm lets you skip this monster room.\n";
            } else alive = doBattle(p, rs.curLayer, false, false);
        }
        else if (cur.actualType == NodeType::Elite) alive = doBattle(p, rs.curLayer, true, false);
        else if (cur.actualType == NodeType::Shop) doShop(p);
        else if (cur.actualType == NodeType::Campfire) doCampfire(p);
        else if (cur.actualType == NodeType::Chest) doChest(p);
        else if (cur.actualType == NodeType::Event) doEvent(p);
        else if (cur.actualType == NodeType::Boss) {
            alive = doBattle(p, rs.curLayer, true, true);
            if (alive) {
                cout << "\nYOU WIN! The run is complete.\n";
                rs.valid = false;
                return false;
            }
        }

        if (!alive) {
            rs.gameOver = true;
            cout << "\nGAME OVER\n";
            return false;
        }
    }

    if (cur.actualType == NodeType::Boss) {
        rs.valid = false;
        return false;
    }

    auto nexts = map.getNextNodes(rs.curLayer, rs.curIndex);
    if (nexts.empty()) {
        rs.valid = false;
        return false;
    }

    cout << "\nNext nodes:\n";
    for (int i = 0; i < (int)nexts.size(); ++i) {
        const Node& nxt = map.getNode(rs.curLayer + 1, nexts[i]);
        string shown = (nxt.hiddenAsQuestion && !nxt.revealed) ? "Unknown" : nodeName(nxt.actualType);
        cout << "[" << i + 1 << "] Layer " << rs.curLayer + 1 << " index " << nexts[i] << " - " << shown << "\n";
    }
    cout << "[" << nexts.size() + 1 << "] Save and return to main menu\n";
    int c = readIntInRange(1, (int)nexts.size() + 1);
    if (c == (int)nexts.size() + 1) return false;

    rs.curLayer++;
    rs.curIndex = nexts[c - 1];
    map.revealNode(rs.curLayer, rs.curIndex);
    return true;
}

int main() {
    //main game loop. handles the main menu and the game progression.
    ios::sync_with_stdio(false);//untie cin from cout for faster input
    cin.tie(nullptr);

    const string RUN_SAVE_FILE = "./run_save.txt";
    const string MAP_SNAPSHOT_FILE = "./map_snapshot.txt";

    while (true) {
        cout << "\n===== MAIN MENU =====\n";
        cout << "1 New game\n2 Continue saved run\n3 Restart on the same map after a loss\n4 Exit\n";
        int choice = readIntInRange(1, 4);
        if (choice == 4) break;

        Player p;
        GameMap map;
        RunState rs;

        if (choice == 1) {
            unsigned int seed;
            local_seed_generator(seed);
            p = makeNewPlayer();
            chooseDifficulty(p);
            map = GameMap(seed);
            saveMapSnapshot(MAP_SNAPSHOT_FILE, map);
            rs.valid = true;
            rs.gameOver = false;
            rs.curLayer = 0;
            rs.curIndex = 0;
            rs.mapId = "new_map";
            map.revealNode(0, 0);
        } else if (choice == 2) {
            if (!loadRun(RUN_SAVE_FILE, rs, p, map) || !rs.valid) {
                cout << "No valid saved run found.\n";
                continue;
            }
        } else {
            if (!restartSameMap(MAP_SNAPSHOT_FILE, map, p, rs)) {
                cout << "No same-map snapshot found.\n";
                continue;
            }
            chooseDifficulty(p);
        }

        while (true) {
            map.printMap(rs.curLayer, rs.curIndex);
            showPlayerStatus(p);
            bool keepPlaying = handleCurrentNode(p, map, rs);

            if (rs.gameOver) {
                cout << "\nAfter losing:\n1 Restart on the same map\n2 Return to main menu\n";
                int x = readIntInRange(1, 2);
                if (x == 1) {
                    if (!restartSameMap(MAP_SNAPSHOT_FILE, map, p, rs)) {
                        cout << "Failed to reload same map.\n";
                        break;
                    }
                    continue;
                }
                rs.valid = false;
                break;
            }

            if (!keepPlaying) {
                if (rs.valid) {
                    if (saveRun(RUN_SAVE_FILE, rs, p, map)) cout << "Run saved.\n";
                    else cout << "Failed to save run.\n";
                }
                break;
            }
        }
    }

    cout << "Goodbye.\n";
    return 0;
}
