#include <vector>
#include <array>
#include <iostream>
#include <math.h>
#include <string>
#include <algorithm>
#include <utility>
#include <queue>
#include <limits>
#include <cassert>
#include <random>
#include <chrono>

#include "hexBoard.h"

using namespace hex;
using namespace std;

static int inf = 1000;
typedef pair<int, int> iPair;

template<class T>
int hex::maxIndex(vector<T> vec) {
    T best = vec[0];
    int index = 0;
    for (int i = 1; i < vec.size(); i ++) {
        if (vec[i] > best) {
            best = vec[i];
            index = i;
        }
    }
    return index;
}

//hexGraph
hexGraph::hexGraph(int numNodes): numNodes(numNodes), adjList(numNodes) {}
inline void hexGraph::addEdge(int i, int j, bool bi) {
    adjList[i].push_back(j);
    if (bi) adjList[j].push_back(i);
}
bool hexGraph::isEdge(int i, int j) const {
    auto pos = find(adjList[i].begin(), adjList[i].end(), j);
    return pos != adjList[i].end();
}
bool hexGraph::isPath(int i, int j) {
    vector<int> temp;
    return (dijkstras(i, j, temp) != inf);
}
void hexGraph::makePath(vector<int> parent, int j, vector<int> &path) {
    if (parent[j] == -2) {
        path.push_back(j);
        return;
    }
    makePath(parent, parent[j], path);
    path.push_back(j);
}
int hexGraph::dijkstras(int s, int e, vector<int> &path) {
    if (adjList[s].empty() || adjList[e].empty()) return inf;
    vector<int> dist(numNodes, inf);
    vector<int> parent(numNodes, -1);
    priority_queue<iPair, vector<iPair>, greater<iPair>> pq;
    dist[s] = 0;
    parent[s] = -2;
    pq.push(make_pair(dist[s], s));
    while (!pq.empty()) {
        int n = pq.top().second;
        pq.pop();
        for (auto neighbor: adjList[n]) {
            if (dist[neighbor] > dist[n] + 1) {
                dist[neighbor] = dist[n] + 1;
                parent[neighbor] = n;
                pq.push(make_pair(dist[neighbor], neighbor));
            }
        }
    }
    path.clear();
    if (dist[e] != inf) makePath(parent, e, path);
    return dist[e];
}

//hexBoard

hexBoard::hexBoard(int size/*=7*/): hexGraph(pow(size,2)),
                                    //white(hexPlayer(pow(size,2), size, true)),
                                    //black(hexPlayer(pow(size,2), size, false)),
                                    size(size),
                                    colorList(pow(size,2), colors::blank) {
    // Loop to build the graph going over each node
    for (int i = 0; i < numNodes; i++) {
        // Get r,c of this node
        array<int, 2> pos = toPos(i);
        // iterate over neighbors
        for (int k = -1; k <= 1; k++) {
            for (int z = -1; z <= 1; z++) {
                int r = pos[0]+k;
                int c = pos[1]+z;
                // if along the diag which violates hex skip
                if (z == k) continue;
                // Check that neighbor is in bounds
                if (inBounds(r,c)) {
                    // Check, we may need to define the vector first
                    addEdge(i, toNode(r, c), false);
                }
            }
        }
    }
}

array<int, 2> hexBoard::toPos(int node) const {
    array<int, 2> pos = {static_cast<int> (node/size), node%size};
    return pos;
}


inline bool hexBoard::inBounds(array<int, 2> pos) const {
    return inBounds(pos[0], pos[1]);
}
inline bool hexBoard::inBounds(int r, int c) const {
    return (r > -1 && r < size && c > -1 && c < size);
}

bool hexBoard::isLegal(int node) const {
    return inBounds(toPos(node)) && colorList[node] == colors::blank;
}
bool hexBoard::isLegal(int r, int c) const {
    return inBounds(r,c) && colorList[toNode(r,c)] == colors::blank;
}

inline int hexBoard::toNode(array<int,2> pos) const {
    return toNode(pos[0], pos[1]);
}
inline int hexBoard::toNode(int r, int  c) const {
    return r*size + c;
}

//hexPlayer
hexPlayer::hexPlayer(colors team, hexBoard *board, bool verb):
                    hexGraph(board->numNodes), nodes(board->numNodes, false),
                    board(board), team(team), verbose(verb) {
    int boardSize = board->size;
    bool top;
    switch (team) {
        case colors::white:
            top = true;
            break;
        case colors::black:
            top = false;
            break;
    }
    for (int i = 0; i < boardSize; i++) {
        if (top) {
            start.push_back(i);
            end.push_back(boardSize*(boardSize - 1) + i);
        } else {
            start.push_back(boardSize*i);
            end.push_back(boardSize*(i + 1) - 1);
        }
    }
}

void hexPlayer::checkAround(int node) {
    for (auto neighbor: board->adjList[node]) {
        if (nodes[neighbor]) addEdge(node, neighbor);
    }
}

bool hexPlayer::place(int node, bool update) {
    if(!board->isLegal(node)) return false;
    nodes[node] = true;
    if(update) {board->colorList[node] = team; board->moves++;}
    checkAround(node);
    return true;
}

bool hexPlayer::place(int r, int c) {
    return place(board->toNode(r,c));
}

bool hexPlayer::hasWon(void) {
    // For all start nodes
    for (auto s: start) {
        // check that start peice has connection
        if (adjList[s].empty()) continue;
        for (auto e: end) {
            // check that the end peice has a connection
            if (adjList[e].empty()) continue;
            // if there is path, game is won
            if (isPath(s,e)) return true;
        }
    }
    return false;
}

bool hexPlayer::getMove() {
    if (board->moves == 1 && !board->swaped) {
        bool swap = false;
        cout << "Do you want to swap? (yes: 1, no: 0) "; cin >> swap;
        if (swap) {
            int node = maxIndex<colors> (board->colorList);
            board->colorList[node] = team;
            nodes[node] = true;
            board->swaped = true;
            return false;
        }
    }
    if (board->moves == 1 && board->swaped) {
        int node = maxIndex<colors> (board->colorList);
        nodes[node] = false;
    }
    if (verbose) cout << "Placing a " << team << " at r, c" << endl;
    int r, c;
    bool valid = false;
    while (!valid) {
        cin >> r >> c;
        valid = place(board->toNode(r,c));
        cout << valid << endl;
    }
    if (hasWon()) {
        cout << endl << team << " has Won!" << endl;
        return true;
    }
    return false;
}

//aiPlayer

// NEED TO MOVE CHECK AROUND FUNCTION TO PLAYER SOMEHOW

aiPlayer::aiPlayer(colors team, hexBoard *board, int iter/*=100*/, bool verb):
                hexPlayer(team, board, verb), iter(iter) {};

bool aiPlayer::getMove() {
    k = (numNodes-board->moves+1)/2;
    if (board->moves == 1 && !board->swaped) {
        vector<int> all(numNodes, 0);
        for (int i = 0; i < numNodes; i++) all[i] = i;
        vector<int> winsSwap = evaluatePOS(*this, all);
        int node = maxIndex<colors> (board->colorList);
        cout << (double)winsSwap[node]/iter << endl;
        if ((double)winsSwap[node]/iter > .5) {
            board->colorList[node] = team;
            board->swaped = true;
            return false;
        }
    }
    if (board->moves == 1 && board->swaped) {
        int node = maxIndex<colors> (board->colorList);
        nodes[node] = false;
    }
    vector<int> wins = evaluatePOS(*this, getAvail());
    if (board->moves == 0) {
        for (auto i = wins.begin(); i != wins.end(); i++) {
            if (*i > iter/2) *i = *i - iter/2;
            else *i = -(*i - iter/2);
        }
    }
    int pos = maxIndex<int>(wins);
    place(pos);
    if (hasWon()) {
        cout << endl << team << " has Won!" << endl;
        return true;
    }
    return false;
}

int aiPlayer::step() {
    k = (numNodes-board->moves+1)/2;
    vector<int> avail = getAvail();
    vector<int> wins = evaluatePOS(*this, avail);
    return maxIndex<int>(wins);
}

vector<int> aiPlayer::getAvail() {
    vector<int> avail;
    for (int i = 0; i < board->colorList.size(); i++) {
        if (board->colorList[i] == colors::blank) avail.push_back(i);
    }
    return avail;
}

vector<int> aiPlayer::evaluatePOS(hexPlayer orig, vector<int> avail) {
    vector<int> wins(numNodes, 0);
    for (int i = 0; i < iter; i++) {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        auto gen = default_random_engine(seed);
        shuffle(avail.begin(), avail.end(), gen);
        if (evaluateAvail(orig, avail)) for (int i = 0; i < k; i++) wins[avail[i]]++;
    }
    return wins;
}

bool aiPlayer::evaluateAvail(hexPlayer orig, const vector<int> &avail) {
    for (int i = 0; i < k; i++) {
        orig.place(avail[i], false);
    }
    return orig.hasWon();
}

//ostream
ostream& hex::operator<<(ostream& out, const hexGraph &graph) {
    // Prints the adjList
    for (int i = 0; i < graph.numNodes; i++) {
        if (graph.adjList[i].empty()) continue;
        out << "Node " << i << " Connects to: ";
        for (auto p: graph.adjList[i]) {
            out << p << ' ';
        }
        out << endl;
    }
    return out;
}
ostream& hex::operator<<(ostream& out, const hexBoard &board) {
    // Prints the Board
    out << endl;
    for (int i = 0; i < board.size * 2 - 1; i++) {
        out << string(i+4, ' ');
        for (int c = 0; c < board.size; c++) {
            int r = i/2;
            if (i%2 == 0) {
                out << board.colorList[board.toNode(r,c)];
                if ((c+1)%board.size != 0) out << " - ";
            } else {
                if ((c+1)%board.size != 0) out << "\\ / ";
                else out << '\\';
            }
        }
        out << endl;
    }
    return out;
}
ostream& hex::operator<<(ostream& out, const colors &color) {
    switch (color) {
        case colors::blank:
            out << '.';
            break;
        case colors::white:
            out << 'X';
            break;
        case colors::black:
            out << 'O';
            break;
        default:
            out << '?';
    }
    return out;
}
//istream
istream& hex::operator>>(istream& in, colors &color) {
    int i;
    in >> i;
    switch (i) {
        case 1:
            color = colors::white;
            break;
        case 2:
            color = colors::black;
            break;
        default:
            color = colors::blank;
    }
    return in;
}
