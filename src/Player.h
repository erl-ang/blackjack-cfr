// TODO: change these imports to modules (he'd like that)
#include <vector>

class Player {
    public:
        Player(int id, bool doneAction /*std::vector<int> cards*/);

        int id; // order to act
        bool doneAction; // no more allowed action (happens if player stands or sum of face up cards >= 20)
        //std::vector<int> cards; // the face up ones (1: A, 2 - 9: as normal: T: {T, J, Q, K})
        int numHits;

        // account for Aces in sum bc 1/11
};