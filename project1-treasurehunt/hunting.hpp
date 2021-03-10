// PROJECT IDENTIFIER: 40FB54C86566B9DDEAB902CC80E8CE85C1C62AAD
//
//  hunting.hpp
//  project1-treasurehunt
//
//  Created by Samyukta Iyengar on 5/7/20.
//  Copyright © 2020 Samyukta Iyengar. All rights reserved.
//

#ifndef hunting_hpp
#define hunting_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <deque>
#include <list>
#include <algorithm>

using namespace std;

#endif /* hunting_hpp */

struct Point {
    Point(size_t row, size_t col)
    : first(row), second(col) {}
    Point() {}

    size_t first = 0;
    size_t second = 0;
    
    bool operator != (const Point& pt) const {
        return !(pt.first == first && pt.second == second);
    }
    
    bool operator == (const Point& pt) const {
        return (pt.first == first && pt.second == second);
    }
};

// hunting class
class Hunting {
    
public:
    enum Container {
        STACK = 0, QUEUE = 1
    };
    
    Hunting(string order_in = "nesw",
            Container captain_container_in = STACK,
            Container first_mate_container_in = QUEUE,
            bool verbose_output = false);
    ~Hunting();
    
    // reads and initializes map
    void read_map_contents(const vector<string>& input);
    
    void read_list_contents(const vector<string>& input);
    
    bool hunt();
    
    bool captain_search();

    void print_stats() const;
    
    void print_treasure_found(bool treasure) const;
    
    void show_path(char path_type);
    
private:
    
    bool first_mate_search(const Point& current_location);
    
    bool is_out_of_bounds(const Point &loc) const;
    
    bool is_valid_move(const Point &loc) const;
    
    bool is_valid_move_for_first_mate(const Point& loc) const;
    
    void add_discovered_spot(const Point &spot);
    
    void calculate_path_length();
    
    size_t get_path_length() const;
    
    void draw_map();
    
    void draw_list();

    // copy of map
    vector<vector<char>> grid;
    
    // all spots that have already been discovered
    // mark discovered spots as true, others as false
    vector<vector<bool>> discovered_spots;
    
    // Used to track the parent chain leading up to the treasure
    // location.
    vector<vector<char>> parents;

    // our order
    string order;
    
    // where the treasure was eventually found
    Point treasure_location;
    
    // captain's start location
    Point captain_start_location;
    
    // number of discovered water spots for stats
    int water_locations = 0;
    bool is_water_location(const Point &loc) const;
    
    // number of discovered land spots for stats
    int land_locations = 0;
    bool is_land_location(const Point &loc) const;
    
    // number of times gone ashore for stats
    int times_ashore = 0;
    
    // Length of the path.
    size_t path_length = 0;
    
    // Type of containers.
    Container captain_container_type = STACK;
    
    Container first_mate_container_type = QUEUE;
    
    // check if treasure is found
    bool treasure_is_found = false;

    // Verbose output.
    bool verbose_output = false;
};
