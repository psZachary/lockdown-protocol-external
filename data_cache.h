#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "game_structures.hpp"

using namespace protocol::game::sdk;

extern std::vector<world_item*> temp_world_item_cache;
extern std::unordered_map<std::string, u_data_item*> unique_item_data;
extern std::vector<std::string> item_names;

void PopulateUniqueItems();  // Function declaration
