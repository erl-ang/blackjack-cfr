// TODO: change these imports to modules (he'd like that)
#pragma once

#include <iostream>
#include <format>
#include <future>
#include <map>
#include <vector>
#include "Player.h"
#include <string>

/**
 * states are the game nodes
 * 
 * Should probs abstract this to InternalState and TerminalState where both
 * inherit from this class
 */

class State {
    public:

        State(int toAct, std::vector<Player>& players, bool isTerminal, 
            /* for debugging */ std::string pastActions,
            std::map<int, std::vector<int>>& upCards, int maxHits);

        State();

        void populateChildren();
        int getNumStates();
        State createStandState();
        State createHitState(int cardNum);
        void printDetails(); // debug
        void initializeMaps();

        int toAct;
        std::vector<Player> players;
        bool isTerminal;
        std::map<std::string, State> nextStates;
        std::string pastActions;
        std::map<int, std::vector<int>> upCards;
        int maxHits;
        std::map<int, std::map<std::string, float>> strategies;
        std::map<int, std::map<std::string, float>> regret;
        std::map<int, std::map<std::string, float>> strategy_sums;
};