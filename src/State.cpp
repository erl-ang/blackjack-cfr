#include "State.h"

State::State(int toAct, std::vector<Player>& players, bool isTerminal, 
            /* for debugging */ std::string pastActions,
            std::map<int, std::vector<int>>& upCards, int maxHits) {
    this->toAct = toAct;
    this->players = players;
    this->isTerminal = isTerminal;
    this->pastActions = pastActions;
    this->upCards = upCards;
    this->maxHits = maxHits;
    initializeMaps();
    populateChildren();
}

State::State() {
    this->toAct = 0;
}


void State::initializeMaps() {
    for (int i = 1; i <= 10; i++) {
        strategies[i] = {{"Stand", 0}, {"Hit", 0}};
        regret[i] = {{"Stand", 0}, {"Hit", 0}};
        strategy_sums[i] = {{"Stand", 0}, {"Hit", 0}};
    }
}

void State::populateChildren() {
    if (isTerminal) {
        return;
    }

    if (pastActions.length() == 0) {

        std::vector<std::future<State>> futures;
        futures.reserve(11);

        futures[0] = std::async(std::launch::async, [this](){
            return State::createStandState();
        });
        for (int i = 1; i <= 10; i++) {
            futures[i] = std::async(std::launch::async, [this, i]() {
                return State::createHitState(i);
            });
        }

        
        for (int i = 0; i <= 10; i++) {
            if (i == 0) {
                const auto& state = futures[0].get();
                //state.printDetails();
                nextStates.emplace("s", state);
            } else {
                std::string formatted = "h" + std::to_string(i);
                const auto& state = futures[i].get();
                //state.printDetails();
                nextStates.emplace(formatted, state);
            }
        }

    } else {
        const auto& nextStandState = createStandState();
        nextStates.emplace("s", nextStandState);
        for (int i = 1; i <= 10; i++) {
            // TODO: change to std::format with cpp20 
            std::string formatted = std::format("h{}", i);
            //std::string formatted = "h" + std::to_string(i);
            const auto& nextHitState = createHitState(i);
            nextStates.emplace(formatted, nextHitState);
        }
    }
}

State State::createStandState() {
    int otherPlayer = toAct == 0 ? 1 : 0;
    // if next player has finished action, our next state will be terminal
    bool nextTerminal = players[otherPlayer].doneAction;
    auto playersCopy = players;
    // TODO: change to std::format with cpp20 
    std::string action = std::to_string(toAct) + ": s, ";
    std::string actions = pastActions + action;
    playersCopy[toAct].doneAction = true;
    return State(otherPlayer, playersCopy, nextTerminal, actions, upCards, maxHits);
}

State State::createHitState(int cardNum) {
    int otherPlayer = toAct == 0 ? 1 : 0;
    int curSum = cardNum;
    for (int card : upCards[toAct]) {
        curSum += card;
    }
    bool doneAction = curSum >= 20 || players[toAct].numHits == maxHits - 1;
    bool otherDoneAction = players[otherPlayer].doneAction;
    bool nextTerminal = doneAction && otherDoneAction;
    int nextPlayer = otherDoneAction ? toAct : otherPlayer;

    auto playersCopy = players;
    auto upCardsCopy = upCards;

    // TODO: change to std::format with cpp20 
    std::string action = std::to_string(toAct) + ": h" + std::to_string(cardNum) + ", ";
    std::string actions = pastActions + action;
    playersCopy[toAct].doneAction = doneAction;
    playersCopy[toAct].numHits++;

    upCardsCopy[toAct].push_back(cardNum);

    return State(nextPlayer, playersCopy, nextTerminal, actions, upCardsCopy, maxHits);
}

int State::getNumStates() {
    int numStates = 1;
    for (auto& [act, state] : nextStates) {
        numStates += state.getNumStates();
    }
    return numStates;
}

void State::printDetails() {
    std::cout << "---------" << std::endl;
    // print upcards
    std::cout << "Upcards" << std::endl;
    std::cout << "Player 0: ";
    for (auto card : upCards[0]) {
        std::cout << card << " ";
    }
    std::cout << std::endl;
    std::cout << "Player 1: ";
    for (auto card : upCards[1]) {
        std::cout << card << " ";
    }
    std::cout << std::endl;
    std::cout << "Dealer: ";
    for (auto card : upCards[2]) {
        std::cout << card << " ";
    }
    std::cout << std::endl;
    for (const auto& player: players) {
        std::cout << "Player " << player.id << " , doneAction: " << player.doneAction << ", numHits: " << player.numHits << std::endl;
    }

    std::cout << "Past Actions: " << pastActions << std::endl;

}