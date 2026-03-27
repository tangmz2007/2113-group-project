#include <iostream>
#include <string>
using namespace std;

struct Slime {
    string type="small";
    int health=52;
    int base_damage=10;
};

struct Goblin {
    string type = "small";
    int health=35;
    int base_damage=15;
    int armor=5;
};

struct Giant {
    string type="elite";
    int health=100;
    int base_damage=20;
    int armor=10;
};
