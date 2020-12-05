#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
// CGAL for linear program
#include <CGAL/basic.h>
#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpq.h>
// json to load the json file
#include "single_include/nlohmann/json.hpp"


typedef float num_t;    // numerical type
typedef CGAL::Gmpq ET;  // exact type
typedef CGAL::Quadratic_program<num_t> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;


// Prints one left aligned element of a table.
template<typename T> void printElement(T t, const int& width)
{
    const char separator = ' ';
    std::cout << std::left << std::setw(width) << std::setfill(separator) << t;
}


// Pretty prints the solution to the linear program.
void printTable(Solution s, std::vector<std::string> &names, std::vector<num_t> &costs, std::vector<num_t> &forms)
{

    // column widths of the table
    int name_width = 20;
    int col_width = 5;

    // index for iterating over the rows
    int index = 0;

    // keep track of overall cost and form
    num_t overall_cost = 0;
    num_t overall_form = 0;

    // header
    for (int i = 0; i < (name_width + 2 * col_width); i++)
        std::cout << "-";
    std::cout << "\n";

    printElement("Web Name", name_width);
    printElement("Cost", col_width);
    printElement("Form", col_width);
    std::cout << "\n";

    for (int i = 0; i < (name_width + 2 * col_width); i++)
        std::cout << "-";
    std::cout << "\n";

    // rows
    for (auto i = s.variable_values_begin(); i != s.variable_values_end(); i++) {
    
        if (i->numerator() == 0) {
            index++;
            continue;
        }

        printElement(names[index], name_width);
        printElement(costs[index], col_width);
        printElement(forms[index], col_width);
        std::cout << "\n";

        overall_cost += costs[index];
        overall_form += forms[index];

        index++;
    }

    for (int i = 0; i < (name_width + 2 * col_width); i++)
        std::cout << "-";
    std::cout << "\n";

    // results
    std::cout << "Overall Cost: " << overall_cost << " $\n";
    std::cout << "Overall Form: " << overall_form << "\n";

}


// Builds and solves a linear program to find the optimal set of Premier League
// fantasy football players under the following set of constraints:
//
// - Each player may only be selected once.
// - No more than 3 players of the same team can be selected.
// - Select exactly 2 goalkeepers, 5 defenders, 5 midfielders, 3 forwards.
// - The overall cost of the selection must not be larger than 1000 $.
//
// Hereby, the last constraint is softened and added as a Lagrange multiplier
// in order to keep the constraint matrix totally unimodular. This guarantees
// integer solutions to the linear program.
int main(int argc, char **argv)
{

    // set factor of the Lagrange multiplier
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " LAMBDA\n";
        return 1;
    }
    num_t lambda = (num_t) std::stof(argv[1]);


    // read the player JSON file
    nlohmann::json players;
    std::ifstream i("data/players.json");
    i >> players;

    // build linear program (with lower and upper bounds)
    Program lp (CGAL::SMALLER, true, 0.0, true, 1.0);

    std::vector<std::string> names;
    std::vector<num_t> teams;
    std::vector<num_t> element_types;
    std::vector<num_t> forms;
    std::vector<num_t> costs;
    std::vector<num_t> playing_next_round;

    unsigned int n_players = players.size();

    for (unsigned int i = 0; i < n_players; i++) {
        
        // parse values of interest
        names.push_back(players[i]["web_name"].dump());
        teams.push_back((num_t) players[i]["team"] - 1);
        element_types.push_back((num_t) players[i]["element_type"] - 1);
        forms.push_back((num_t) std::stof(players[i]["form"].dump().substr(1,3)));
        costs.push_back((num_t) players[i]["now_cost"]);

        if (players[i]["chance_of_playing_next_round"].dump() == "null") {
            playing_next_round.push_back((num_t) 1);
        } else {
            num_t chance = (num_t) players[i]["chance_of_playing_next_round"];
            playing_next_round.push_back(chance / 100.);
        }

        // cost function: minimize lambda * costs - forms
        lp.set_c(i, -forms[i] * playing_next_round[i] + lambda * costs[i]);

        // inequality constraints: no more than 3 players of the same team
        lp.set_a(i, teams[i], 1);

        // equality constraints: match number of players per position
        lp.set_a(i, element_types[i] + 20, 1);      
        lp.set_a(i, element_types[i] + 20 + 4, -1);
    }

    // b: no more than 3 players of the same team
    for (unsigned int i = 0; i < 20; i++) {
        lp.set_b(i, 3);
    }

    // b: match number of players per position
    lp.set_b(20, 2); lp.set_b(24, -2);
    lp.set_b(21, 5); lp.set_b(25, -5);
    lp.set_b(22, 5); lp.set_b(26, -5);
    lp.set_b(23, 3); lp.set_b(27, -3);

    // solve linear program
    Solution s = CGAL::solve_linear_program(lp, ET());

    // assert solution solves linear program and is integral
    assert (s.solves_linear_program(lp));
    for (auto i = s.variable_values_begin(); i != s.variable_values_end(); i++) {
        if (i->numerator() > 0) {
            assert (i->numerator() == 1);
            assert (i->denominator() == 1);
        } else {
            assert (i->numerator() == 0);
            assert (i->denominator() == 1);
        }
    }

    // print output nicely
    std::cout << "Objective Value: " << -s.objective_value() << "\n";
    printTable(s, names, costs, forms);

    return 0;
}