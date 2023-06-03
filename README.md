# Blackjack++: Counterfactual Regret Minimization in Large Imperfect-Information Games ♠️
Team members: Mateo Maturana (mm5589), Marcus Fong (maf2300), Erin Liang (ell2147)

Made for Professor Stroustrup's (Yes, ***that*** Stroustrup) Fall 2022 COMS W4995 Design Using C++ Course @ Columbia Unversity.

## Motivation ❓
AI’s history has been a story of advances in AI-based players beating out professional players in increasingly more complicated games [[1]](https://analyticsindiamag.com/timeline-of-games-mastered-by-artificial-intelligence/). Checkers. Chess. Go. Improvements on chess. Improvements on Go. 

Information about the latest game algorithms powering these AI game players mostly resides inside academic papers filled with complicated mathematical notation and algorithmic proofs. This confinement to academia gatekeeps knowledge from those without rigorous technical background, such as casual and professional players interested in improving their game-play, as well as interested students.

The latest of these game algorithms is ***CFR (counterfactual regret minimization)***, which forms the basis for poker bots at poker competitions [[2, 3, 4]](https://github.com/erl-ang/blackjack-cfr#references). High-level details of CFR are discussed in section 3. 

Moreover, because of the nacency of regret-based algorithms, there are few practical materials available to introduce algorithms like CFR.  Arguably the best way to learn the insights behind AI-based game-playing strategies is to interact with AI players. This necessitates the development of interactive programs where people can play against the algorithm, whether the goal is to improve their strategy, learn about the algorithm interactively, or simply have fun.

## Goals 🥅
Our project’s goal is to implement a game of Blackjack to provide users with a worked example of how the CFR library can interact with games like Blackjack. Blackjack++ will hopefully not only let users understand CFR somewhat interactively, but also encourage the further development of the CFR library for solving other games.

On an implementation note, we choose to use C++ because 
1) many games like Blackjack can have rather large game trees, 
2) CFR requires a lot of iterations to converge, 
3) C++’s Object-Oriented nature makes the code conceptually intuitive to understand, and 
4) coroutines in C++20 can be used to solve different parts of the game tree in parallel (using C++20 was a req for this assignment)
5) this class is a class on C++...

### Structure
First, we present the background information necessary to use Blackjack++, which is broken down into background on CFR and the assumed rules of our specific variant of Blackjack. Section four provides a tutorial on how to use Blackjack++. The fifth section includes more detailed information about Blackjack++’s design. The sixth section describes the performance of Blackjack++ with different inputs. The seventh and final section discusses plans for future work, as well as a post-mortem on the work done and roadblocks encountered.

## Background Information ℹ️
This section is meant to provide the necessary background information for readers that are not familiar with the mechanics of Blackjack or game theory to understand CFR. 

### Background on CFR
At a high level, CFR (counterfactual regret minimization) is a game theory concept that refers to the idea of minimizing the potential regret that a player may experience after making a decision in a game. In other words, it involves trying to make decisions that will minimize the chances of ending up in a situation where you regret your choice. 

This can be done by considering the potential outcomes of different decisions and choosing the one that is most likely to result in the best outcome. For example, in a game of chess, an AI player may use CFR to consider the potential moves that their opponent could make and choose the move that is most likely to lead to a successful outcome. 

### Background on Blackjack++
There are many variants of Blackjack, so it is important to clarify Blackjack++’s assumed rules of Blackjack. Blackjack++ is played with 2 players and a dealer. As opposed to most other games where players try to beat each other, Blackjack is a game where the goal is to beat the dealer. Each player and the dealer is dealt 1 card face up, 1 card face down. 

The goal is simple: get <= 21 than the dealer without going over. Players play in turn and can either draw another card or stay for each turn. This card remains face up. Once players both stay, this round is over. After both players finish their turns, the dealer acts in accordance with normal Blackjack rules. 

Card Values: Cards 2-10 are valued at their face value, JQK are all valued at 10, and Ace can be valued at either 1 or 11, whichever is better for the hand. 

## Tutorial 🧑‍🏫
### How to Build
We included a Makefile to make building Blackjack++ easier. To build, just run make:
```
$ make
c++  -I/usr/local/opt/ncurses/include  -c -o src/Cfr.o src/Cfr.cpp
...
```

To run a game of Blackjack++ to save under sol where all players start with a ten with a maximum of 1 hit (players can draw up to 1 more card) and 10000 iterations of CFR:
```
$ ./a.out “10, 10, 10” 1 10000 sol
```

In the general case, we would run as follows.
```
$ ./a.out “[player0 card], [player 1 card], [dealer card]” [maxHits] [numIterations] [dump directory]
4.2 "Hello world" simple example
```

```
$ ./a.out “6, 6, 2” 1 1000 tmp
Up Cards| Player 0: 6, Player 1: 6, Dealer: 2
Finished Building Tree, Time Taken (ms): 12
Num nodes: 133
CFR TRAINING DONE
ROOT PAYOFFS: 
Player 0: 0.859844
Player 1: 0.844316
Dealer: 1.29584
```

This produces a directory of the structure
```
├──tmp
    ├── strategy.txt
    ├── s
    │   ├── strategy.txt
    │   ├── s
    │   └── ...
    ├── h1
    │   └── ...
    ├── h2
    │   └── ... 
    ├──h3
    │  └── ...
    ├── ... 
```

Where strategy.txt contains the strategy of the node given the decision tree. For example, if we want to see the strategy for Player 2 if Player 1 stands, this would be found under tmp/s/strategy.txt

### Common Errors
Common errors in using Blackjack++ include setting `maxHits` too high, setting `cfrIterations` too high, and in general, incorrect command line arguments. 

`maxHits` is a command-line argument and denotes the maximum number of times that a player can request an extra card. If users set the maxHits value to be too high, it will take a very long time to build a game tree. This is because there is a branching factor of ~100 for an increment of 1 in maxHit.

`cfrIterations` is a command-line argument and denotes the number of iterations that the user wants CFR to take. Due to CFR+, the strategies converge fairly quickly.

## Design Manual 🎨
### CFR Class
The details of the CFR+ algorithm are out of scope for this design document and for the sake of the brevity of this design document, will be omitted. However, these details can be found in the paper “Solving Large Imperfect Information Games Using CFR+” [[4]](https://arxiv.org/pdf/1407.5042.pdf). For this section, we provide a high-level description of the methods in the CFR class. 

#### `train()`
Runs cfrIterations number of iterations of CFR+. cfrIterations is supplied by the user as a command-line argument. 

#### `normalize()`
Normalizes weighted strategy sums to get a strategy that sums to 1.

#### `runCfr()`
Runs each iteration of CFR. In the training stage, CFR trains and works to iteratively improve the strategy of each player by computing the differences between different actions at different states. At each iteration, the algorithm updates the strategy of the player by taking into account of these differences. Specifically, it calculates the "regret" of each action. The algorithm is to calculate "regret' by comparing what would have happened if the player chose a different action than the one it actually took. By assigning values to certain actions that lead to a win more quickly (or a loss), the algorithm is able to use the differences in action outcomes to figure out how "regretful" an action can be. Thus, as it continues to iterate, the strategy continuously updates to minimize the amount of "regret" it feels from each action.

#### `getStrategy()`
Use RegretMatching+ to find the strategy of the current CFR+ iteration. Please see the CFR+ paper for more details.

#### `generateDownCards()`
In Blackjack++, each player starts off with two cards, one of which is a down card (a card that faces down). This means that each player knows the value of only its own downcard, no one else. To train our CFR model, we must randomly generate downcards for each player. Thus, this function generates three downcards, one for each player. It does so by randomly generating a number from 1-13, where numbers within the range 10-13 (10, J, K, Q) are treated to have a value of 10.

### Game Class
The Game class defines the logic for the Blackjack++ game. To explain the functionality of this class, it helps to start from the main() method where the Game object is constructed, as all the methods of this class are eventually called from `main()`.

#### `main()`
To play Blackjack++, we run the resulting executable with 3 arguments: the starting cards and the desired amount of maxHits and cfrIterations. Both of these parameters constrain the size of the game tree. As CFR takes a lot of iterations to converge, it is valuable for testing the CFR algorithm to be constraining the amount of steps that CFR is allowed to take. A similar design reasoning was applied to supplying maxHits as a parameter. More detail is provided on maxHits later in this section.

Then, a game object is created using  maxHits and cfrIterations, as well as a a starting hand (`std::vector<int>`). 

#### `Game(startingCards, maxHits, cfrIterations)`
To create a Game Object, a starting hand of 3 has to be provided to deal one card to Player0, Player1, and the dealer. The user needs to specify the maximum number of cards a player can request, `maxHits`, as well as `cfrIterations`,  the amount of steps the user wants the CFR algorithm to take.

Creating a Game object builds the game tree using `Game::constructTree()`, as well as creates and trains the CFR algorithm on the constructed tree with the specified number of iterations for the CFR algorithm. 

#### `constructTree()`
This method contains the larger game logic for Blackjack++. First, two Player objects are created with their ids. They have not taken any actions yet as the game has just begun, so doneAction is false. 

The strategy of Blackjack++ follows the cards that are visible on the table— players can only reasonably make decisions based on the cards that they can see on the table. We denote this internally as the `upCards`.  At the start of the game, these upCards are the cards that are originally dealt. We map each card that is facing up on the table to the player that owns the card in a `std::map<int, std::vector<int>>`

We keep track of the time and the amount of nodes it takes to build the game tree for performance tuning. This lets us keep track of how `maxHits` and `cfrIterations` impact the game tree build time and size and, correspondingly, how we should optimally be adjusting these parameters.

#### `printRandomPath(State state)`
This method is only for debugging/pedagogical purposes. It recursively traverses the resulting game tree and prints out the actions corresponding to a random path of the tree. 

#### `startingCards`
`startingCards` is a vector of three ints initialized by command line arguments. While it would be nice to randomize this, we stuck with a simpler implementation in order to test the CFR strategy’s correctness.

#### `maxHits`
`maxHits` is an integer constraining the maximum number of times Blackjack++ players are allowed to hit, or request an extra card from the dealer. As we needed to test the CFR algorithm on small game trees before running it on a full-fledged game of Blackjack++, we provided maxHits as an argument to run the game to test our code on a constrained game tree.

#### Game Class Design Choices:
Alternative approaches to this class included a more object-oriented approach with a deck class, a card class, etc. We opted to implement cards internally as a vector of ints because in the other existing card and deck classes that we looked at, there was a lot of extraneous information stored that we don’t need for computing Blackjack++ strategy.

### Player Class
There are 2 players and one dealer in Blackjack++. The `id` attribute identifies these players. This Player class design is very particular to Blackjack++, as there are specific things within the game that we need to keep track of. The following section details how Players are used and implemented in Blackjack++.

Where are Players Used?
We can see the Players being used in the Game class when the game tree is constructed in `Game::constructTree()`. Player0 and Player1 are the 2 players.

#### `Player(id, doneAction)`
Players are constructed in Game.cpp as follows:
```
auto Player0 = Player(0, false);
auto Player1 = Player(1, false);
```

#### `id`
The id attribute identifies players of Blackjack++. 

#### `doneAction`
One thing within Blackjack++ that we need to keep track of is whether the Player is allowed to take any more actions. This is denoted by the boolean `doneAction`. The value of doneAction is set to true if the player’s up cards sum to more than 20.

#### `numHits`
Another thing that we need to keep track of in Blackjack++ is the total number of hits. A hit is when the player requests an extra card from the dealer. 

### State Class 
To explain the functionality of this class, it helps to start from the constructor and to sequentially work our way down through each method. All the methods in this class are called within the constructor, so it would make the most sense to explain each method in the order in which they are called. 

#### What is a State? 
As mentioned before, CFR is an algorithm that finds the best possible action for a player's current hand by simulating tens of thousands games and possibilities. It can simulate different game states and actions by using a decision tree. Each node in this tree represents varying "states" of the game. We can distinguish different "states" of the game by keeping track of a select set of variables that describe the current state of the game. For example, information like whether a player has won yet, which cards are currently faced up or down, and what the players' past actions are can allow us to distinguish one game state from another. Thus, we can store all of the relevant game state information into our State class, where each newly created State object represents a different state of the game (i.e.a different node in the decision tree).

At each game state, players can choose different actions to continue the game. They can either "Stand" or "Hit" (i.e. draw a card that has a random value from 1-10). Each action taken changes the game State, and we can represent this change in state by creating a new State object as a child node of the previous State object. This means that every State object will have 11 children, 1 child node for the "Stand" action and 10 other nodes to represent each possible card value it could draw after choosing "Hit". Therefore, not only will the State class contain information about the current state of the game but also recursively provide access to subsequent States of the game.

#### Where is `State` called or used?
Now that we know the general idea of what our `State` class represents, lets take a look at when it is first used. Starting from the beginning of our program in `Game.cpp`'s main function, we can see it calls `Game::constructTree()`. Inside `Game::constructTree()`, the game calls the `State` constructor to create a State object named root.  This is done to simulate the decision tree of all possible outcomes of Blackjack++, given the starting settings that are specified at the beginning of the program (i.e. `startingCards` and `maxHits`). We can see how the `State` class recursively simulates every possible game state by taking a closer look at the State constructor.

#### `State::State()`
The job of our `State()` constructor is to initialize a node in the CFR decision tree to represent the current state of the game. It first calls `initializeMaps()`, which initializes three `std::maps`:
```
std::map<int, std::map<std::string, float>> strategies;
std::map<int, std::map<std::string, float>> regret;
std::map<int, std::map<std::string, float>> strategy_sums;
```
These maps map a card point value (1-10) to an action's ("Stand" or "Hit") strategy or regret value. These values are updated and changed throughout the CFR process, and are essential to determining the optimal actions for each State. In other words, for every card value (1-10), there is a correspnoding strategy, regret, and strategy_sums value for each possible action at that point. The constructor then finally calls `populateChildren()`, which is the primary function that is in charge of generating child State objects after taking some action. 

####`State::populateChildren()`
`populateChildren()` is a function that recursively generates child States. A child State represents the state of the game after taking a specific action. There are 11 actions total, and an action can either be to 1) "Stay" or to 2) "Hit" and retrieve a card that is valued from 1 to 10. The goal of CFR is to simulate all possible to game states (until it reaches a terminal state or a max depth specified by the user), and to do so, it must iterate through all 11 actions per generated State. However, building the tree with only process is very time costly, so we decided to could speed up the process by asynchronously building it with C++ `std::futures`.  Specifically, when `populateChildren` is called for the first time when the tree is being first instantiated, we intitialize 11 asynchronous processes to recursively build the tree from the top-down. Each process is assigned to one of the eleven possible distinct actions (i.e. "Stand", "Hit1", "Hit2", "Hit3", "Hit4", etc.). Then, each process recursively calls `populateChildren()` and simulates all subsequent game states until a terminal state is reached. Once each child process finish generating its part of the decision tree, the `std::future` returns a State object that represents the state of the game after taking its assigned action. Meanwhile, while the parent process blocks its program until all of its child processes have finished, it finally assigns the returned State objects to each corresponding State-action pair in its nextStates map. Because nextStates maps the current State to a subsequent game State after taking one out of the eleven actions, the root of our CFR tree is therefore able to access all subsequent States of the game by recursively recursing through nextStates and all of its possible State-action pairs.

#### `State State::createStandState()`
This function creates a new State after taking the "Stand" action from the current State. This method is constantly invoked by `populateChildren()` to create child State objects whenever it needs to simulate the the current Player choosing "Stand" as its next action. 

To create the next State, it has to change and update several variables. Firstly, because every State keeps track of who's current turn it is, our code indicates to the newly returned State that it is now the other player's turn. The function also marks the current player as "Done", as choosing "Stand" means that they are no longer allowed to Hit. It also updates the history of past actions by appending the player number and an "s" to mark which player has just chosen "State".

#### `State State::createHitState(int cardNum)`
This function creates a new State after taking the "Hit" action from the current `State`, where the newly "Hit" card has the value cardNum. This method is constantly invoked by populateChildren() to create child State objects whenever it needs to simulate the the current Player choosing "Hit" as its next action. 

To create the next State, it has to change and update several variables. Firstly, because every State keeps track of who's current turn it is, our code indicates to the newly returned State that it is now the other player's turn. It also updates the history of past actions by appending the player number, an "h", and cardNum to indicate which player just Hit which card. Finally, it also checks whether the player has over hit more than its max number of Hits or if the player has reached a total value of <= 20 from summing all of its upCard values. 

## Performance 📊

To quantify the performance of Blackjack++, we ran Blackjack++ on 3 significantly different sets of input and compared the resulting performance. Specifically, we measured both the time it took to train the CFR algorithm, as well as the time it took to build the entire game tree. For the different input sizes, we used 1k, 10k, and 100k `cfrIterations`. We also tested 1, 2, and 3 `maxHits` against these different input sizes. To control the game tree, we ran each experiment with every player starting with a 10. 

Here are the results summed up in graph form:
<p align='center'>
<img src="https://github.com/erl-ang/cpp-blackjack/blob/master/assets/graph1.png">
</p>
<p align='center'>
<img src="https://github.com/erl-ang/cpp-blackjack/blob/master/assets/graph2.png">
</p>
<p align='center'>
<img src="https://github.com/erl-ang/cpp-blackjack/blob/master/assets/graph3.png">
</p>
The above three graphs illustrate the impact of the command line arguments, `maxHits` and `cfrIterations`, on the build time of the Blackjack++ game tree. Overall, the larger patterns are that the build time of the tree stays relatively constant when varying the CFR iterations and keeping the maxHits the same. This makes sense because cfrIterations should only impact the training time of the CFR algorithm, not the build time of the tree. Another trend is that the build time greatly increases when maxHits is incremented. This makes sense because there is approximately a branching factor of ~100 for an increment of 1 in maxHit. The running time accordingly increases by a factor of ~100.

## Post-Mortem: Roadblocks ++ Future Work 🌠
Most things we originally aimed to accomplish in Blackjack v1.0 were completed. We have a working Blackjack++ game, managed to implement the CFR algorithm from the paper (which, to our knowledge, hasn’t been implemented in C++ until this project), the CFR decision trees are compatible with our Blackjack++ AI player, and the algorithm and the dealer outperforms the user most of the time! 

However, our initial project proposal was admittedly a bit overambitious. We (expectedly?) stumbled into some roadblocks along the way that prevented us from successfully completing some of the things that would have really put Blackjack++ over the edge. Namely, these things are: adding modules/coroutines, adding more interactivity into Blackjack++, and working to make the CFR algorithm generalizable to other games. Adding these features would make Blackjack++ a more practical introduction to game theory algorithms like CFR for casual players and interested students.

### Modern C++ 
Originally in our initial proposal, we wanted to include coroutines and modules to incorporate bits of modern C++ into our project. 

Coroutines: We wanted to use coroutines to concurrently build each subtree of the root of the game tree at the same time to make building the game tree a lot faster. We ran into a lot of problems when trying to incorporate coroutines because there was no support for coroutines. In particular, we tried using external libraries from Facebook and other coroutine abstractions to write coroutines [[5](https://github.com/facebook/folly/blob/main/folly/experimental/coro/README.md), [6](https://github.com/lewissbaker/cppcoro)].

To compromise,  we incorporated some vanilla multithreading to concurrently build each subtree of the root at the same time. 

Modules: We thought that incorporating modules would be a quick and simple way to add some C++20 into our project.  When trying to incorporate modules into our project, we ran into some problems with `std::format` utility that we were unable to resolve [[7]](https://stackoverflow.com/questions/65083544/format-no-such-file-or-directory) since C++20 just released. 

### Better ++ More Interactivity
As our focus on Blackjack++ was primarily getting the CFR decision trees to be compatible with our Blackjack++ AI player, we didn’t get to spend the effort that we wanted to in making our project the most newbie-friendly. Making Blackjack++ more interactive and friendly to game theory novices would definitely be more of a priority to the next release of Blackjack++. Some ideas that we have include making the AI playable and more usage guidance.

In particular, we would have tried to implement Blackjack++ so players could actually play against the Blackjack++ AI player. However, trying to make the AI player playable, while also trying to debug the CFR decision trees, while also trying to check the logic of our Blackjack++ game was a lot to take on for Blackjack++ v1.0. Thus, for v1.0, we sacrificed the playability of the AI player in order to ensure the underlying logic behind the algorithms were correct. 

Instead of incorporating interactivity, we settled for a middle ground that still holds a lot of pedagogical value. Given the starting hands, all the possible outcomes of the game are printed out. More specifically, for each potential value of the card that is facing down, the probabilities that the AI player hits or stands are printed out in order to determine the optimal strategy. This still holds a lot of pedagogical value because users of Blackjack++ can improve their own strategy by seeing how the AI player determines its optimal strategy throughout all game scenarios. In the future, we could incorporate a UI element so users can have a more concise, less text-based way of visualizing Blackjack++ strategies. 

On more of a nitpicky note,  when users want to use Blackjack++, they build our project and run Blackjack++ by supplying  the desired maxHits and cfrIterations which determine the game tree size. Users unfamiliar with CFR, Blackjack, or game theory could benefit from some <usage>  printing guidance when supplying these parameters.

### Making it generalizable to other Games
Although this wasn’t included in our project proposal, we thought it might be useful to try to create a CFR library that was abstracted enough to solve other games other than Blackjack++. However, this was not a priority for Blackjack++ v1.0. 

This deprioritization of abstraction was primarily due to the prioritization of other tasks, namely dedicating time to understanding a new game theory algorithm like CFR and needing something “demoable” for this project. This demoability necessitated also implementing the logic of a game compatible with CFR. Though looking at a specific application of CFR was a necessary step in creating abstraction, it left us less time for abstraction. 

CFR is an iterative algorithm that is proven to converge to a Nash equilibrium in two-player zero-sum imperfect-information games [8](https://ocw.mit.edu/courses/15-053-optimization-methods-in-management-science-spring-2013/2e66a9d9a74dc5c11b620f70663400da_MIT15_053S13_tut08.pdf). Other games that we could work on abstracting the CFR library toward could include RPS (rock paper scissors) and Kuhn Poker.

## References 🗞️
1. https://analyticsindiamag.com/timeline-of-games-mastered-by-artificial-intelligence/ 
2. https://int8.io/counterfactual-regret-minimization-for-poker-ai/ 
3. http://modelai.gettysburg.edu/2013/cfr/
4. https://arxiv.org/pdf/1407.5042.pdf 
5. https://github.com/facebook/folly/blob/main/folly/experimental/coro/README.md 
6. https://github.com/lewissbaker/cppcoro 
7. https://stackoverflow.com/questions/65083544/format-no-such-file-or-directory 
8. https://ocw.mit.edu/courses/15-053-optimization-methods-in-management-science-spring-2013/2e66a9d9a74dc5c11b620f70663400da_MIT15_053S13_tut08.pdf 
