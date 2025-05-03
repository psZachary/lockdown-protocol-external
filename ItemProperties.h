// ItemProperties.h

#pragma once

#include <unordered_map>
#include <string>
#include <iostream>

struct ItemProperties {
    // Common
    bool is_melee = true;

    // Melee-specific properties
    double melee_cast_time = 0;
    double melee_recover_time = 0;
    double melee_stun = 0;
    int melee_range = 0;
    int melee_cost = 0;

    // Gun-specific properties
    bool auto_fire = false;
    double fire_rate = 0;
    int damage = 0;
    double shake_intensity = 0;
    double oscillation_reactivity = 0;
    double walk_oscillation = 0;
    double run_oscillation = 0;
    double stand_oscillation = 0;
    double recoil_reactivity = 0;
    double walk_precision = 0;
    double air_precision = 0;
    double run_precision = 0;
    double stun = 0;

    // Default constructor
    ItemProperties() = default;

    // Melee constructor
    ItemProperties(double cast, double recover, double stun, int range, int cost)
        : is_melee(true), melee_cast_time(cast), melee_recover_time(recover), melee_stun(stun),
        melee_range(range), melee_cost(cost) {}

    // Gun constructor
    ItemProperties(bool autoFire, double rate, int dmg, double shake, double oscill_reactivity,
        double walk_osc, double run_osc, double stand_osc, double recoil_react,
        double walk_prec, double air_prec, double run_prec, double stun)
        : is_melee(false), auto_fire(autoFire), fire_rate(rate), damage(dmg),
        shake_intensity(shake), oscillation_reactivity(oscill_reactivity),
        walk_oscillation(walk_osc), run_oscillation(run_osc), stand_oscillation(stand_osc),
        recoil_reactivity(recoil_react), walk_precision(walk_prec),
        air_precision(air_prec), run_precision(run_prec), stun(stun) {}
};

// Declare itemData with extern to signal it's defined elsewhere
extern std::unordered_map<std::string, ItemProperties> itemData;

void InitializeItems();
ItemProperties GetItemProperties(const std::string& itemName);
