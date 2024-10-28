// ItemProperties.h

#pragma once

#include <unordered_map>
#include <string>
#include <iostream>

struct ItemProperties {
    double melee_cast_time;
    double melee_recover_time;
    double melee_stun;
    int melee_range;
    int melee_cost;

    ItemProperties() : melee_cast_time(0), melee_recover_time(0), melee_stun(0), melee_range(0), melee_cost(0) {}
    ItemProperties(double cast, double recover, double stun, int range, int cost)
        : melee_cast_time(cast), melee_recover_time(recover), melee_stun(stun), melee_range(range), melee_cost(cost) {}
};

// Declare itemData with extern to signal it's defined elsewhere
extern std::unordered_map<std::string, ItemProperties> itemData;

void InitializeItems();
ItemProperties GetItemProperties(const std::string& itemName);
