#include "map.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <random>
#include <ctime>

void seed_generator(unsigned int& seed) {
    std::cout << "Generating game map...\n";
    std::cout << "press 1 for random seed, 0 for fixed seed: ";
    int seed_choice;
    std::cin >> seed_choice;
    if (seed_choice == 0) {
        std::cout << "enter seed value: ";
        std::cin >> seed;
    } else {
        seed = (unsigned int)time(NULL);
        std::cout << "Using random seed: " << seed << "\n";
    }
    srand(seed);
}

NodeType GameMap::randomNormalType(int layer) {
    int r = randint(1, 100);
    if (layer <= 2) {
        if (r <= 52) return NodeType::Monster;
        if (r <= 67) return NodeType::Shop;
        if (r <= 82) return NodeType::Campfire;
        if (r <= 100) return NodeType::Event;
          // no chests in the first 3 layers, no elites in the first 3 layers
    }
    if (r <= 45) return NodeType::Monster;
    if (r <= 58) return NodeType::Shop;
    if (r <= 72) return NodeType::Campfire;
    if (r <= 84) return NodeType::Event;
    if (r <= 94) return NodeType::Chest;
    return NodeType::Elite;
}

std::string GameMap::nodeDisplay(const Node& n, bool cur) const {
    auto fullName = [&](NodeType t) -> std::string {
        switch (t) {
            case NodeType::Start: return "start";
            case NodeType::Monster: return "monster";
            case NodeType::Elite: return "elite";
            case NodeType::Shop: return "shop";
            case NodeType::Campfire: return "campfire";
            case NodeType::Chest: return "chest";
            case NodeType::Event: return "???";
            case NodeType::Boss: return "boss";
        }
        return "unknown";
    };

    std::string base;
    if (n.hiddenAsQuestion && !n.revealed) base = "???";
    else base = fullName(n.actualType);

    if (cur) return "[@" + base + "]";
    if (n.visited) return "(" + base + "*)";
    return "(" + base + ")";
}

void GameMap::putText(std::vector<std::string>& canvas, int y, int x, const std::string& s) const {
    if (y < 0 || y >= (int)canvas.size()) return;
    int start = std::max(0, x - (int)s.size() / 2);
    for (int i = 0; i < (int)s.size() && start + i < (int)canvas[y].size(); ++i) {
        canvas[y][start + i] = s[i];
    }
}

void GameMap::drawPath(std::vector<std::string>& canvas, int y1, int x1, int y2, int x2) const {
    if (y1 == y2) return;
    int prevX = x1;
    for (int y = y1 + 1; y < y2; ++y) {
        if (y < 0 || y >= (int)canvas.size()) continue;
        float t = float(y - y1) / float(y2 - y1);
        int curX = int(std::round(x1 + (x2 - x1) * t));
        if (curX < 0 || curX >= CANVAS_W) continue;
        char ch;
        if (curX == prevX) ch = '|';
        else if (curX > prevX) ch = '\\';
        else ch = '/';
        if (canvas[y][curX] == ' ') canvas[y][curX] = ch;
        prevX = curX;
    }
}

int GameMap::preferredTarget(int curIndex, int curCount, int nextCount) const {
    if (curCount == 1) return nextCount / 2;
    double ratio = (double)curIndex / (curCount - 1);
    int mapped = (int)std::round(ratio * (nextCount - 1));
    return std::max(0, std::min(nextCount - 1, mapped));
}

void GameMap::buildPositions() {
    int L = (int)layers.size();
    posX.assign(L, {});
    rowY.assign(L, 0);
    for (int l = 0; l < L; ++l) {
        int cnt = (int)layers[l].size();
        posX[l].resize(cnt);
        rowY[l] = l * LAYER_GAP;
        int totalWidth = cnt * CELL_W;
        int left = (CANVAS_W - totalWidth) / 2 + CELL_W / 2;
        for (int i = 0; i < cnt; ++i) posX[l][i] = left + i * CELL_W;
    }
}

void GameMap::generateEdges() {
    int L = (int)layers.size();
    for (int l = 0; l < L - 1; ++l) {
        int curCount = (int)layers[l].size();
        int nextCount = (int)layers[l + 1].size();
        for (int i = 0; i < curCount; ++i) {
            auto& node = layers[l][i];
            node.next.clear();
            int center = preferredTarget(i, curCount, nextCount);
            node.next.push_back(center);
            if (nextCount > 1 && randint(1, 100) <= 45) {
                std::vector<int> cand;
                if (center - 1 >= 0) cand.push_back(center - 1);
                if (center + 1 < nextCount) cand.push_back(center + 1);
                std::random_device rd;
                std::mt19937 rng(rd());
                std::shuffle(cand.begin(), cand.end(), rng);
                if (!cand.empty()) node.next.push_back(cand[0]);
            }
            std::sort(node.next.begin(), node.next.end());
            node.next.erase(std::unique(node.next.begin(), node.next.end()), node.next.end());
        }

        std::vector<int> indeg(nextCount, 0);
        for (int i = 0; i < curCount; ++i) for (int v : layers[l][i].next) indeg[v]++;
        for (int j = 0; j < nextCount; ++j) {
            if (indeg[j] > 0) continue;
            int bestI = 0;
            int bestDist = 1e9;
            for (int i = 0; i < curCount; ++i) {
                int d = std::abs(preferredTarget(i, curCount, nextCount) - j);
                if (d < bestDist) {
                    bestDist = d;
                    bestI = i;
                }
            }
            layers[l][bestI].next.push_back(j);
            std::sort(layers[l][bestI].next.begin(), layers[l][bestI].next.end());
            layers[l][bestI].next.erase(std::unique(layers[l][bestI].next.begin(), layers[l][bestI].next.end()), layers[l][bestI].next.end());
        }
    }
}

GameMap::GameMap() : GameMap((unsigned int)time(NULL)) {}

GameMap::GameMap(unsigned int seed) {
    srand(seed);
    std::vector<int> shape = {1, 2, 4, 6, 7, 6, 4, 3, 2, 1};
    int L = (int)shape.size();
    layers.resize(L);
    for (int l = 0; l < L; ++l) {
        layers[l].resize(shape[l]);
        for (int i = 0; i < shape[l]; ++i) {
            layers[l][i].layer = l;
            layers[l][i].index = i;
            if (l == 0) layers[l][i].actualType = NodeType::Start;
            else if (l == 4 && i == shape[l] / 2) layers[l][i].actualType = NodeType::Chest;
            else if (l == L - 1) layers[l][i].actualType = NodeType::Boss;
            else layers[l][i].actualType = randomNormalType(l);
            layers[l][i].hiddenAsQuestion = !(layers[l][i].actualType == NodeType::Start || layers[l][i].actualType == NodeType::Boss) && randint(1, 100) <= 28;
        }
    }
    buildPositions();
    generateEdges();
}

void GameMap::revealNode(int l, int i) {
    if (l < 0 || l >= (int)layers.size() || i < 0 || i >= (int)layers[l].size()) return;
    layers[l][i].revealed = true;
    layers[l][i].visited = true;
}

const Node& GameMap::getNode(int l, int i) const { return layers[l][i]; }
std::vector<int> GameMap::getNextNodes(int l, int i) const { return layers[l][i].next; }
int GameMap::layerCount() const { return (int)layers.size(); }
int GameMap::rowSize(int l) const { return (int)layers[l].size(); }

void GameMap::printMap(int curL, int curI) const {
    int H = rowY.back() + 2;
    std::vector<std::string> canvas(H + 1, std::string(CANVAS_W, ' '));
    for (int l = 0; l < (int)layers.size() - 1; ++l) {
        for (int i = 0; i < (int)layers[l].size(); ++i) {
            int x1 = posX[l][i], y1 = rowY[l] + 1;
            for (int ni : layers[l][i].next) {
                int x2 = posX[l + 1][ni], y2 = rowY[l + 1] - 1;
                drawPath(canvas, y1, x1, y2, x2);
            }
        }
    }
    for (int l = 0; l < (int)layers.size(); ++l) {
        for (int i = 0; i < (int)layers[l].size(); ++i) {
            putText(canvas, rowY[l], posX[l][i], nodeDisplay(layers[l][i], l == curL && i == curI));
        }
    }
    std::cout << "\n===== MAP =====\n";
    for (const auto& row : canvas) {
        size_t end = row.find_last_not_of(' ');
        if (end != std::string::npos) std::cout << row.substr(0, end + 1) << "\n";
    }
    std::cout << "Legend: [@X]=Current [X*]=Visited [?]=Unknown\n";
    std::cout << "Types: ST Start, M Monster, E Elite, S Shop, C Campfire, CH Chest, ? Event, BO Boss\n";
}

bool GameMap::save(std::ofstream& out) const {
    out << layerCount() << "\n";
    for (int l = 0; l < layerCount(); ++l) {
        out << rowSize(l) << "\n";
        for (int i = 0; i < rowSize(l); ++i) {
            const Node& n = layers[l][i];
            out << n.layer << ' ' << n.index << ' ' << (int)n.actualType << ' '
                << n.hiddenAsQuestion << ' ' << n.revealed << ' ' << n.visited << ' '
                << joinVec(n.next) << "\n";
        }
    }
    return true;
}

bool GameMap::load(std::ifstream& in) {
    int L;
    if (!(in >> L)) return false;
    std::string dummy;
    std::getline(in, dummy);
    layers.assign(L, {});
    for (int l = 0; l < L; ++l) {
        int cnt;
        if (!(in >> cnt)) return false;
        std::getline(in, dummy);
        layers[l].resize(cnt);
        for (int i = 0; i < cnt; ++i) {
            std::string line;
            std::getline(in, line);
            if (line.empty()) { --i; continue; }
            std::stringstream ss(line);
            Node n;
            int typeInt;
            if (!(ss >> n.layer >> n.index >> typeInt >> n.hiddenAsQuestion >> n.revealed >> n.visited)) return false;
            std::string nextStr;
            std::getline(ss >> std::ws, nextStr);
            n.actualType = (NodeType)typeInt;
            n.next = splitInts(nextStr);
            layers[l][i] = n;
        }
    }
    buildPositions();
    return true;
}
