#pragma once 

#include <vector>
#include "State.h"

class Cfr {
    public:
        Cfr(State& root, int iterations);

        void train();
        void normalize(State& state);
        std::vector<float> runCfr(std::vector<int>& downCards, State& state,
            std::vector<float>& probabilities, int iterationNum);
        std::map<std::string, float> getStrategy(std::vector<int>& downCards,
            State& state, std::vector<float>& probabilities, int iterationNum);
        std::vector<float> getTerminalNodePayoffs(std::vector<int>& downCards, State& state);

        static int getSum(int downCard, std::vector<int> upCards);
        static std::vector<int> generateDownCards();

        State root;
        int iterations;
};