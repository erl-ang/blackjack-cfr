#include "Player.h"

Player::Player(int id, bool doneAction /*std::vector<int> cards*/) {
    this->id = id;
    this->doneAction = doneAction;
    //this->cards = cards;
    numHits = 0;
}