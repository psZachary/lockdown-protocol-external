#pragma once

#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>
#include "game_structures.hpp"
using namespace protocol::engine::sdk;
using namespace protocol::game::sdk;
namespace config {
	// Hotkeys
	inline int menu_hotkey = VK_F1;
	inline int esp_hotkey = VK_F2;
	inline int speedhack_hotkey = VK_F3;
	inline int god_mode_hotkey = VK_F4;
	inline int infinite_stamina_hotkey = VK_F5;
	inline int fast_melee_hotkey = VK_F6;
	inline int infinite_melee_range_hotkey = VK_F7;
	inline int auto_fire_hotkey = VK_F8;
	inline int rapid_fire_hotkey = VK_F9;
	inline int no_recoil_hotkey = VK_INSERT;
	inline int max_damage_hotkey = VK_DELETE;
	inline int aimbot_hotkey = VK_PRIOR;
	inline int aimbot_hold_key = VK_LMENU;
	inline int player_list_hotkey = VK_NEXT;

	inline bool isDebugging = false;

	// Player
	inline bool speedhack = false;
	inline double max_speed = 2000.00; // 800.0 is default value run speed
	inline double friction = 100000.00; // 0.0 is default value friction
	inline bool infinite_stamina = false;
	inline bool god_mode = false;
	inline bool fast_hp_recovery = false;
	inline bool fast_stam_recovery = false;
	inline bool rainbowsuit = false;
	inline int rainbow_speed = 200;
	inline int hp_recovery_rate = 100;
	inline int stam_recovery_rate = 100;
	inline int player_fov = 103;
	inline bool admin_toggle = false;

	// Aimbot
	inline bool aimbot = true;
	inline bool target_closest = false;
	inline std::string target_filter = "All";
	inline std::string aim_target = "Head";
	inline float smooth_factor = 0.50;
	inline float aimbot_fov = 120.0;
	inline ImVec4 fov_color = ImVec4(1.0f, 1.0f, 1.0f, 0.392f);  // White transparent
	inline bool anti_weapon_drop = false;
	inline float drop_threshold = 25.0;

	// Weapons
	// Melee
	inline bool fast_melee = false;
	inline double cast_time = 0.0; // avg is 0.1 : 0.5
	inline double recover_time = 0.0; // avg is 0.4 : 1.0
	inline double stun = 1.0; // avg is -1 : 0.4
	inline int cost = 0; // avg is 20 : 40
	inline bool infinite_melee_range = false;
	inline int range = 10000; // avg is 130 : 180

	// Guns
	inline bool max_damage = false;
	inline bool auto_fire = false;
	inline bool rapid_fire = false;
	inline double rapid_fire_rate = 0.02; // avg 0.07 : 0.3
	inline bool no_recoil = false;
	inline double movement_osc = 0.0; // avg 0.3 : 20
	inline double osc_reactivity = 0.0; // avg 4 : 100
	inline double movement_prec = 0.0; // avg 0.5 : 40
	inline double recoil_react = 0.0; // avg 1 : 40
	inline double shake_intensity = 0.0; // avg 0.5 : 5
	inline double fire_spread = 0.0;
	inline bool infinite_ammo = false;
	inline bool impact_change = false;
	inline int ammo_count = 12;
	inline double impact_type = 1.0;

	// Inventory
	inline bool can_inventory = false;
	inline bool lock_hand_item = false;
	inline bool lock_bag_item = false;
	inline bool item_puke = false;

	// Inventory Lock Variables
	inline u_data_item* locked_hand_item = nullptr; // Store the locked hand item
	inline FStr_ItemState locked_hand_state;        // Store the locked item state
	inline u_data_item* locked_bag_item = nullptr; // Store the locked bag item
	inline FStr_ItemState locked_bag_state;        // Store the locked item state

	// ESP
	inline bool esp_enabled = true;
	inline double esp_max_distance = 70.0;

	inline bool esp_radar = true;
	inline std::string esp_radar_position = "Middle Right";
	inline double esp_radar_scale = 0.06;

	inline bool player_esp = true;
	inline bool player_distance = true;
	inline bool player_inventory = true;
	inline bool player_box = true;
	inline bool player_ghost = true;
	inline bool player_radar = true;
	inline bool ghost_radar = false;
	inline bool player_list = false;
	inline bool player_list_locked = false;
	inline float player_list_x = 530;
	inline float player_list_y = 20;
	inline bool list_inv = false;
	inline bool fly_mode = false;

	inline ImVec4 employee_color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
	inline ImVec4 dissident_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red 
	inline ImVec4 ghost_employee_color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
	inline ImVec4 ghost_dissident_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red 

	inline bool weapon_esp = true;
	inline bool weapon_case_esp = true;
	inline bool weapon_item_state = true;
	inline bool weapon_case_state = false;
	inline bool weapon_distance = false;
	inline bool weapon_case_distance = false;
	inline bool weapon_case_info = false;
	inline bool weapon_radar = true;
	inline ImVec4 weapon_color = ImVec4(250.0f / 255.0f, 92.0f / 255.0f, 0.0f / 255.0f, 1.0f); // Orange
	inline ImVec4 weapon_case_color = ImVec4(250.0f / 255.0f, 92.0f / 255.0f, 0.0f / 255.0f, 1.0f); // Orange

	inline bool primary_object_esp = true;
	inline bool primary_item_state = false;
	inline bool primary_distance = false;
	inline bool primary_radar = false;
	inline ImVec4 gaz_bottle_color = ImVec4(0.0f / 255.0f, 83.0f / 255.0f, 250.0f / 255.0f, 1.0f); // Blue
	inline ImVec4 vent_filter_color = ImVec4(65.0f / 255.0f, 115.0f / 255.0f, 217.0f / 255.0f, 1.0f); // Light Blue
	inline ImVec4 rice_color = ImVec4(201.0f / 255.0f, 169.0f / 255.0f, 122.0f / 255.0f, 1.0f); // Tan
	inline ImVec4 package_color = ImVec4(87.0f / 255.0f, 47.0f / 255.0f, 24.0f / 255.0f, 1.0f); // Brown
	inline ImVec4 sample_color = ImVec4(92.0f / 255.0f, 6.0f / 255.0f, 191.0f / 255.0f, 1.0f); // Purple
	inline ImVec4 access_card_color = ImVec4(66.0f / 255.0f, 245.0f / 255.0f, 191.0f / 255.0f, 1.0f); // Mint green #42f5bf

	inline bool secondary_item_state = false;
	inline bool secondary_distance = false;
	inline bool secondary_object_esp = true;
	inline bool secondary_radar = false;
	inline ImVec4 fuse_color = ImVec4(105.0f / 255.0f, 105.0f / 255.0f, 105.0f / 255.0f, 1.0f); // Grey
	inline ImVec4 battery_color = ImVec4(189.0f / 255.0f, 189.0f / 255.0f, 189.0f / 255.0f, 1.0f); // Light Grey
	inline ImVec4 screw_driver_color = ImVec4(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1.0f); // White
	inline ImVec4 container_color = ImVec4(136.0f / 255.0f, 0.0f / 255.0f, 145.0f / 255.0f, 1.0f); // Magenta
	inline ImVec4 egg_color = ImVec4(67.0f / 255.0f, 140.0f / 255.0f, 95.0f / 255.0f, 1.0f); // Egg Green (#438c5f)
	inline ImVec4 machine_part_color = ImVec4(58.0f / 255.0f, 58.0f / 255.0f, 255.0f / 166.0f, 1.0f); // Blue-Violet (##3a3aa6)

	inline bool task_object_esp = true;
	inline bool task_object_distance = true;
	inline bool task_object_state = true;

	inline bool task_delivery = true;
	inline ImVec4 task_delivery_color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow

	inline bool task_machine = true;
	inline ImVec4 task_machine_color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow

	inline bool task_vent = true;
	inline ImVec4 task_vent_color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow

	inline bool task_alim = true;
	inline ImVec4 task_alim_color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow

	inline bool task_pizzushi = true;
	inline ImVec4 task_pizzushi_color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow

	inline bool task_computers = true;
	inline ImVec4 task_computer_color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow

	inline bool task_scanners = true;
	inline ImVec4 task_scanner_color = ImVec4(0.0f, 1.0f, 1.0f, 1.0f); // Cyan

	inline bool rez_esp = true;
	inline ImVec4 rez_color = ImVec4(0.0f, 1.0f, 1.0f, 1.0f); // Cyan
	inline ImVec4 defib_color = ImVec4(0.0f, 1.0f, 1.0f, 1.0f); // Cyan

	inline bool alarm_esp = true;
	inline ImVec4 alarm_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red 

	// Function to convert ImVec4 to JSON and vice versa
	inline void to_json(nlohmann::json& j, const ImVec4& color) {
		j = nlohmann::json{ {"r", color.x}, {"g", color.y}, {"b", color.z}, {"a", color.w} };
	}

	inline void from_json(const nlohmann::json& j, ImVec4& color) {
		j.at("r").get_to(color.x);
		j.at("g").get_to(color.y);
		j.at("b").get_to(color.z);
		j.at("a").get_to(color.w);
	}

	// Function to save config values to a JSON file
	inline void SaveConfig(const std::string& filename = "config.json") {
		std::string directory = "config";
		std::filesystem::create_directories(directory);  // Creates directory if it doesn't exist

		// Construct the full path to the config file
		std::string filePath = directory + "/" + filename;

		nlohmann::json j;
		// Save hotkey settings
		j["menu_hotkey"] = menu_hotkey;
		j["esp_hotkey"] = esp_hotkey;
		j["speedhack_hotkey"] = speedhack_hotkey;
		j["god_mode_hotkey"] = god_mode_hotkey;
		j["infinite_stamina_hotkey"] = infinite_stamina_hotkey;
		j["fast_melee_hotkey"] = fast_melee_hotkey;
		j["infinite_melee_range_hotkey"] = infinite_melee_range_hotkey;
		j["auto_fire_hotkey"] = auto_fire_hotkey;
		j["rapid_fire_hotkey"] = rapid_fire_hotkey;
		j["no_recoil_hotkey"] = no_recoil_hotkey;
		j["max_damage_hotkey"] = max_damage_hotkey;
		j["aimbot_hotkey"] = aimbot_hotkey;
		j["aimbot_hold_key"] = aimbot_hold_key;
		j["player_list_hotkey"] = player_list_hotkey;

		// Save boolean settings
		j["speedhack"] = speedhack;
		j["max_speed"] = max_speed;
		j["friction"] = friction;
		j["infinite_stamina"] = infinite_stamina;
		j["god_mode"] = god_mode;
		j["fast_hp_recovery"] = fast_hp_recovery;
		j["fast_stam_recovery"] = fast_stam_recovery;
		j["hp_recovery_rate"] = hp_recovery_rate;
		j["stam_recovery_rate"] = stam_recovery_rate;
		j["player_fov"] = player_fov;
		j["fast_melee"] = fast_melee;
		j["cast_time"] = cast_time;
		j["recover_time"] = recover_time;
		j["stun"] = stun;
		j["cost"] = cost;
		j["infinite_melee_range"] = infinite_melee_range;
		j["range"] = range;
		j["max_damage"] = max_damage;
		j["auto_fire"] = auto_fire;
		j["rapid_fire"] = rapid_fire;
		j["rapid_fire_rate"] = rapid_fire_rate;
		j["no_recoil"] = no_recoil;
		j["movement_osc"] = movement_osc;
		j["osc_reactivity"] = osc_reactivity;
		j["infinite_ammo"] = infinite_ammo;
		j["ammo_count"] = ammo_count;
		j["can_inventory"] = can_inventory;
		j["aimbot"] = aimbot;
		j["target_closest"] = target_closest;
		j["target_filter"] = target_filter;
		j["smooth_factor"] = smooth_factor;
		j["aim_target"] = aim_target;
		j["aimbot_fov"] = aimbot_fov;
		j["anti_weapon_drop"] = anti_weapon_drop;
		j["drop_threshold"] = drop_threshold;

		// Save ESP-related settings
		j["esp_enabled"] = esp_enabled;
		j["esp_max_distance"] = esp_max_distance;
		j["esp_radar"] = esp_radar;
		j["esp_radar_position"] = esp_radar_position;
		j["esp_radar_scale"] = esp_radar_scale;
		j["player_esp"] = player_esp;
		j["player_distance"] = player_distance;
		j["player_inventory"] = player_inventory;
		j["player_box"] = player_box;
		j["player_ghost"] = player_ghost;
		j["player_radar"] = player_radar;
		j["ghost_radar"] = ghost_radar;
		j["player_list"] = player_list;
		j["player_list_locked"] = player_list_locked;
		j["player_list_x"] = player_list_x;
		j["player_list_y"] = player_list_y;
		j["list_inv"] = list_inv;
		j["weapon_esp"] = weapon_esp;
		j["weapon_case_esp"] = weapon_case_esp;
		j["weapon_item_state"] = weapon_item_state;
		j["weapon_case_state"] = weapon_case_state;
		j["weapon_case_info"] = weapon_case_info;
		j["weapon_distance"] = weapon_distance;
		j["weapon_case_distance"] = weapon_case_distance;
		j["weapon_radar"] = weapon_radar;
		j["primary_object_esp"] = primary_object_esp;
		j["primary_item_state"] = primary_item_state;
		j["primary_distance"] = primary_distance;
		j["primary_radar"] = primary_radar;
		j["secondary_item_state"] = secondary_item_state;
		j["secondary_distance"] = secondary_distance;
		j["secondary_object_esp"] = secondary_object_esp;
		j["secondary_radar"] = secondary_radar;
		j["task_object_esp"] = task_object_esp;
		j["task_object_distance"] = task_object_distance;
		j["task_object_state"] = task_object_state;
		j["task_delivery"] = task_delivery;
		j["task_machine"] = task_machine;
		j["task_vent"] = task_vent;
		j["task_alim"] = task_alim;
		j["task_pizzushi"] = task_pizzushi;
		j["task_computers"] = task_computers;
		j["task_scanners"] = task_scanners;
		j["alarm_esp"] = alarm_esp;
		j["rez_esp"] = rez_esp;


		// Colors (individually serialize RGBA components for each color)
		j["employee_color"] = { {"r", employee_color.x}, {"g", employee_color.y}, {"b", employee_color.z}, {"a", employee_color.w} };
		j["ghost_employee_color"] = { {"r", ghost_employee_color.x}, {"g", ghost_employee_color.y}, {"b", ghost_employee_color.z}, {"a", ghost_employee_color.w} };
		j["dissident_color"] = { {"r", dissident_color.x}, {"g", dissident_color.y}, {"b", dissident_color.z}, {"a", dissident_color.w} };
		j["ghost_dissident_color"] = { {"r", ghost_dissident_color.x}, {"g", ghost_dissident_color.y}, {"b", ghost_dissident_color.z}, {"a", ghost_dissident_color.w} };
		j["task_delivery_color"] = { {"r", task_delivery_color.x}, {"g", task_delivery_color.y}, {"b", task_delivery_color.z}, {"a", task_delivery_color.w} };
		j["task_machine_color"] = { {"r", task_machine_color.x}, {"g", task_machine_color.y}, {"b", task_machine_color.z}, {"a", task_machine_color.w} };
		j["task_vent_color"] = { {"r", task_vent_color.x}, {"g", task_vent_color.y}, {"b", task_vent_color.z}, {"a", task_vent_color.w} };
		j["task_alim_color"] = { {"r", task_alim_color.x}, {"g", task_alim_color.y}, {"b", task_alim_color.z}, {"a", task_alim_color.w} };
		j["task_pizzushi_color"] = { {"r", task_pizzushi_color.x}, {"g", task_pizzushi_color.y}, {"b", task_pizzushi_color.z}, {"a", task_pizzushi_color.w} };
		j["task_computer_color"] = { {"r", task_computer_color.x}, {"g", task_computer_color.y}, {"b", task_computer_color.z}, {"a", task_computer_color.w} };
		j["task_scanner_color"] = { {"r", task_scanner_color.x}, {"g", task_scanner_color.y}, {"b", task_scanner_color.z}, {"a", task_scanner_color.w} };
		j["weapon_color"] = { {"r", weapon_color.x}, {"g", weapon_color.y}, {"b", weapon_color.z}, {"a", weapon_color.w} };
		j["weapon_case_color"] = { {"r", weapon_case_color.x}, {"g", weapon_case_color.y}, {"b", weapon_case_color.z}, {"a", weapon_case_color.w} };
		j["fuse_color"] = { {"r", fuse_color.x}, {"g", fuse_color.y}, {"b", fuse_color.z}, {"a", fuse_color.w} };
		j["battery_color"] = { {"r", battery_color.x}, {"g", battery_color.y}, {"b", battery_color.z}, {"a", battery_color.w} };
		j["screw_driver_color"] = { {"r", screw_driver_color.x}, {"g", screw_driver_color.y}, {"b", screw_driver_color.z}, {"a", screw_driver_color.w} };
		j["egg_color"] = { {"r", egg_color.x}, {"g", egg_color.y}, {"b", egg_color.z}, {"a", egg_color.w} };
		j["container_color"] = { {"r", container_color.x}, {"g", container_color.y}, {"b", container_color.z}, {"a", container_color.w} };
		j["gaz_bottle_color"] = { {"r", gaz_bottle_color.x}, {"g", gaz_bottle_color.y}, {"b", gaz_bottle_color.z}, {"a", gaz_bottle_color.w} };
		j["machine_part_color"] = { {"r", machine_part_color.x}, {"g", machine_part_color.y}, {"b", machine_part_color.z}, {"a", machine_part_color.w} };
		j["vent_filter_color"] = { {"r", vent_filter_color.x}, {"g", vent_filter_color.y}, {"b", vent_filter_color.z}, {"a", vent_filter_color.w} };
		j["rice_color"] = { {"r", rice_color.x}, {"g", rice_color.y}, {"b", rice_color.z}, {"a", rice_color.w} };
		j["package_color"] = { {"r", package_color.x}, {"g", package_color.y}, {"b", package_color.z}, {"a", package_color.w} };
		j["sample_color"] = { {"r", sample_color.x}, {"g", sample_color.y}, {"b", sample_color.z}, {"a", sample_color.w} };
		j["access_card_color"] = { {"r", access_card_color.x}, {"g", access_card_color.y}, {"b", access_card_color.z}, {"a", access_card_color.w} };
		j["fov_color"] = { {"r", fov_color.x}, {"g", fov_color.y}, {"b", fov_color.z}, {"a", fov_color.w} };
		j["alarm_color"] = { {"r", fov_color.x}, {"g", fov_color.y}, {"b", fov_color.z}, {"a", fov_color.w} };
		j["rez_color"] = { {"r", rez_color.x}, {"g", rez_color.y}, {"b", rez_color.z}, {"a", rez_color.w} };
		j["defib_color"] = { {"r", defib_color.x}, {"g", defib_color.y}, {"b", defib_color.z}, {"a", defib_color.w} };

		std::ofstream file(filePath);  // This will create the file if it doesn't exist
		if (file.is_open()) {
			file << j.dump(4);  // Pretty print with 4 spaces
			file.close();
			std::cerr << "Config saved at " << filePath << std::endl;
		}
		else {
			std::cerr << "Error: Unable to open or create config file at " << filePath << std::endl;
		}
	}

	// Function to load config values from a JSON file
	inline void LoadConfig(const std::string& filename = "config.json") {
		std::string directory = "config";
		std::string filePath = directory + "/" + filename;

		try {
			// Check if the file exists
			if (!std::filesystem::exists(filePath)) {
				std::cerr << "Config file not found. Creating a new one with defaults." << std::endl;
				SaveConfig();
				return;
			}

			// Open and parse the config file
			std::ifstream file(filePath);
			if (!file.is_open()) {
				throw std::ios_base::failure("Failed to open config file.");
			}

			nlohmann::json j;
			file >> j;
			file.close();

			// Load hotkey settings
			j.at("menu_hotkey").get_to(menu_hotkey);
			j.at("esp_hotkey").get_to(esp_hotkey);
			j.at("speedhack_hotkey").get_to(speedhack_hotkey);
			j.at("god_mode_hotkey").get_to(god_mode_hotkey);
			j.at("infinite_stamina_hotkey").get_to(infinite_stamina_hotkey);
			j.at("fast_melee_hotkey").get_to(fast_melee_hotkey);
			j.at("infinite_melee_range_hotkey").get_to(infinite_melee_range_hotkey);
			j.at("auto_fire_hotkey").get_to(auto_fire_hotkey);
			j.at("rapid_fire_hotkey").get_to(rapid_fire_hotkey);
			j.at("no_recoil_hotkey").get_to(no_recoil_hotkey);
			j.at("max_damage_hotkey").get_to(max_damage_hotkey);
			j.at("aimbot_hotkey").get_to(aimbot_hotkey);
			j.at("aimbot_hold_key").get_to(aimbot_hold_key);
			j.at("player_list_hotkey").get_to(player_list_hotkey);

			// Load boolean settings
			j.at("speedhack").get_to(speedhack);
			j.at("max_speed").get_to(max_speed);
			j.at("friction").get_to(friction);
			j.at("infinite_stamina").get_to(infinite_stamina);
			j.at("god_mode").get_to(god_mode);
			j.at("fast_hp_recovery").get_to(fast_hp_recovery);
			j.at("fast_stam_recovery").get_to(fast_stam_recovery);
			j.at("hp_recovery_rate").get_to(hp_recovery_rate);
			j.at("stam_recovery_rate").get_to(stam_recovery_rate);
			j.at("player_fov").get_to(player_fov);
			j.at("fast_melee").get_to(fast_melee);
			j.at("cast_time").get_to(cast_time);
			j.at("recover_time").get_to(recover_time);
			j.at("stun").get_to(stun);
			j.at("cost").get_to(cost);
			j.at("infinite_melee_range").get_to(infinite_melee_range);
			j.at("range").get_to(range);
			j.at("max_damage").get_to(max_damage);
			j.at("auto_fire").get_to(auto_fire);
			j.at("rapid_fire").get_to(rapid_fire);
			j.at("rapid_fire_rate").get_to(rapid_fire_rate);
			j.at("no_recoil").get_to(no_recoil);
			j.at("movement_osc").get_to(movement_osc);
			j.at("osc_reactivity").get_to(osc_reactivity);
			j.at("infinite_ammo").get_to(infinite_ammo);
			j.at("ammo_count").get_to(ammo_count);
			j.at("can_inventory").get_to(can_inventory);
			j.at("aimbot").get_to(aimbot);
			j.at("target_filter").get_to(target_filter);
			j.at("target_closest").get_to(target_closest);
			j.at("smooth_factor").get_to(smooth_factor);
			j.at("aim_target").get_to(aim_target);
			j.at("aimbot_fov").get_to(aimbot_fov);
			j.at("anti_weapon_drop").get_to(anti_weapon_drop);
			j.at("drop_threshold").get_to(drop_threshold);

			// Load ESP-related settings
			j.at("esp_enabled").get_to(esp_enabled);
			j.at("esp_max_distance").get_to(esp_max_distance);
			j.at("esp_radar").get_to(esp_radar);
			j.at("esp_radar_position").get_to(esp_radar_position);
			j.at("esp_radar_scale").get_to(esp_radar_scale);
			j.at("player_esp").get_to(player_esp);
			j.at("player_distance").get_to(player_distance);
			j.at("player_inventory").get_to(player_inventory);
			j.at("player_box").get_to(player_box);
			j.at("player_ghost").get_to(player_ghost);
			j.at("player_radar").get_to(player_radar);
			j.at("ghost_radar").get_to(ghost_radar);
			j.at("player_list").get_to(player_list);
			j.at("player_list_locked").get_to(player_list_locked);
			j.at("player_list_x").get_to(player_list_x);
			j.at("player_list_y").get_to(player_list_y);
			j.at("list_inv").get_to(list_inv);
			j.at("weapon_esp").get_to(weapon_esp);
			j.at("weapon_case_esp").get_to(weapon_case_esp);
			j.at("weapon_item_state").get_to(weapon_item_state);
			j.at("weapon_case_state").get_to(weapon_case_state);
			j.at("weapon_case_info").get_to(weapon_case_info);
			j.at("weapon_distance").get_to(weapon_distance);
			j.at("weapon_case_distance").get_to(weapon_case_distance);
			j.at("weapon_radar").get_to(weapon_radar);
			j.at("primary_object_esp").get_to(primary_object_esp);
			j.at("primary_item_state").get_to(primary_item_state);
			j.at("primary_distance").get_to(primary_distance);
			j.at("primary_radar").get_to(primary_radar);
			j.at("secondary_item_state").get_to(secondary_item_state);
			j.at("secondary_distance").get_to(secondary_distance);
			j.at("secondary_object_esp").get_to(secondary_object_esp);
			j.at("secondary_radar").get_to(secondary_radar);
			j.at("task_object_esp").get_to(task_object_esp);
			j.at("task_object_distance").get_to(task_object_distance);
			j.at("task_object_state").get_to(task_object_state);
			j.at("task_delivery").get_to(task_delivery);
			j.at("task_machine").get_to(task_machine);
			j.at("task_vent").get_to(task_vent);
			j.at("task_alim").get_to(task_alim);
			j.at("task_pizzushi").get_to(task_pizzushi);
			j.at("task_computers").get_to(task_computers);
			j.at("task_scanners").get_to(task_scanners);
			j.at("alarm_esp").get_to(alarm_esp);
			j.at("rez_esp").get_to(rez_esp);

			// Colors (individually unpack RGBA components for each color if present)
			if (j.contains("employee_color")) {
				employee_color.x = j["employee_color"].at("r").get<float>();
				employee_color.y = j["employee_color"].at("g").get<float>();
				employee_color.z = j["employee_color"].at("b").get<float>();
				employee_color.w = j["employee_color"].at("a").get<float>();
			}

			if (j.contains("dissident_color")) {
				dissident_color.x = j["dissident_color"].at("r").get<float>();
				dissident_color.y = j["dissident_color"].at("g").get<float>();
				dissident_color.z = j["dissident_color"].at("b").get<float>();
				dissident_color.w = j["dissident_color"].at("a").get<float>();
			}
			if (j.contains("ghost_employee_color")) {
				ghost_employee_color.x = j["ghost_employee_color"].at("r").get<float>();
				ghost_employee_color.y = j["ghost_employee_color"].at("g").get<float>();
				ghost_employee_color.z = j["ghost_employee_color"].at("b").get<float>();
				ghost_employee_color.w = j["ghost_employee_color"].at("a").get<float>();
			}

			if (j.contains("ghost_dissident_color")) {
				ghost_dissident_color.x = j["ghost_dissident_color"].at("r").get<float>();
				ghost_dissident_color.y = j["ghost_dissident_color"].at("g").get<float>();
				ghost_dissident_color.z = j["ghost_dissident_color"].at("b").get<float>();
				ghost_dissident_color.w = j["ghost_dissident_color"].at("a").get<float>();
			}

			if (j.contains("task_delivery_color")) {
				task_delivery_color.x = j["task_delivery_color"].at("r").get<float>();
				task_delivery_color.y = j["task_delivery_color"].at("g").get<float>();
				task_delivery_color.z = j["task_delivery_color"].at("b").get<float>();
				task_delivery_color.w = j["task_delivery_color"].at("a").get<float>();
			}

			if (j.contains("task_machine_color")) {
				task_machine_color.x = j["task_machine_color"].at("r").get<float>();
				task_machine_color.y = j["task_machine_color"].at("g").get<float>();
				task_machine_color.z = j["task_machine_color"].at("b").get<float>();
				task_machine_color.w = j["task_machine_color"].at("a").get<float>();
			}

			if (j.contains("task_vent_color")) {
				task_vent_color.x = j["task_vent_color"].at("r").get<float>();
				task_vent_color.y = j["task_vent_color"].at("g").get<float>();
				task_vent_color.z = j["task_vent_color"].at("b").get<float>();
				task_vent_color.w = j["task_vent_color"].at("a").get<float>();
			}

			if (j.contains("task_alim_color")) {
				task_alim_color.x = j["task_alim_color"].at("r").get<float>();
				task_alim_color.y = j["task_alim_color"].at("g").get<float>();
				task_alim_color.z = j["task_alim_color"].at("b").get<float>();
				task_alim_color.w = j["task_alim_color"].at("a").get<float>();
			}

			if (j.contains("task_pizzushi_color")) {
				task_pizzushi_color.x = j["task_pizzushi_color"].at("r").get<float>();
				task_pizzushi_color.y = j["task_pizzushi_color"].at("g").get<float>();
				task_pizzushi_color.z = j["task_pizzushi_color"].at("b").get<float>();
				task_pizzushi_color.w = j["task_pizzushi_color"].at("a").get<float>();
			}

			if (j.contains("task_computer_color")) {
				task_computer_color.x = j["task_computer_color"].at("r").get<float>();
				task_computer_color.y = j["task_computer_color"].at("g").get<float>();
				task_computer_color.z = j["task_computer_color"].at("b").get<float>();
				task_computer_color.w = j["task_computer_color"].at("a").get<float>();
			}

			if (j.contains("task_scanner_color")) {
				task_scanner_color.x = j["task_scanner_color"].at("r").get<float>();
				task_scanner_color.y = j["task_scanner_color"].at("g").get<float>();
				task_scanner_color.z = j["task_scanner_color"].at("b").get<float>();
				task_scanner_color.w = j["task_scanner_color"].at("a").get<float>();
			}

			if (j.contains("weapon_color")) {
				weapon_color.x = j["weapon_color"].at("r").get<float>();
				weapon_color.y = j["weapon_color"].at("g").get<float>();
				weapon_color.z = j["weapon_color"].at("b").get<float>();
				weapon_color.w = j["weapon_color"].at("a").get<float>();
			}


			if (j.contains("weapon_case_color")) {
				weapon_case_color.x = j["weapon_case_color"].at("r").get<float>();
				weapon_case_color.y = j["weapon_case_color"].at("g").get<float>();
				weapon_case_color.z = j["weapon_case_color"].at("b").get<float>();
				weapon_case_color.w = j["weapon_case_color"].at("a").get<float>();
			}

			if (j.contains("alarm_color")) {
				alarm_color.x = j["alarm_color"].at("r").get<float>();
				alarm_color.y = j["alarm_color"].at("g").get<float>();
				alarm_color.z = j["alarm_color"].at("b").get<float>();
				alarm_color.w = j["alarm_color"].at("a").get<float>();
			}

			if (j.contains("rez_color")) {
				rez_color.x = j["rez_color"].at("r").get<float>();
				rez_color.y = j["rez_color"].at("g").get<float>();
				rez_color.z = j["rez_color"].at("b").get<float>();
				rez_color.w = j["rez_color"].at("a").get<float>();
			}

			if (j.contains("defib_color")) {
				defib_color.x = j["defib_color"].at("r").get<float>();
				defib_color.y = j["defib_color"].at("g").get<float>();
				defib_color.z = j["defib_color"].at("b").get<float>();
				defib_color.w = j["defib_color"].at("a").get<float>();
			}

			if (j.contains("fuse_color")) {
				fuse_color.x = j["fuse_color"].at("r").get<float>();
				fuse_color.y = j["fuse_color"].at("g").get<float>();
				fuse_color.z = j["fuse_color"].at("b").get<float>();
				fuse_color.w = j["fuse_color"].at("a").get<float>();
			}

			if (j.contains("battery_color")) {
				battery_color.x = j["battery_color"].at("r").get<float>();
				battery_color.y = j["battery_color"].at("g").get<float>();
				battery_color.z = j["battery_color"].at("b").get<float>();
				battery_color.w = j["battery_color"].at("a").get<float>();
			}

			if (j.contains("screw_driver_color")) {
				screw_driver_color.x = j["screw_driver_color"].at("r").get<float>();
				screw_driver_color.y = j["screw_driver_color"].at("g").get<float>();
				screw_driver_color.z = j["screw_driver_color"].at("b").get<float>();
				screw_driver_color.w = j["screw_driver_color"].at("a").get<float>();
			}

			if (j.contains("egg_color")) {
				egg_color.x = j["egg_color"].at("r").get<float>();
				egg_color.y = j["egg_color"].at("g").get<float>();
				egg_color.z = j["egg_color"].at("b").get<float>();
				egg_color.w = j["egg_color"].at("a").get<float>();
			}

			if (j.contains("container_color")) {
				container_color.x = j["container_color"].at("r").get<float>();
				container_color.y = j["container_color"].at("g").get<float>();
				container_color.z = j["container_color"].at("b").get<float>();
				container_color.w = j["container_color"].at("a").get<float>();
			}

			if (j.contains("gaz_bottle_color")) {
				gaz_bottle_color.x = j["gaz_bottle_color"].at("r").get<float>();
				gaz_bottle_color.y = j["gaz_bottle_color"].at("g").get<float>();
				gaz_bottle_color.z = j["gaz_bottle_color"].at("b").get<float>();
				gaz_bottle_color.w = j["gaz_bottle_color"].at("a").get<float>();
			}

			if (j.contains("machine_part_color")) {
				machine_part_color.x = j["machine_part_color"].at("r").get<float>();
				machine_part_color.y = j["machine_part_color"].at("g").get<float>();
				machine_part_color.z = j["machine_part_color"].at("b").get<float>();
				machine_part_color.w = j["machine_part_color"].at("a").get<float>();
			}

			if (j.contains("access_card_color")) {
				access_card_color.x = j["access_card_color"].at("r").get<float>();
				access_card_color.y = j["access_card_color"].at("g").get<float>();
				access_card_color.z = j["access_card_color"].at("b").get<float>();
				access_card_color.w = j["access_card_color"].at("a").get<float>();
			}

			if (j.contains("vent_filter_color")) {
				vent_filter_color.x = j["vent_filter_color"].at("r").get<float>();
				vent_filter_color.y = j["vent_filter_color"].at("g").get<float>();
				vent_filter_color.z = j["vent_filter_color"].at("b").get<float>();
				vent_filter_color.w = j["vent_filter_color"].at("a").get<float>();
			}

			if (j.contains("rice_color")) {
				rice_color.x = j["rice_color"].at("r").get<float>();
				rice_color.y = j["rice_color"].at("g").get<float>();
				rice_color.z = j["rice_color"].at("b").get<float>();
				rice_color.w = j["rice_color"].at("a").get<float>();
			}

			if (j.contains("package_color")) {
				package_color.x = j["package_color"].at("r").get<float>();
				package_color.y = j["package_color"].at("g").get<float>();
				package_color.z = j["package_color"].at("b").get<float>();
				package_color.w = j["package_color"].at("a").get<float>();
			}

			if (j.contains("sample_color")) {
				sample_color.x = j["sample_color"].at("r").get<float>();
				sample_color.y = j["sample_color"].at("g").get<float>();
				sample_color.z = j["sample_color"].at("b").get<float>();
				sample_color.w = j["sample_color"].at("a").get<float>();
			}

			if (j.contains("fov_color")) {
				fov_color.x = j["fov_color"].at("r").get<float>();
				fov_color.y = j["fov_color"].at("g").get<float>();
				fov_color.z = j["fov_color"].at("b").get<float>();
				fov_color.w = j["fov_color"].at("a").get<float>();
			}

			std::cerr << "Config loaded." << std::endl;
		}
		catch (const std::exception& e) {
			std::cerr << "Error loading config file: " << e.what() << std::endl;
			std::cerr << "Recreating config file with defaults." << std::endl;

			// Delete the invalid config file
			std::filesystem::remove(filePath);

			// Save a new config file with default settings
			SaveConfig();
		}
	}
}

namespace flyhack {
	inline bool enabled = false;
	inline bool noFall = true;
	inline bool noPush = true;
	inline bool noClip = false;
	inline bool directional = true;

	inline int hotkey = 'F';    
	inline int forwardKey = 'W';
	inline int backwardKey = 'S';
	inline int leftKey = 'A';
	inline int rightKey = 'D';
	inline int upKey = VK_SPACE;
	inline int downKey = VK_CONTROL;

	inline double hSpeed = 25.0;
	inline double vSpeed = 15.0;
}