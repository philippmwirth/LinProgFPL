# Linear Program for Optimal FPL Player Selection

## Installation

1. Install CGAL and create the cmake scripts:

   ```
   sudo apt-get install libcgal-dev
   cgal_create_cmake_script
   ```
2. Compile:
   ```
   cmake .
   make
   ```
3. Run with `lambda : [0, 0.5, 1]`:
   ```
   ./main 0
   ./main 0.5
   ./main 1
   ```


## Theory

### Problem Description
[Fantasy Premier Leauge](https://fantasy.premierleague.com/) is a competition where participants can pick a team of 15 players from the Premier League under a set of specific constraints. The competition proceeds in rounds and each of the players is awarded points based on how well he played in every round. The goal is to get as many points as possible and the constraints under which players can be selected are:
- Every player can only be selected once.
- There can be at most three players from the same team.
- The selection must contain exactly two goalkeepers, five defenders, five midfielders, and three forwards.
- The combined cost of the selection must not exceed a given budget.


A natural question that arises is: 

**Given a performance score for each player, what are the optimal picks under the given constraints?**

**Brute-Force:** The above question could easily be answered by iterating over all possible teams. However, in the Season 2020/2021, there are 68 goalkeepers, 219 defenders, 240 midfielders, and 76 forwards to pick from. Ignoring the constraints limiting the number of players from the same team, the number of possible picks is roughly 4 * 10<sup>27</sup>. Hence, even with the additional constraint, iterating over all teams is likely unfeasible.

**Knapsack:** Anyone who has had at least one algorithm class in his life probably knows about the knapsack problem and how to solve it in pseudo-polynomial time. Unfortunately, the complexity of the pseudo-polynomial time algorithm grows exponentially in the number of constraints and therefore the above problem cannot be efficiently solved (note that allowing at most three players from the same team introduces 20 constraints).

### Linear Program Formulation
The above problem can be formulated as an integer linear program. Let *p<sub>i</sub>* be the performance score, *team<sub>i</sub>* be the team, *pos<sub>i</sub>* be the position, and *c<sub>i</sub>* be the cost of player *i*. Then the linear program formulation reads:


Or, in matrix notation:

### Proof that Solutions are Integral

## Example