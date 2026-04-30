#include "types.h"
#include <iostream>
#include <sstream>
#include <limits>
#include <random>
using namespace std;
// already reviewed
//just tool functions



// Global random number generator
static mt19937 rng((random_device())());


// Returns a random integer in the range [l, r].
int randint(int l, int r) {
    uniform_int_distribution<int> dist(l, r);
    return dist(rng);
}


// Returns a random element from the given vector.
template <class T>
string joinVec(const vector<T>& v, char sep) {
    std::ostringstream oss;
    for (int i = 0; i < (int)v.size(); ++i) {
        if (i) oss << sep;
        oss << v[i];
    }
    return oss.str();
}

template string joinVec<int>(const vector<int>&, char);
template string joinVec<string>(const vector<string>&, char);

// Splits a string by the given separator and converts the parts to integers.
vector<int> splitInts(const string& s, char sep) {
    vector<int> out;
    if (s.empty()) return out;
    string item;
    std::stringstream ss(s);
    while (getline(ss, item, sep)) {
        if (!item.empty()) out.push_back(stoi(item));
    }
    return out;
}

//a basic function to read an integer n, either from user input or from a file, with error handling
int readIntInRange(int lo, int hi) {
    while (true) {
        cout << "> " << flush;
        int x;
        if (cin >> x) {
            if (x >= lo && x <= hi) return x;
            cout << "Please enter a number between " << lo << " and " << hi << ".\n";
            continue;
        }
        if (cin.eof()) {    //end of file
            cout << "\nInput ended. Program terminated.\n";
            exit(0);
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a valid number.\n";
    }
}


bool askYesNo(const string& prompt) {
    cout << prompt << "\n1 Yes\n2 No\n";
    return readIntInRange(1, 2) == 1;
}

string nodeName(NodeType t) {
    switch (t) {
        case NodeType::Start: return "Start";
        case NodeType::Monster: return "Monster";
        case NodeType::Elite: return "Elite";
        case NodeType::Shop: return "Shop";
        case NodeType::Campfire: return "Campfire";
        case NodeType::Chest: return "Chest";
        case NodeType::Event: return "Event";
        case NodeType::Boss: return "Boss";
    }
    return "?";
}

//at first short names were used for better map visualization, but then I decided to switch to full names for better readability, and did not remove this
string shortNodeName(NodeType t) {
    switch (t) {
        case NodeType::Start: return "Start";
        case NodeType::Monster: return "Monster";
        case NodeType::Elite: return "Elite";
        case NodeType::Shop: return "Shop";
        case NodeType::Campfire: return "Campfire";
        case NodeType::Chest: return "Chest";
        case NodeType::Event: return "???";
        case NodeType::Boss: return "Boss";
    }
    return "?";
}

string branchName(SkillBranch b) {
    switch (b) {
        case SkillBranch::Attack: return "Attack";
        case SkillBranch::Survival: return "Survival";
        case SkillBranch::Tactics: return "Tactics";
        case SkillBranch::Chaos: return "Chaos";
    }
    return "?";
}
