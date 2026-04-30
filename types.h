#ifndef TYPES_H
#define TYPES_H
using namespace std;  
#include <vector>
#include <string>

enum class NodeType { Start, Monster, Elite, Shop, Campfire, Chest, Event, Boss };
enum class SkillType { Damage, Heal, Block, Utility };
enum class SkillBranch { Attack, Survival, Tactics, Chaos };
enum class PotionType {
    Caffeine, Serum, Adrenaline, LiquidMetal, Entrench,
    Oblivion, HealingDraught, EnergyPotion
};
enum class RelicType {
    ApplePie, WhiskeyBottle, Backpack, GrandmaCharm, Spinster,
    ThornedArmor, SilverFork, Notebook, BlueCrystal, WarBanner,
    LuckyCoin, AlchemyRing, PhoenixFeather, ClockworkHeart
};
enum class ActionType {
    Attack, Block, BuffAttack, BuffBlock, Trap, PoisonSpit,
    Heal, DigDeep, ExplosiveCharge, MultiHit, WeakCurse, Drain, Roar
};
enum class MonsterKind { Dummy, Slime, Golem, Witch, BanditCaptain, IronBeast, AbyssBoss };
enum class Difficulty { Easy, Normal, Hard };

struct Node { //should see map.cpp for details
    int layer = 0;
    int index = 0;
    NodeType actualType = NodeType::Monster; // the actual type of the node, which is determined when the player first visits the node
    bool hiddenAsQuestion = false;
    bool revealed = false;
    bool visited = false;
    vector<int> next; // indices of next nodes in the next layer
};

// Utility functions
int randint(int l, int r);
template <class T> string joinVec(const vector<T>& v, char sep = ',');
vector<int> splitInts(const std::string& s, char sep = ',');
int readIntInRange(int lo, int hi);
bool askYesNo(const string& prompt);
string nodeName(NodeType t);
string shortNodeName(NodeType t);
string branchName(SkillBranch b);
double getDifficultyMultiplier(Difficulty d);

#endif
