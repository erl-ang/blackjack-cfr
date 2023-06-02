// TODO: change these imports to modules (he'd like that)
#pragma once

#include <iostream>
#include <chrono>
#include <vector>
#include "Cfr.h"
#include "State.h"

class Game {

    public:
        Game(std::vector<int> startingCards, int maxHits, int cfrIterations);

        State constructTree();
        static void printRandomPath(State state);

        std::vector<int> startingCards;
        int maxHits;
};