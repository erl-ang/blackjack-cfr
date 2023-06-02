#include "Game.h"
#include "time.h"

/**
 * format - {player 0, player 1, dealer}
 * maxTurns - max number of hits a player can take
 */
Game::Game(std::vector<int> startingCards, int maxHits, int cfrIterations) {
    this->startingCards = startingCards;
    this->maxHits = maxHits;
    auto root = constructTree();

    //Cfr cfr = Cfr(root, cfrIterations);
    //cfr.train();

    std::cout << "CFR complete" << std::endl;
}

State Game::constructTree() {
    auto Player0 = Player(0, false /*{startingCards[0]}*/);
    auto Player1 = Player(1, false /*{startingCards[1]}*/);
    std::vector<Player> players = {Player0, Player1};
    std::map<int, std::vector<int>> upCards = {{0, {startingCards[0]}}, {1, {startingCards[1]}}, {2, {startingCards[2]}}};
    std::cout << "Up Cards| Player 0: " << startingCards[0] << ", Player 1: " << startingCards[1] << ", Dealer: " << startingCards[2] << std::endl;;
    auto start = std::chrono::system_clock::now();
    auto root = State(0, players, false, "", upCards, maxHits);
    auto end = std::chrono::system_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Finished Building Tree, Time Taken (ms): " << time << std::endl;
    std::cout << "Num nodes: " << root.getNumStates() << std::endl;
    // printRandomPath(root); // debug
    return root;
}

void Game::printRandomPath(State state) {
    state.printDetails();
    if (!state.isTerminal) {
        auto it = state.nextStates.begin();
        srand(time(0) + state.pastActions.length());
        std::advance(it, rand() % state.nextStates.size());
        printRandomPath(it->second);
    }
}

int main(int argc, char *argv[]) {
    // placeholder
    auto maxHits = atoi(argv[1]);
    auto cfrIterations = atoi(argv[2]);
    Game game = Game({1, 1, 1}, maxHits, cfrIterations);
}