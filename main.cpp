#include <iostream>
#include "hexBoard.h"

using namespace hex;
using namespace std;

void game(hexBoard &board, hexPlayer *white, hexPlayer *black) {
    cout << board << endl;
    bool done = false;
    bool flip = true;
    // Flip between each player printing the board in between
    while(!done) {
        if (flip) {
            cout << "white to play ";
            done = white->getMove();
            flip = !flip;
        } else {
            cout << "black to play ";
            done = black->getMove();
            flip = !flip;
        }
        cout << board << endl;
    }
}

int main() {
    int size;
    bool ai, first = true;
    // Intro Code
    cout << "Welcome to hex by MMA" << endl;
    cout << "white plays top down, black plays left right" << endl;
    cout << "Desired Board Size? "; cin >> size;
    cout << "Do you want AI? (yes: 1, no: 0) "; cin >> ai;
    if (ai) {cout << "Do you want to move first? (yes: 1, no: 0) "; cin >> first;}
    auto board = hexBoard(size);
    hexPlayer *white;
    hexPlayer *black;
    // Set up whose color based on selections
    if (first) {
        white = new hexPlayer(colors::white, &board, true);
        if (!ai) black = new hexPlayer(colors::black, &board, true);
        else black = new aiPlayer(colors::black, &board, 10000, false);
    } else {
        black = new hexPlayer(colors::white, &board, true);
        if (!ai) white = new hexPlayer(colors::black, &board, true);
        else white = new aiPlayer(colors::black, &board, 10000, false);
    }
    // Play the game
    game(board, white, black);
    delete white;
    delete black;
    return 0;
}
