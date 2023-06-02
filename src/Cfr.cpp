#include "Cfr.h"
#include "time.h"

Cfr::Cfr(State& root, int iterations) {
    this->root = root;
    this->iterations = iterations;
}

void Cfr::train() {
    std::vector<float> root_payoffs = {0, 0, 0};
    srand(time(0));

    for (int i = 1; i <= iterations; i++) {
        auto downCards = generateDownCards(); // generate randomly
        std::vector<float> probabilities = {1, 1, 1};
        if (i % 100 == 0) {
            //std::cout << i << " done" << std::endl;
        }
        const auto& payoffs = runCfr(downCards, root, probabilities, i);
        root_payoffs[0] += payoffs[0];
        root_payoffs[1] += payoffs[1];
        root_payoffs[2] += payoffs[2];
    }

    std::cout << "CFR TRAINING DONE" << std::endl;
    
    root_payoffs[0] /= iterations;
    root_payoffs[1] /= iterations;
    root_payoffs[2] /= iterations;

    std::cout << "ROOT PAYOFFS: " << std::endl;
    std::cout << "Player 0: " << root_payoffs[0] << std::endl;
    std::cout << "Player 1: " << root_payoffs[1] << std::endl;
    std::cout << "Dealer: " << root_payoffs[2] << std::endl;

    normalize(root);

    std::cout << std::endl;
    std::cout << "PRINTING P1 ROOT STRATEGY" << std::endl;
    for (int i = 1; i <= 10; i++) {
        std::cout << "Down Card: " << i << " || Stand: " << root.strategy_sums[i]["Stand"] << ", Hit: " << root.strategy_sums[i]["Hit"] << std::endl;
    }

    std::cout << std::endl;
    State& nextState = root.nextStates["s"];
    std::cout << "PRINTING P2 STRATEGY AFTER P1 Stand" << std::endl;
    for (int i = 1; i <= 10; i++) {
        std::cout << "Down Card: " << i << " || Stand: " << nextState.strategy_sums[i]["Stand"] << ", Hit: " << nextState.strategy_sums[i]["Hit"] << std::endl;
    }

    std::cout << std::endl;
    State& nextState1 = root.nextStates["h10"];
    std::cout << "PRINTING P2 STRATEGY AFTER P1 HITS and Gets a T " << std::endl;
    for (int i = 1; i <= 10; i++) {
        std::cout << "Down Card: " << i << " || Stand: " << nextState1.strategy_sums[i]["Stand"] << ", Hit: " << nextState1.strategy_sums[i]["Hit"] << std::endl;
    }
}

void Cfr::normalize(State& state) {
    if (state.isTerminal) {
        return;
    }
    for (auto it = state.strategy_sums.begin(); it != state.strategy_sums.end(); it++) {
        float normalizer = 0;
        const auto& downCard = it->first;
        auto& strategySums = it->second;
        for (auto action : {"Stand", "Hit"}) {
            normalizer += strategySums[action];
        }
        if (normalizer > 0) {
            for (auto action : {"Stand", "Hit"}) {
                state.strategy_sums[downCard][action] = strategySums[action] / normalizer;
            }
        }
    }
    for (auto it = state.nextStates.begin(); it != state.nextStates.end(); it++) {
        normalize(it->second);
    }
}

std::vector<float> Cfr::runCfr(std::vector<int>& downCards, State& state,
    std::vector<float>& probabilities, int iterationNum) {
    auto downCard = downCards[state.toAct];
    if (state.isTerminal) {
        return getTerminalNodePayoffs(downCards, state);
    }

    auto strategy = getStrategy(downCards, state, probabilities, iterationNum); // want to reference this
    std::map<std::string, float> utilities = {{"Stand", 0}, {"Hit", 0}};
    std::vector<float> payoffs = {0, 0, 0};

    // hitting, need to accumulate from all possible hit child states

    std::vector<std::string> subActs = {"s", "h1", "h2", "h3", "h4", "h5", "h6",
        "h7", "h8", "h9", "h10"};

    for (auto subAct : subActs) {
        float subProbability = 1;
        std::string action = "Stand";

        if (subAct.substr(0, 1) == "h") {
            action = "Hit";
            int cardNum = stoi(subAct.substr(1, subAct.length() - 1));
            subProbability = cardNum == 10 ? (4.0 / 13) : (1.0 / 13);
        }
        auto newProbabilities = probabilities;
        newProbabilities[state.toAct] *= (strategy[action] * subProbability);
        const auto& childPayoffs = runCfr(downCards, state.nextStates[subAct], newProbabilities, iterationNum);
        utilities[action] += childPayoffs[state.toAct] * subProbability;

        int otherPlayer = state.toAct == 1 ? 0 : 1;

        payoffs[otherPlayer] += strategy[action] * childPayoffs[otherPlayer] * subProbability;
        payoffs[state.toAct] += strategy[action] * childPayoffs[state.toAct] * subProbability;
        payoffs[2] += strategy[action] * childPayoffs[2] * subProbability; // dealer
    }

    for (auto action : {"Stand", "Hit"}) {
        auto curRegret = state.regret[downCard][action];
        auto regret = std::max(curRegret + utilities[action] - payoffs[state.toAct], 0.0f);
        state.regret[downCard][action] = regret;
    }
    
    return payoffs;

}

std::map<std::string, float> Cfr::getStrategy(std::vector<int>& downCards,
    State& state, std::vector<float>& probabilities, int iterationNum) {
    auto downCard = downCards[state.toAct];
    auto& strategy = state.strategies[downCard];
    auto& strategySum = state.strategy_sums[downCard];
    auto& regret = state.regret[downCard];
    auto probability = probabilities[state.toAct];

    float normalizer = 0;
    for (auto action : {"Stand", "Hit"}) {
        auto actionRegret = regret[action];
        if (actionRegret > 0) {
            strategy[action] = actionRegret;
        } else {
            strategy[action] = 0;
        }
        normalizer += strategy[action];
    }

    for (auto action : {"Stand", "Hit"}) {
        if (normalizer > 0) {
            strategy[action] /= normalizer;
        } else {
            strategy[action] = .5; // 1 / 2
        }
        strategySum[action] += probability * strategy[action] * iterationNum;
    }

    return strategy;

}

std::vector<float> Cfr::getTerminalNodePayoffs(std::vector<int>& downCards, State& state) {
    // TODO: implement dealer probabilities
    auto& upCards = state.upCards;
    float player0Hits = state.players[0].numHits;
    float player1Hits = state.players[1].numHits;
    float pot = 3 + player0Hits + player1Hits;
    std::vector<int> sums = {0, 0, 0};
    for (int i = 0; i < 2; i++) {
        sums[i] = getSum(downCards[i], upCards[i]);
    }
    if (sums[0] > sums[1]) {
        return {pot - player0Hits, -1 * player1Hits, 0};
    } else if (sums[1] > sums[0]) {
        return {-1 * player0Hits, pot - player1Hits, 0};
    } else if (sums[0] == 0) {
        return {-1 * player0Hits, -1 * player1Hits, pot};
    } else {
        return {pot / 2 - player0Hits, pot / 2 - player1Hits, 0};
    }
}

int Cfr::getSum(int downCard, std::vector<int> upCards) {
    int sum = 0;
    upCards.push_back(downCard);
    bool hasAce = false;
    for (auto& card : upCards) {
        if (card == 1) {
            hasAce = true;
        }
        sum += card;
    }
    if (hasAce && sum <= 11) {
        sum += 10;
    }
    if (sum > 21) {
        sum = 0;
    }
    return sum;
}

std::vector<int> Cfr::generateDownCards() {
    std::vector<int> res;
    for (int i = 0; i < 3; i++) {
        auto randNum = rand() % 13 + 1;
        if (randNum >= 10) {
            res.push_back(10);
        } else {
            res.push_back(randNum);
        }
    }
    return res;
}