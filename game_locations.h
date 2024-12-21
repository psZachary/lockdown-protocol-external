#pragma once
#include <unordered_map>
#include <string>
#include "game_structures.hpp"

using namespace protocol::engine::sdk;
using namespace protocol::game::sdk;
using namespace protocol::engine;

// Define a struct to hold both position and rotation for each location
struct LocationData {
    FVector position;
    FRotator rotation;
};

// Ordered list of location names
inline std::vector<std::string> orderedLocationNames = {
    // Game Area
    "Lobby", "Tutorial Room", "Machine", "Storage", "Medical", "Security",
    "Pizzushi", "Garden", "Lab", "Computers",
    // Out of Bounds
    "Yellow Apartment", "Red Apartment", "Storage Catwalk", "Medical Hallway",
    "Pizzushi Glass Room", "Pizzushi Aquarium", "Glass Roof Spawn", "Lobby Glass Room"
};

// Map of predefined locations with their positions and rotations
inline std::unordered_map<std::string, LocationData> predefinedLocations = {
    // Game Area
    {"Lobby", {FVector(11092.68f, 12646.16f, 500.0f), FRotator(0.0, 0.0, 0.0)}},
    {"Tutorial Room", {FVector(11059.0f, 19629.5f, 500.0f), FRotator(0.0, 0.0, 0.0)}},
    {"Machine", {FVector(5159.9f, 11440.0f, 0.0f), FRotator(0.0, 0.0, 0.0)}},
    {"Storage", {FVector(3759.9f, 8559.9f, 83.0f), FRotator(0.0, 0.0, 0.0)}},
    {"Medical", {FVector(6640.17f, 3640.0f, 0.0f), FRotator(0.0, 0.0, 0.0)}},
    {"Security", {FVector(10141.4f, 11959.99f, 0.0f), FRotator(0.0, 0.0, 0.0)}},
    {"Pizzushi", {FVector(7460.0f, 7940.0f, 0.0f), FRotator(0.0, 0.0, 0.0)}},
    {"Garden", {FVector(11140.0f, 2740.0f, 0.0f), FRotator(0.0, 0.0, 0.0)}},
    {"Lab", {FVector(8759.99f, 5544.90f, 1.15f), FRotator(0.0, 0.0, 0.0)}},
    {"Computers", {FVector(11082.7f, 6245.15f, 0.0f), FRotator(0.0, 0.0, 0.0)}},
    // Out of Bounds
    {"Yellow Apartment", {FVector(10686.7f, 6287.19f, 500.0f), FRotator(0.0, 0.0, 0.0)}},
    {"Red Apartment", {FVector(8496.13f, 10656.75f, 1000.0f), FRotator(0.0, 0.0, 0.0)}},
    {"Pizzushi Apartment", {FVector(7460.0f, 7940.0f, 500.0f), FRotator(0.0, 0.0, 0.0)}},
    {"Storage Catwalk", {FVector(5105.84f, 8560.0f, 500.0f), FRotator(0.0, 0.0, 0.0)}},
    {"Medical Hallway", {FVector(3706.26f, 3631.11f, 0.0f), FRotator(0.0, 0.0, 0.0)}},
    {"Pizzushi Glass Room", {FVector(5940.0f, 7940.0f, 0.0f), FRotator(0.0, 0.0, 0.0)}},
    {"Pizzushi Aquarium", {FVector(5802.8f, 7024.61f, 125.14f), FRotator(0.0, 0.0, 0.0)}},
    {"Glass Roof Spawn", {FVector(10596.48f, 8701.45f, 1500.0f), FRotator(0.0, 0.0, 0.0)}},
    {"Lobby Glass Room", {FVector(10128.07f, 12433.84f, 500.0f), FRotator(0.0, 0.0, 0.0)}}
};