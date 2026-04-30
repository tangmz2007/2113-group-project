#ifndef ITEMS_H
#define ITEMS_H

#include "types.h"
#include <vector>
#include <string>
using namespace std;
struct Potion {
    int id = 0;
    string name;
    PotionType type = PotionType::HealingDraught;
    int price = 0;
    string desc;
};

struct Relic {
    int id = 0;
    string name;
    RelicType type = RelicType::ApplePie;
    int price = 0;
    string desc;
};

const vector<Potion>& getAllPotions();
const vector<Relic>& getAllRelics();

#endif