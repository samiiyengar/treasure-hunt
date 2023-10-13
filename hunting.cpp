// PROJECT IDENTIFIER: 40FB54C86566B9DDEAB902CC80E8CE85C1C62AAD
//
//  hunting.cpp
//  project1-treasurehunt
//
//  Created by Samyukta Iyengar on 5/7/20.
//  Copyright © 2020 Samyukta Iyengar. All rights reserved.
//

#include "hunting.hpp"
#include <cassert>
#include <sstream>

Hunting::Hunting(string order_in, Container captain_container_in, Container first_mate_container_in, bool verbose)
: order(order_in), captain_container_type(captain_container_in), first_mate_container_type(first_mate_container_in),
treasure_is_found(false), verbose_output(verbose) {
}

Hunting::~Hunting() {}

void Hunting::read_map_contents(const vector<string>& input) {
    //assert(input.size() > 2);
    
    //assert(input[0] == "M");
    
    size_t N = static_cast<size_t>(stoi(input[1]));
    for (size_t i = 0; i < N; ++i) {
        grid.push_back(vector<char>());
    }
    
    for (size_t i = 2, map_index = 0; i < input.size(); ++i, ++map_index) {
        //assert(input[i].length() == N);
        size_t col = 0;
        for (const char& ch : input[i]) {
            grid[map_index].push_back(ch);
            if (ch == '@') {
                captain_start_location.first = i - 2;
                captain_start_location.second = col;
            }
            col++;
        }
    }
    if (!grid.empty()) {
        discovered_spots.resize(grid.size(), vector<bool>(grid[0].size(), false));
        parents.resize(grid.size(), vector<char>(grid[0].size(), ' '));
    }
}

void Hunting::read_list_contents(const vector<string>& input) {
    //assert(input.size() > 2);
    
    // skip blank or new lines
    size_t index = 0;
    while (input[index] == "\n" || input[index] == "") {
        index++;
    }
    //assert(input[index] == "L");
    index++;
    
    // skip blank or new lines
    while (input[index] == "\n" || input[index] == "") {
        index++;
    }
    size_t N = static_cast<size_t>(stoi(input[index]));
    index++;
    
    for (size_t i = 0; i < N; ++i) {
        grid.push_back(vector<char>());
        
        for (size_t j = 0; j < N; ++j) {
            grid[i].push_back('.');
        }
    }
    
    for (size_t i = index; i < input.size(); i++) {
        // skip blank or new lines
        if (input[i] == "\n" || input[i] == "" ||
            input[i].find_first_not_of(' ') == string::npos) {
            continue;
        }
        
        const string& data = input[i];
        
        size_t initial_pos = 0;
        size_t row = input.size() + 1;
        size_t column = input.size() + 1;
        char terrain = 0;
        
        size_t pos = data.find(' ');
        while (pos != string::npos) {
            if (row == input.size() + 1) {
                row = static_cast<size_t>(stoi(data.substr(initial_pos, pos - initial_pos)));
            } else if (column == input.size() + 1) {
                column = static_cast<size_t>(stoi(data.substr(initial_pos, pos - initial_pos)));
            }
            else {
                // Terrain should be a single character like ., o, $, #, etc.
                terrain = data.substr(initial_pos)[0];
                break;
            }
            initial_pos = pos + 1;
            pos = data.find(' ', initial_pos);
        }
        if (terrain == 0) {
            terrain = data.substr(initial_pos)[0];
        }
        grid[row][column] = terrain;
        if (terrain == '@') {
            captain_start_location.first = row;
            captain_start_location.second = column;
        }
    }
    if (!grid.empty()) {
        discovered_spots.resize(grid.size(), vector<bool>(grid[0].size(), false));
        parents.resize(grid.size(), vector<char>(grid[0].size(), ' '));
    }
}

bool Hunting::hunt() {
    bool treasure_found = captain_search();

    discovered_spots.clear();

    if (treasure_found) {
        // Calculate the whole path if needed.
        calculate_path_length();
    }
    else {
        if (verbose_output) {
            cout << "Treasure hunt failed\n";
        }
    }
    return treasure_found;
}

bool Hunting::captain_search() {
    if (grid.empty() || grid[0].empty()) {
        return false;
    }
    // check that start location was found
    if (grid[captain_start_location.first][captain_start_location.second] != '@') {
        return false;
    }
    
    if (verbose_output) {
        cout << "Treasure hunt started at: " << captain_start_location.first << ","
            << captain_start_location.second << "\n";
    }

    deque<Point> d;
    
    // add starting location to discovered spots
    d.push_back(captain_start_location);
    add_discovered_spot(captain_start_location);
    
    // the actual hunt
    while (!d.empty()) {
        Point current_location;
        
        if (captain_container_type == Container::STACK) {
            current_location = d.back();
            d.pop_back();
        }
        else {
            current_location = d.front();
            d.pop_front();
        }
        
        // increment water locations
        if (is_water_location(current_location)) {
            water_locations++;
        }
        
        // check every direction and move accordingly
        for (char direction : order) {
            char opp_direction = ' ';
            Point next_location;
            if (direction == 'n') {
                next_location.first = current_location.first - 1;
                next_location.second = current_location.second;
                opp_direction = 's';
            }
            else if (direction == 's') {
                next_location.first = current_location.first + 1;
                next_location.second = current_location.second;
                opp_direction = 'n';
            }
            else if (direction == 'w') {
                next_location.first = current_location.first;
                next_location.second = current_location.second - 1;
                opp_direction = 'e';
            }
            else {
                next_location.first = current_location.first;
                next_location.second = current_location.second + 1;
                opp_direction = 'w';
            }
            if (is_valid_move(next_location)) {
                parents[next_location.first][next_location.second] = opp_direction;

                add_discovered_spot(next_location);
                // if treasure is found, you stop
                if (grid[next_location.first][next_location.second] == '$') {
                    if (verbose_output) {
                        cout << "Went ashore at: " << next_location.first << ","
                            << next_location.second << "\n";
                    }

                    times_ashore++;
                    treasure_is_found = true;
                    treasure_location = next_location;

                    if (verbose_output) {
                        cout << "Searching island... party found treasure at " <<
                            next_location.first << "," << next_location.second << ".\n";
                    }

                    land_locations++;
                    return true;
                }
                
                if (is_land_location(next_location)) {
                    bool ret = first_mate_search(next_location);
                    if (ret) {
                        treasure_is_found = true;
                        return true;
                    }
                    continue;
                }
                
                d.push_back(next_location);
            }
        }
    }
    return treasure_is_found;
}

bool Hunting::first_mate_search(const Point& location) {
    deque<Point> d;
    d.push_back(location);
    
    if (verbose_output) {
        cout << "Went ashore at: " << location.first << ","
            << location.second << "\n";
    }

    times_ashore++;
    
    while (!d.empty()) {
        Point current_location;
        
        if (first_mate_container_type == Container::STACK) {
            current_location = d.back();
            d.pop_back();
        }
        else {
            current_location = d.front();
            d.pop_front();
        }
        
        land_locations++;
        
        // check every direction and move accordingly
        for (char direction : order) {
            Point next_location;
            char opp_direction = ' ';
            if (direction == 'n') {
                next_location.first = current_location.first - 1;
                next_location.second = current_location.second;
                opp_direction = 's';
            }
            else if (direction == 's') {
                next_location.first = current_location.first + 1;
                next_location.second = current_location.second;
                opp_direction = 'n';
            }
            else if (direction == 'w') {
                next_location.first = current_location.first;
                next_location.second = current_location.second - 1;
                opp_direction = 'e';
            }
            else {
                next_location.first = current_location.first;
                next_location.second = current_location.second + 1;
                opp_direction = 'w';
            }
            if (is_valid_move_for_first_mate(next_location)) {
                parents[next_location.first][next_location.second] = opp_direction;

                if (grid[next_location.first][next_location.second] == '$') {
                    treasure_is_found = true;
                    treasure_location = next_location;
                    if (verbose_output) {
                        cout << "Searching island... party found treasure at " <<
                            next_location.first << "," << next_location.second << ".\n";
                    }
                    //show_search_state("Searching island... party found treasure at ",//next_location,   //      true);
                    land_locations++;
                    return true;
                }
                
                d.push_back(next_location);
                add_discovered_spot(next_location);
            }
        }
    }
    if (verbose_output) {
        cout << "Searching island... party returned with no treasure.\n";
    }
    //show_search_state("Searching island... party returned with no treasure.",        Point(), false);
    return false;
}

bool Hunting::is_out_of_bounds(const Point &loc) const {
    if (static_cast<int>(loc.first) < 0 || loc.first >= grid.size()) {
        return true;
    }
    if (static_cast<int>(loc.second) < 0 || loc.second >= grid[0].size()) {
        return true;
    }
    return false;
}

bool Hunting::is_valid_move(const Point &loc) const {
    if (is_out_of_bounds(loc)) {
        return false;
    }
    if (grid[loc.first][loc.second] == '#') {
        return false;
    }
    return !discovered_spots[loc.first][loc.second];
}

bool Hunting::is_valid_move_for_first_mate(const Point& loc) const {
    if (!is_valid_move(loc)) {
        return false;
    }
    return is_land_location(loc);
}

void Hunting::add_discovered_spot(const Point &spot) {
    discovered_spots[spot.first][spot.second] = true;
}

void Hunting::print_stats() const {
    cout << "--- STATS ---\n"
    << "Starting location: "
    << captain_start_location.first << ","
    << captain_start_location.second
    << "\nWater locations investigated: "
    << water_locations
    << "\nLand locations investigated: "
    << land_locations
    << "\nWent ashore: "
    << times_ashore
    << "\n";
    
    if (treasure_is_found) {
        cout << "Path length: "
        << get_path_length()
        << "\nTreasure location: "
        << treasure_location.first
        << ","
        << treasure_location.second
        << "\n";
    }
    cout << "--- STATS ---\n";
}

void Hunting::print_treasure_found(bool treasure) const {
    if (treasure) {
        cout << "Treasure found at " << treasure_location.first << "," << treasure_location.second
             << " with path length " << get_path_length() << ".\n";
    }
    else {
        int investigated_locations = water_locations + land_locations;
        cout << "No treasure found after investigating " << investigated_locations
            << " locations.\n";
    }
    return;
}

void Hunting::calculate_path_length() {
    if (!treasure_is_found || path_length > 0) {
        return;
    }
 
    path_length = 0;
    Point current = treasure_location;
    while (current != captain_start_location) {
        char direction = parents[current.first][current.second];
        switch (direction) {
        case 'n':
            current.first--;
            break;
        case 's':
            current.first++;
            break;
        case 'e':
            current.second++;
            break;
        case 'w':
            current.second--;
            break;
        default:
            break;
        }
        path_length++;
    }
}

size_t Hunting::get_path_length() const {
    return path_length;
}

void Hunting::show_path(char path_type) {
    if (!treasure_is_found) {
        return;
    }
    
    if (path_type == 'M') {
        draw_map();
    }
    else if (path_type == 'L') {
        draw_list();
    }
}

void Hunting::draw_map() {
    bool prev_vertical = false;
    bool prev_horizontal = false;

    grid[treasure_location.first][treasure_location.second] = 'X';

    Point prev = treasure_location;

    Point current = treasure_location;

    while (current != captain_start_location) {
        char direction = parents[current.first][current.second];
        switch (direction) {
        case 'n':
            current.first--;
            break;
        case 's':
            current.first++;
            break;
        case 'e':
            current.second++;
            break;
        case 'w':
            current.second--;
            break;
        default:
            break;
        }

        if (current == prev) {
            continue;
        }

        if (current.first == prev.first) {
            grid[current.first][current.second] = '-';
            if (prev_vertical) {
                grid[prev.first][prev.second] = '+';
            }
            prev_vertical = false;
            prev_horizontal = true;
        } else {
            grid[current.first][current.second] = '|';
            if (prev_horizontal) {
                grid[prev.first][prev.second] = '+';
            }
            prev_vertical = true;
            prev_horizontal = false;
        }
        prev = current;
    }

    grid[captain_start_location.first][captain_start_location.second] = '@';

    string output;
    for (const vector<char> &row : grid) {
        for (const char &c : row) {
            output += c;
        }
        output += "\n";
    }
    cout << output;
}

void Hunting::draw_list() {
    vector<Point> path(path_length);
    
    Point current = treasure_location;
    size_t i = 0;
    while (current != captain_start_location) {
        path[i] = current;

        char direction = parents[current.first][current.second];
        switch (direction) {
        case 'n':
            current.first--;
            break;
        case 's':
            current.first++;
            break;
        case 'e':
            current.second++;
            break;
        case 'w':
            current.second--;
            break;
        default:
            break;
        }
        i++;
    }

    cout << "Sail:\n" << captain_start_location.first << "," << captain_start_location.second
         << "\n";

    string land_output = "Search:\n";

    for (vector<Point>::reverse_iterator iter = path.rbegin(); iter != path.rend(); ++iter) {
        const Point& pt = *iter;
        if (is_water_location(pt)) {
            cout << pt.first << "," << pt.second << "\n";
        } else if (is_land_location(pt)) {
            land_output += to_string(pt.first) + "," + to_string(pt.second) + "\n";
        }
    }

    cout << land_output;
}

bool Hunting::is_water_location(const Point &loc) const {
    if (grid[loc.first][loc.second] == '.' || (grid[loc.first][loc.second] == '@')) {
        return true;
    }
    return false;
}

bool Hunting::is_land_location(const Point &loc) const {
    if (grid[loc.first][loc.second] == 'o' || (grid[loc.first][loc.second] == '$')) {
        return true;
    }
    return false;
}
