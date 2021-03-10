// PROJECT IDENTIFIER: 40FB54C86566B9DDEAB902CC80E8CE85C1C62AAD
//
//  driver2.cpp
//  project1-treasurehunt
//
//  Created by Samyukta Iyengar on 5/10/20.
//  Copyright © 2020 Samyukta Iyengar. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cassert>
#include "hunting.hpp"
using namespace std;

void print_helper(char *argv[]) {
    cout << "Usage: " << argv[0] << "<options> " << " [inputfile]" << "\n"
    << "Options: " << "\n"
    << "\t-h, --help\t\t Help message" << "\n"
    << "\t-c, --captain <queue | stack\t Container to use for the captain" << "\n"
    << "\t-f, --first-mate <queue | stack\t Container to use for the first mate on land" << "\n"
    << "\t-o, --hunt-order <order>\t Order of discovery of adjacent tiles (default nesw)" << "\n"
    << "\t-v, --verbose\t Print verbose output while searching" << "\n"
    << "\t-s, --stats\t Display statistics after the search is complete" << "\n"
    << "\t-p, --show-path <M|L>\t Display a treasure map or list of locations that describe the path" << "\n";
    return;
}

vector<string> read_file_contents(const std::string& file_name, bool* map_file,
                                  bool* list_file) {
    vector<string> file_contents;
    
    string line;
    ifstream input_file(file_name);
    
    bool file_beginning = true;
    if (input_file.is_open()) {
        while (getline(input_file, line)) {
            if (line[0] == '#' && file_beginning) {
                continue;
            }
            else if ((line == "" || line == "\n") && file_beginning) {
                continue;
            }
            file_beginning = false;
            if (line == "M") {
                *map_file = true;
            }
            if (line == "L") {
                *list_file = true;
            }
            file_contents.push_back(line);
        }
        input_file.close();
    }
    else {
        while (std::getline(std::cin, line)) {
            if (line[0] == '#' && file_beginning) {
                continue;
            }
            else if ((line == "" || line == "\n") && file_beginning) {
                continue;
            }
            file_beginning = false;
            if (line == "M") {
                *map_file = true;
            }
            if (line == "L") {
                *list_file = true;
            }
            file_contents.push_back(line);
        }
    }
    return file_contents;
}

bool check_hunt_order(string hunt_order) {
    sort(hunt_order.begin(), hunt_order.end());
    if (hunt_order != "ensw") {
        cout << "Hunt order should contain one of nesw in any order" << "\n";
        cout << "\n";
        return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    // declaring all the variables we're going to use
    Hunting::Container captain_container_enum  = Hunting::Container::STACK;
    Hunting::Container first_mate_container_enum = Hunting::Container::QUEUE;
    char show_path = ' ';
    string hunt_order;
    string file_name;
    bool verbose = false;
    bool stats = false;
    bool read_map_path = false;
    bool read_captain_container = false;
    bool read_first_mate_container = false;
    bool read_hunt_order = false;
    
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            print_helper(argv);
            return 0;
        }
        else if (arg == "-c" || arg == "--captain" ||
                 read_captain_container) {
            string captain_container;
            if (read_captain_container) {
                captain_container = arg;
                read_captain_container = false;
            }
            else if (i + 1 < argc) {
                captain_container = argv[++i];
            }
            else {
                cout << "No value specified for captain container" << "\n";
                cout << "\n";
                print_helper(argv);
                return 1;
            }
            if (captain_container != "queue" && captain_container != "stack") {
                cout << "Captain container should be one of queue or stack"
                << "\n" << "\n";
                print_helper(argv);
                return 1;
            }
            else if (captain_container == "queue") {
                captain_container_enum = Hunting::Container::QUEUE;
            }
            else {
                captain_container_enum = Hunting::Container::STACK;
            }
        }
        else if (arg == "-f" || arg == "--first-mate" ||
                 read_first_mate_container) {
            string first_mate_container;
            if (read_first_mate_container) {
                first_mate_container = arg;
                read_first_mate_container = false;
            }
            else if (i + 1 < argc) {
                first_mate_container = argv[++i];
            }
            else {
                cout << "No value specified for First mate container" << "\n";
                cout << "\n";
                print_helper(argv);
                return 1;
            }
            
            if (first_mate_container != "queue" &&
                first_mate_container != "stack") {
                cout << "First mate container should be one of queue or stack" << "\n";
                cout << "\n";
                print_helper(argv);
                return 1;
            }
            else if (first_mate_container == "queue") {
                first_mate_container_enum = Hunting::Container::QUEUE;
            }
            else {
                first_mate_container_enum = Hunting::Container::STACK;
            }
        }
        else if (arg == "-o" || arg == "--hunt-order" || read_hunt_order) {
            if (read_hunt_order) {
                hunt_order = arg;
                read_hunt_order = false;
            }
            else if (i + 1 < argc) {
                hunt_order = argv[++i];
            }
            else {
                cout << "No value specified for hunt order" << "\n";
                cout << "\n";
                print_helper(argv);
                return 1;
            }
            
            if (!check_hunt_order(hunt_order)) {
                print_helper(argv);
                return 1;
            }
        }
        else if (arg == "-p" || arg == "--show-path" || read_map_path) {
            if (show_path != ' ') {
                cout << "Show path can only be specified once" << "\n";
                cout << "\n";
                print_helper(argv);
                return 1;
            }
            if (read_map_path) {
                show_path = arg[0];
                read_map_path = false;
            }
            else if (i + 1 < argc) {
                show_path = argv[++i][0];
            }
            else {
                cout << "No value specified for Show path" << "\n";
                cout << "\n";
                print_helper(argv);
                return 1;
            }
            if (show_path != 'M' && show_path != 'L') {
                cout << "Show path can only contain M or L" << "\n";
                cout << "\n";
                print_helper(argv);
                return 1;
            }
        }
        else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        }
        else if (arg == "-s" || arg == "--stats") {
            stats = true;
        }
        else if (arg[0] == '-') {
            if (arg.find("--") != string::npos) {
                cout << "Unsupported option specified";
                cout << "\n";
                print_helper(argv);
                return 1;
            }
            
            bool key_value_pair = false;;
            for (size_t j = 1; j < arg.length(); j++) {
                if (key_value_pair) {
                    cout << "Unsupported option specified";
                    cout << "\n";
                    print_helper(argv);
                    return 1;
                }
                if (arg[j] == 's') {
                    stats = true;
                }
                else if (arg[j] == 'h') {
                    print_helper(argv);
                    return 0;
                }
                else if (arg[j] == 'v') {
                    verbose = true;
                }
                else {
                    if (arg[j] == 'p') {
                        if (show_path != ' ') {
                            cout << "Show path can only be specified once" << "\n";
                            cout << "\n";
                            print_helper(argv);
                            return 1;
                        }
                        if (j + 1 == arg.length() - 1) {
                            if (arg[j + 1] == 'M' || arg[j + 1] == 'L') {
                                show_path = arg[j + 1];
                                break;
                            }
                            else {
                                cout << "Invalid command line option" << "\n";
                                print_helper(argv);
                                return 1;
                            }
                        }
                        key_value_pair = true;
                        // Set a flag indicating that the next command line
                        // being read would contain the map path.
                        if (show_path == ' ') {
                            read_map_path = true;
                        }
                    }
                    else if (arg[j] == 'c') {
                        string next_token = arg.substr(j + 1);
                        if (!next_token.empty()) {
                            if (next_token != "stack" && next_token != "queue") {
                                cout << "Invalid command line option" << "\n";
                                print_helper(argv);
                                return 1;
                            }
                            else {
                                if (next_token == "queue") {
                                    captain_container_enum = Hunting::Container::QUEUE;
                                }
                                else {
                                    captain_container_enum = Hunting::Container::STACK;
                                }
                                
                                break;
                            }
                        }
                        // Set a flag indicating that the next command line
                        // being read would contain the captain container.
                        key_value_pair = true;
                        read_captain_container = true;
                    }
                    else if (arg[j] == 'f') {
                        string next_token = arg.substr(j + 1);
                        if (!next_token.empty()) {
                            if (next_token != "stack" && next_token != "queue") {
                                cout << "Invalid command line option" << "\n";
                                print_helper(argv);
                                return 1;
                            }
                            else {
                                if (next_token == "queue") {
                                    first_mate_container_enum = Hunting::Container::QUEUE;
                                }
                                else {
                                    first_mate_container_enum = Hunting::Container::STACK;
                                }
                                break;
                            }
                        }
                        // Set a flag indicating that the next command line
                        // being read would contain the first mate container.
                        key_value_pair = true;
                        read_first_mate_container = true;
                    }
                    else if (arg[j] == 'o') {
                        string next_token = arg.substr(j + 1);
                        if (!next_token.empty()) {
                            if (!check_hunt_order(next_token)) {
                                print_helper(argv);
                                return 1;
                            }
                            hunt_order = next_token;
                            break;
                        }
                        // Set a flag indicating that the next command line
                        // being read would contain the hunt order.
                        key_value_pair = true;
                        read_hunt_order = true;
                    }
                    else {
                        cout << "Unsupported option specified";
                        cout << "\n";
                        print_helper(argv);
                        return 1;
                    }
                }
            }
        }
    }
    if (hunt_order.empty()) {
        hunt_order = "nesw";
    }
    
    bool map_file = false;
    bool list_file = false;
    
    if (argc > 1) {
        file_name = argv[argc - 1];
    }
    
    Hunting hunting(hunt_order, captain_container_enum,
                    first_mate_container_enum, verbose);
    
    {
        vector<string> file_contents = read_file_contents(file_name, &map_file,
                                                          &list_file);
        
        // either map or list file should be true
        //assert(map_file == true || list_file == true);
        // both cannot be true
        //assert(map_file != list_file);
        
        if (map_file) {
            hunting.read_map_contents(file_contents);
        }
        else if (list_file) {
            hunting.read_list_contents(file_contents);
        }
    }
    
    bool treasure = hunting.hunt();
    
    if (stats) {
        hunting.print_stats();
    }
    
    hunting.show_path(show_path);
   
    hunting.print_treasure_found(treasure);
}
