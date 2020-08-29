#ifndef HEXBOARD
#define HEXBOARD

#include <vector>
#include <array>
#include <iostream>

using namespace std;

namespace hex
{
    enum class colors : short {blank, white, black};

    template<class T>
    int maxIndex(vector<T> vec);

    // A simplified bidirectional graph that initalizes
    class hexGraph {
    protected:
        vector<vector<int>> adjList;
        /*const*/ int numNodes;
    public:
        hexGraph(int numNodes);
        hexGraph() = default;
        friend ostream& operator<<(ostream&, const hexGraph&);
        inline void addEdge(int i, int j, bool bi = true);
        bool isEdge(int i, int j) const;
        bool isPath(int i, int j);
        void makePath(vector<int> parent, int j, vector<int> &path);
        int dijkstras (int s, int e, vector<int> &path);
    };

    class hexBoard: public hexGraph {
    protected:
        int moves = 0;
        vector<colors> colorList;
        /*const*/ int size;
        array<int, 2> toPos(int node) const;
        inline bool inBounds(array<int, 2> pos) const;
        inline bool inBounds(int r, int c) const;
        inline int toNode(array<int, 2> pos) const;
        inline int toNode(int r, int c) const;
        bool isLegal(int node) const;
        bool isLegal(int r, int c) const;
        bool swaped = false;
        friend class hexPlayer;
        friend class aiPlayer;
    public:
        hexBoard(int size=7);
        friend ostream& operator<<(ostream&, const hexBoard&);
    };

    class hexPlayer: public hexGraph {
    protected:
        vector<int> start;
        vector<int> end;
        vector<bool> nodes;
        hexBoard *board;
        colors team;
        void checkAround(int node);
        bool verbose;
    public:
        hexPlayer(colors team, hexBoard *board, bool verb);
        hexPlayer() = default;
        bool place(int node, bool update = true);
        bool place(int r, int c);
        bool hasWon(void);
        virtual bool getMove(void);
    };

    class aiPlayer: public hexPlayer {
    protected:
        int step();
        vector<int> getAvail();
        vector<int> evaluatePOS(hexPlayer orig, vector<int> avail);
        bool evaluateAvail(hexPlayer orig, const vector<int> &avail);
        int k;
        int iter;
    public:
        aiPlayer(colors team, hexBoard *board, int iter = 10, bool verb = false);
        bool getMove();
    };



    ostream& operator<<(ostream& out, const hexGraph& graph);
    ostream& operator<<(ostream& out, const hexBoard &board);
    ostream& operator<<(ostream& out, const colors &color);
    istream& operator>>(istream& in, colors &color);
}
#endif /* HEXBOARD */
