#include <iostream>
#include <thread>
#include <string>
#include "overlay/overlay.h"
#include "mem.hpp"
#include "game_structures.hpp"
#include "util.hpp"
#include "config.h"
#include "menu.h"
#include "radar.h"
#include "globals.h"
#include "ItemProperties.h"
#include <cmath>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include "data_cache.h"
#include <algorithm>
#include <limits>
#include "game_math.hpp"
#include "game_function.hpp"
#include <unordered_set>
#include <algorithm>
#include <set>

using namespace menu;
using namespace radar;
using namespace config;
using namespace globals;
using namespace protocol::engine::sdk;
using namespace protocol::game::sdk;
using namespace protocol::engine;

std::unordered_map<std::string, ItemProperties> itemData;

static int current_target_index = 0;

void InitializeItems() {
	std::cout << "Initializing item data..." << std::endl;
	itemData["KNIFE"] = ItemProperties(0.1, 0.4, 0.4, 130, 20);
	itemData["PACKAGE"] = ItemProperties(0.3, 1, -1, 180, 40);
	itemData["GAZ BOTTLE"] = ItemProperties(0.3, 1, -1, 180, 40);
	itemData["FUSE"] = ItemProperties(0.2, 0.7, -0.8, 130, 20);
	itemData["SCREW DRIVER"] = ItemProperties(0.1, 0.4, 0.1, 130, 15);
	itemData["CASSETTE"] = ItemProperties(0.2, 0.7, -0.8, 130, 20);
	itemData["BATTERY"] = ItemProperties(0.2, 0.7, -0.8, 130, 20);
	itemData["VENT FILTER"] = ItemProperties(0.3, 1, -1, 180, 40);
	itemData["FISH"] = ItemProperties(0.2, 0.7, 1, 130, 20);
	itemData["RICE"] = ItemProperties(0.3, 1, -1, 180, 40);
	itemData["CONTAINER"] = ItemProperties(0.2, 0.7, -0.8, 130, 20);
	itemData["C4"] = ItemProperties(0.2, 0.7, -0.8, 130, 20);
	itemData["NAME"] = ItemProperties(0.3, 1, -1, 180, 40); // Pizzushi
	itemData["SAMPLE"] = ItemProperties(0.2, 0.7, -0.8, 130, 20);
	itemData["PISTOL"] = ItemProperties(false, 0.1, 5, 1, 30, 0.8, 1.5, 0.3, 40, 0.5, 4, 1);
	itemData["SHORTY"] = ItemProperties(false, 0.3, 5, 5, 10, 1, 2.5, 0.3, 20, 30, 0.6, 40);
	itemData["REVOLVER"] = ItemProperties(false, 0.25, 25, 4, 15, 0.8, 2, 0.4, 20, 0.5, 6, 3);
	itemData["SHOTGUN"] = ItemProperties(true, 0.2, 5, 3, 100, 0.3, 0.8, 0.1, 20, 2, 1, 5);
	itemData["RIFLE"] = ItemProperties(true, 0.12, 25, 2, 10, 2, 2.5, 0.1, 10, 1, 7, 3);
	itemData["SMG"] = ItemProperties(true, 0.07, 10, 0.5, 50, 0.6, 1.2, 0.2, 50, 1, 4, 1.5);
}

ItemProperties GetItemProperties(const std::string& itemName) {
	if (itemData.find(itemName) != itemData.end()) {
		return itemData[itemName];
	}
}

auto calculate_text_width = [](const std::string& text) -> int {
	return text.length() * 7; // Adjust 7 to the average pixel width of font
	};

std::string CalculateDistance(const FVector& location1, const vector3& location2) {
	// Calculate the difference in coordinates
	double dx = location1.X - location2.x;
	double dy = location1.Y - location2.y;
	double dz = location1.Z - location2.z;

	// Calculate the distance and divide by 100.0 to get distance in meters (assuming 100 units per meter)
	double distance = std::sqrt(dx * dx + dy * dy + dz * dz) / 100.0;

	std::ostringstream stream;
	stream << std::fixed << std::setprecision(2) << distance;
	return stream.str();
}

double CalculateDistanceMeters(const vector3& location1, const vector3& location2) {
	double dx = location1.x - location2.x;
	double dy = location1.y - location2.y;
	double dz = location1.z - location2.z;

	return std::sqrt(dx * dx + dy * dy + dz * dz) / 100.0;
}

// Function to convert FVector to vector3
vector3 ConvertFVectorToVector3(const FVector& fvec) {
	return vector3{ static_cast<float>(fvec.X), static_cast<float>(fvec.Y), static_cast<float>(fvec.Z) };
}

// Example CalculateDistance function (returns a float)
float CalculateDistanceFloat(const vector3& player_pos, const vector3& enemy_pos) {
	return sqrtf(
		powf(enemy_pos.x - player_pos.x, 2.0f) +
		powf(enemy_pos.y - player_pos.y, 2.0f) +
		powf(enemy_pos.z - player_pos.z, 2.0f)
	);
}

std::string TranslateRoomName(int roomNum) {
	switch (roomNum) {
	case 111: return "Office 1";
	case 112: return "Office 2";
	case 123: return "Office 3";
	case 124: return "Office 4";
	case 125: return "Office 5";
	case 211: return "Botanic";
	case 141: return "Restaurant";
	case 311: return "Medical";
	case 511: return "Machine";
	case 131: return "Security";
	case 411: return "Storage";
	default: return "Unknown";
	}
}

float ExtractMeaningfulValue(double largeValue) {
	std::ostringstream oss;
	oss << std::scientific << largeValue; // Convert to string in scientific notation
	std::string valueAsString = oss.str();

	size_t eIndex = valueAsString.find("e-");
	if (eIndex != std::string::npos) {
		// Extract the exponent part after "e-"
		std::string exponentPart = valueAsString.substr(eIndex + 2); // Skip "e-"
		int exponent = std::stoi(exponentPart); // Convert to an integer

		// Compute the meaningful value (e.g., exponent % 360 for rotation logic)
		return static_cast<float>(exponent % 360);
	}
	return largeValue; // If no "e-" is found, return the original value
}

// Function to calculate angular distance
float CalculateAngularDistance(float target_pitch, float target_yaw, float camera_pitch, float camera_yaw) {
	// Calculate pitch difference and normalize it
	float pitch_diff = target_pitch - camera_pitch;
	if (pitch_diff > 180.0f) pitch_diff -= 360.0f;
	if (pitch_diff < -180.0f) pitch_diff += 360.0f;

	// Calculate yaw difference and normalize it
	float yaw_diff = target_yaw - camera_yaw;
	if (yaw_diff > 180.0f) yaw_diff -= 360.0f;
	if (yaw_diff < -180.0f) yaw_diff += 360.0f;

	// Return the combined angular difference (simple Euclidean distance)
	return sqrtf(pitch_diff * pitch_diff + yaw_diff * yaw_diff);
}

// Function to check if the target has been focused for the required time
bool IsTargetFocused(float focus_time, float required_time) {
	return focus_time >= required_time;
}

static void cache_useful() {
	bool items_populated = false;  // Flag to track if items have been populated once

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		gworld = u_world::get_world(mem::module_base);
		if (!gworld) continue;
		game_state = gworld->get_game_state();
		if (!game_state) continue;
		owning_instance = gworld->get_owning_game_instance();
		if (!owning_instance) continue;
		local_player = owning_instance->get_localplayer();
		if (!local_player) continue;
		local_controller = local_player->get_player_controller();
		if (!local_controller) continue;
		local_camera_manager = local_controller->get_camera_manager();
		if (!local_camera_manager) continue;
		local_mec = (mec_pawn*)local_controller->get_pawn();
		if (!local_mec) continue;

		std::vector < mec_pawn* > temp_player_cache{};
		std::vector < world_item* > temp_world_item_cache{};
		std::vector < task_vents* > temp_task_vents_cache{};
		std::vector < task_machines* > temp_task_machines_cache{};
		std::vector < task_alimentations* > temp_task_alims_cache{};
		std::vector < task_deliveries* > temp_task_delivery_cache{};
		std::vector < task_pizzushis* > temp_task_pizzushi_cache{};
		std::vector < task_data* > temp_task_data_cache{};
		std::vector < task_scanner* > temp_task_scanner_cache{};
		std::vector < a_weapon_case_code_c* > temp_weapon_case_cache{};

		f_camera_cache last_frame_cached;
		int current_target_index = 0;
		float expanding_radius = 0.0f;
		std::vector<FStr_ScannerDot> scanner_targets;

		auto levels = gworld->get_levels();
		for (auto level : levels.list()) {
			auto actors = level->get_actors();

			for (auto actor : actors.list()) {
				auto class_name = util::get_name_from_fname(actor->class_private()->fname_index());
				auto name = util::get_name_from_fname(actor->outer()->fname_index());

				if (class_name.find("WorldItem_C") != std::string::npos) {
					auto item = static_cast<world_item*>(actor);
					auto item_data = item->get_data();
					auto item_name = item_data->get_name().read_string();


					temp_world_item_cache.push_back((world_item*)actor);
				}
				if (class_name.find("Task_Vents_C") != std::string::npos) {
					temp_task_vents_cache.push_back((task_vents*)actor);
				}
				if (class_name.find("Task_Machine_C") != std::string::npos) {
					temp_task_machines_cache.push_back((task_machines*)actor);
				}
				if (class_name.find("Task_Alim_C") != std::string::npos) {
					temp_task_alims_cache.push_back((task_alimentations*)actor);
				}
				if (class_name.find("Task_DelivryIn_C") != std::string::npos) {
					temp_task_delivery_cache.push_back((task_deliveries*)actor);
				}
				if (class_name.find("Task_Pizzushi_C") != std::string::npos) {
					temp_task_pizzushi_cache.push_back((task_pizzushis*)actor);
				}
				if (class_name.find("Task_Data_C") != std::string::npos) {
					temp_task_data_cache.push_back((task_data*)actor);
				}
				if (class_name.find("Task_Scanner_C") != std::string::npos) {
					temp_task_scanner_cache.push_back((task_scanner*)actor);
				}
				if (class_name.find("Mec_C") != std::string::npos) {
					temp_player_cache.push_back((mec_pawn*)actor);
				}
				if (class_name.find("WeaponCaseCode_C") != std::string::npos) {
					temp_weapon_case_cache.push_back((a_weapon_case_code_c*)actor);
				}
			}
		}

		player_cache = temp_player_cache;
		world_item_cache = temp_world_item_cache;
		task_vents_cache = temp_task_vents_cache;
		task_machines_cache = temp_task_machines_cache;
		task_alims_cache = temp_task_alims_cache;
		task_delivery_cache = temp_task_delivery_cache;
		task_pizzushi_cache = temp_task_pizzushi_cache;
		task_data_cache = temp_task_data_cache;
		task_scanner_cache = temp_task_scanner_cache;
		weapon_case_cache = temp_weapon_case_cache;

		// Call PopulateUniqueItems only once after items are populated
		if (!items_populated && !temp_world_item_cache.empty()) {
			PopulateUniqueItems();
			items_populated = true;  // Ensure this only happens once
		}
	}
}

static void render_callback() {
	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	menu::draw();
	radar::draw();

	f_camera_cache last_frame_cached = local_camera_manager->get_cached_frame_private();
	auto players = game_state->player_array();

	if (GetAsyncKeyState(esp_hotkey) & 1) {
		esp_enabled = !esp_enabled;
		player_esp = esp_enabled;
		weapon_esp = esp_enabled;
		task_object_esp = esp_enabled;
		primary_object_esp = esp_enabled;
		secondary_object_esp = esp_enabled;
		esp_radar = esp_enabled;
		weapon_case_esp = esp_enabled;
		//std::cout << "Fire Spread:" << local_mec->get_fire_spread() << std::endl;
	}
	if (GetAsyncKeyState(speedhack_hotkey) & 1) {
		speedhack = !speedhack;
	}
	if (GetAsyncKeyState(god_mode_hotkey) & 1) {
		god_mode = !god_mode;
	}
	if (GetAsyncKeyState(infinite_stamina_hotkey) & 1) {
		infinite_stamina = !infinite_stamina;
	}
	if (GetAsyncKeyState(fast_melee_hotkey) & 1) {
		fast_melee = !fast_melee;
	}
	if (GetAsyncKeyState(infinite_melee_range_hotkey) & 1) {
		infinite_melee_range = !infinite_melee_range;
	}
	if (GetAsyncKeyState(auto_fire_hotkey) & 1) {
		auto_fire = !auto_fire;
	}
	if (GetAsyncKeyState(rapid_fire_hotkey) & 1) {
		rapid_fire = !rapid_fire;
	}

	if (GetAsyncKeyState(aimbot_hotkey) & 1) {
		aimbot = !aimbot;
	}


	auto hand_item = local_mec->get_hand_item();
	auto melee_item_data = (u_data_melee*)hand_item;

	//if (hand_item) {
	//    auto mtype = melee_item_data->get_melee_type();
	//    // Retrieve the properties
	//    double castTime = mtype->get_cast_time();
	//    double recoverTime = mtype->get_recover_time();
	//    double stun = mtype->get_stun();
	//    int cost = mtype->get_cost();
	//    int range = mtype->get_range();

	//    // Format and output the string to the console
	//    std::cout << "itemData[\"" << hand_item->get_name().read_string() << "\"] = "
	//    << "ItemProperties(" << castTime << ", "
	//    << recoverTime << ", "
	//    << stun << ", "
	//    << range << ", "
	//    << cost << ");" << std::endl;
	//}

	if (aimbot) {
		// Draw FOV circle
		int screen_width = GetSystemMetrics(SM_CXSCREEN);
		int screen_height = GetSystemMetrics(SM_CYSCREEN);
		ImVec2 screen_center = ImVec2(screen_width / 2.0f, screen_height / 2.0f);

		// Convert ImVec4 to IM_COL32 for the circle
		ImU32 circle_color = ImGui::ColorConvertFloat4ToU32(fov_color);

		overlay->draw_circle(screen_center, aimbot_fov, circle_color, 64, 1.5f);
	}
	if (aimbot && GetAsyncKeyState(aimbot_hold_key) & 0x8000) {
		// Retrieve cached frame for camera details
		f_camera_cache last_frame_cached = local_camera_manager->get_cached_frame_private();
		vector3 camera_location = last_frame_cached.pov.location; // Camera position
		vector3 camera_rotation = last_frame_cached.pov.rotation; // Camera rotation

		// Retrieve local player position
		auto player_position_fvector = local_mec->get_net_location();
		auto player_position = ConvertFVectorToVector3(player_position_fvector);

		mec_pawn* best_target = nullptr;
		float best_value = 999999.0f; // Closest distance or smallest screen offset
		vector3 best_target_screen_position;

		// Screen dimensions and center
		int screen_width = GetSystemMetrics(SM_CXSCREEN);
		int screen_height = GetSystemMetrics(SM_CYSCREEN);
		vector2 screen_center = { screen_width / 2.0f, screen_height / 2.0f };

		// Iterate through all cached players to find the best target
		for (auto mec : player_cache) {
			auto state = mec->player_state();
			if (!state) continue;

			auto role = mec->get_player_role();

			// Apply target filter
			if (target_filter == "Employees" && role != 3) continue;
			if (target_filter == "Dissidents" && role != 4) continue;

			if (mec != local_mec) {
				// Check if the player is dead
				auto ghost_root = mec->get_ghost_root();
				if (!ghost_root) continue;

				auto ghost_position = ghost_root->get_relative_location();
				auto current_position = mec->get_root_component()->get_relative_location();

				double ghost_distance = CalculateDistanceMeters(current_position, ghost_position);
				if (ghost_distance > 2) continue; // Skip dead bodies

				// Adjust for selected aim target
				float aim_height_offset = (aim_target == "Neck") ? 145.0f : (aim_target == "Body") ? 125.0f : 165.0f;
				vector3 target_position = current_position + vector3{ 0, 0, aim_height_offset };

				vector3 screen_position{};
				bool visible = util::w2s(target_position, last_frame_cached.pov, screen_position);

				if (visible) {
					// Calculate distances
					float distance_to_player = CalculateDistanceFloat(player_position, target_position);
					float screen_distance = sqrtf(powf(screen_position.x - screen_center.x, 2) +
						powf(screen_position.y - screen_center.y, 2));

					// Skip targets outside FOV
					if (screen_distance > aimbot_fov) continue;

					if (target_closest) {
						// Prioritize closest to the player
						if (distance_to_player < best_value) {
							best_value = distance_to_player;
							best_target = mec;
							best_target_screen_position = screen_position;
						}
					}
					else {
						// Prioritize closest to the center of the screen
						if (screen_distance < best_value) {
							best_value = screen_distance;
							best_target = mec;
							best_target_screen_position = screen_position;
						}
					}
				}
			}
		}

		// Smoothly move the cursor to the best target
		if (best_target) {
			int target_x = static_cast<int>(best_target_screen_position.x);
			int target_y = static_cast<int>(best_target_screen_position.y);

			int center_x = static_cast<int>(screen_center.x);
			int center_y = static_cast<int>(screen_center.y);

			// Calculate relative movement offsets
			float delta_x = static_cast<float>(target_x - center_x);
			float delta_y = static_cast<float>(target_y - center_y);

			// Apply smoothing
			delta_x *= smooth_factor;
			delta_y *= smooth_factor;

			// Simulate mouse movement using SendInput
			INPUT input = { 0 };
			input.type = INPUT_MOUSE;
			input.mi.dx = static_cast<LONG>(delta_x);
			input.mi.dy = static_cast<LONG>(delta_y);
			input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_MOVE_NOCOALESCE;

			// Send the input
			SendInput(1, &input, sizeof(INPUT));
		}
	}

	if (infinite_ammo) {
		if (hand_item) {
			auto item_name = hand_item->get_name().read_string();
			if (item_name == "SHORTY" || item_name == "PISTOL" || item_name == "REVOLVER" || item_name == "SHOTGUN" || item_name == "RIFLE" || item_name == "SMG") {
				auto hand_state = local_mec->get_hand_state();
				hand_state.Value_8 = ammo_count;
				local_mec->set_hand_state(hand_state);
			}
			else {
				std::cout << "Error: " << item_name << " is not a valid gun." << std::endl;
				infinite_ammo = !infinite_ammo;
			}
		}
	}

	if (infinite_stamina) {
		local_mec->set_stamina(1.);
	}

	if (anti_weapon_drop) {
		// Retrieve the current stamina
		double current_stamina = local_mec->get_stamina();

		// Convert user-friendly threshold to actual stamina value
		double adjusted_threshold = 100.0 - drop_threshold;

		// If stamina is greater than or equal to the adjusted threshold and not already at 1.0
		if (current_stamina >= adjusted_threshold && current_stamina != 1.0) {
			local_mec->set_stamina(1.0);
		}
	}

	if (god_mode) {
		local_mec->set_health(10000);
	}

	// Track the last tick time for incremental healing
	static auto last_hp_tick = std::chrono::steady_clock::now();
	static auto last_stam_tick = std::chrono::steady_clock::now();

	if (fast_hp_recovery || fast_stam_recovery) {
		// Get the current time
		auto current_time = std::chrono::steady_clock::now();

		// Check for HP recovery
		if (fast_hp_recovery) {
			// Calculate the time difference in milliseconds
			auto elapsed_hp_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_hp_tick).count();

			// Check if the recovery rate has passed
			if (elapsed_hp_time >= hp_recovery_rate) {
				// Get current health
				int current_health = local_mec->get_health();

				// Increment health if it's less than 100
				if (current_health < 100) {
					local_mec->set_health(current_health + 1); // Increase health by 1
				}

				// Update the last HP tick time
				last_hp_tick = current_time;
			}
		}

		// Check for Stamina recovery
		if (fast_stam_recovery) {
			// Calculate the time difference in milliseconds
			auto elapsed_stam_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_stam_tick).count();

			// Check if the recovery rate has passed
			if (elapsed_stam_time >= stam_recovery_rate) {
				// Get current stamina
				double current_stamina = local_mec->get_stamina();

				// Decrement stamina if it's greater than 1.0
				if (current_stamina > 1.0) {
					local_mec->set_stamina(current_stamina - 1.0); // Decrease stamina by 1.0
				}

				// Update the last Stamina tick time
				last_stam_tick = current_time;
			}
		}
	}

	if (player_fov != 103) {
		local_mec->get_camera()->set_field_of_view(player_fov);
	}

	//static double fric = local_mec->get_friction();
	//std::cout << *reinterpret_cast<std::uint64_t*>(&fric);
	if (speedhack) {
		local_mec->set_acceleration(vector2(9999.0, 9999.0));
		local_mec->set_max_speed(max_speed);
		local_mec->set_friction(friction);
	}
	else {
		local_mec->set_acceleration(vector2(100.0, 100.0));
		local_mec->set_max_speed(800.0);
		local_mec->set_friction(0);
	}

	if (lock_hand_item) {
		// Check if the hand item is empty
		if (!local_mec->get_hand_item()) {
			if (locked_hand_item) {
				// Restore the locked hand item and state
				local_mec->set_hand_item(locked_hand_item);
				local_mec->set_hand_state(locked_hand_state);
			}
		}
	}

	if (lock_bag_item) {
		// Check if the bag item is empty
		if (!local_mec->get_bag_item()) {
			if (locked_bag_item) {
				// Restore the locked bag item and state
				local_mec->set_bag_item(locked_bag_item);
				local_mec->set_bag_state(locked_bag_state);
			}
		}
	}

	if (hand_item) {
		if (fast_melee || infinite_melee_range) {
			if (util::get_name_from_fname(hand_item->class_private()->fname_index()).find("Data_Melee_C") != std::string::npos) {
				auto mtype = melee_item_data->get_melee_type();

				if (fast_melee) {
					mtype->set_cast_time(cast_time);
					mtype->set_recover_time(recover_time);
					mtype->set_stun(stun);
					mtype->set_cost(cost);
				}
				if (infinite_melee_range) {
					mtype->set_range(range);
				}
			}
		}
	}

	if (hand_item) {
		if (!fast_melee) {
			auto mtype = melee_item_data->get_melee_type();
			std::string item_name = hand_item->get_name().read_string();
			ItemProperties itemprops = GetItemProperties(item_name);

			if (!fast_melee) {
				mtype->set_cast_time(itemprops.melee_cast_time);
				mtype->set_recover_time(itemprops.melee_recover_time);
				mtype->set_stun(itemprops.melee_stun);
				mtype->set_cost(itemprops.melee_cost);
			}
		}

		if (!infinite_melee_range) {
			auto mtype = melee_item_data->get_melee_type();
			std::string item_name = hand_item->get_name().read_string();
			ItemProperties itemprops = GetItemProperties(item_name);

			if (!infinite_melee_range) {
				mtype->set_range(itemprops.melee_range);
			}
		}
	}

	if (hand_item) {
		if (auto_fire || rapid_fire || no_recoil || max_damage) {
			if (util::get_name_from_fname(hand_item->class_private()->fname_index()).find("Data_Gun_C") != std::string::npos) {
				auto gun_data = (u_data_gun*)hand_item;
				if (auto_fire) {
					gun_data->set_auto_fire(true);
				}
				if (rapid_fire) {
					gun_data->set_fire_rate(rapid_fire_rate);
				}
				if (no_recoil) {
					local_mec->set_fire_spread(fire_spread);

					gun_data->set_shake_intensity(shake_intensity);

					gun_data->set_oscillation_reactivity(osc_reactivity);
					gun_data->set_walk_oscillation(movement_osc);
					gun_data->set_run_oscillation(movement_osc);
					gun_data->set_stand_oscillation(movement_osc);

					gun_data->set_recoil_reactivity(recoil_react);
					gun_data->set_walk_precision(movement_prec);
					gun_data->set_air_precision(movement_prec);
					gun_data->set_run_precision(movement_prec);
				}
				if (max_damage) {
					gun_data->set_damage(10000);
				}
			}
		}
	}

	if (hand_item) {
		if (!auto_fire || !rapid_fire || !no_recoil || !max_damage) {
			if (util::get_name_from_fname(hand_item->class_private()->fname_index()).find("Data_Gun_C") != std::string::npos) {
				auto gun_data = (u_data_gun*)hand_item;
				std::string item_name = hand_item->get_name().read_string();
				ItemProperties itemprops = GetItemProperties(item_name);
				if (!auto_fire) {
					gun_data->set_auto_fire(itemprops.auto_fire);
				}
				if (!rapid_fire) {
					gun_data->set_fire_rate(itemprops.fire_rate);
				}
				if (!no_recoil) {
					gun_data->set_shake_intensity(itemprops.shake_intensity);

					gun_data->set_oscillation_reactivity(itemprops.oscillation_reactivity);
					gun_data->set_walk_oscillation(itemprops.walk_oscillation);
					gun_data->set_run_oscillation(itemprops.run_oscillation);
					gun_data->set_stand_oscillation(itemprops.stand_oscillation);

					gun_data->set_recoil_reactivity(itemprops.recoil_reactivity);
					gun_data->set_walk_precision(itemprops.walk_precision);
					gun_data->set_air_precision(itemprops.air_precision);
					gun_data->set_run_precision(itemprops.run_precision);
				}
				if (!max_damage) {
					gun_data->set_damage(itemprops.damage);
				}
			}
		}
	}

	if (player_esp) {
		for (auto mec : player_cache) {
			auto state = mec->player_state();
			if (!state) continue;

			if (mec != local_mec) {
				auto name = state->get_player_name_private().read_string();
				//std::string name = "Женечка";

				auto role = mec->get_player_role();

				ImU32 color = (role == 4)
					? ImGui::ColorConvertFloat4ToU32(dissident_color)
					: ImGui::ColorConvertFloat4ToU32(employee_color);
				ImU32 ghostcolor = (role == 4)
					? ImGui::ColorConvertFloat4ToU32(ghost_dissident_color)
					: ImGui::ColorConvertFloat4ToU32(ghost_employee_color);

				// Temporary color with modified alpha
				ImVec4 temp_color = (role == 4) ? dissident_color : employee_color;
				temp_color.w = 0.05f;  // 10% transparency
				ImU32 color_with_alpha = ImGui::ColorConvertFloat4ToU32(temp_color);

				// Temporary color with modified alpha
				ImVec4 temp_ghostcolor = (role == 4) ? ghost_dissident_color : ghost_employee_color;
				temp_ghostcolor.w = 0.05f;  // 10% transparency
				ImU32 color_with_alpha_ghost = ImGui::ColorConvertFloat4ToU32(temp_ghostcolor);

				auto mec_root = mec->get_root_component();
				if (!mec_root) continue;

				auto ghost_root = mec->get_ghost_root();
				if (!ghost_root) continue;

				auto position = mec_root->get_relative_location(); // Player location (feet)
				auto ghostPosition = ghost_root->get_relative_location(); // Ghost location (head/base)

				// Calculate the distance between ghost and player
				double ghost_distance = CalculateDistanceMeters(position, ghostPosition);

				if (ghost_distance > 2) { // If ghost moved away from the body
					if (player_ghost) {
						if (player_box) {
							vector3 screen_position_top, screen_position_bottom;

							// Adjust for ghost height and feet position
							bool top_visible = util::w2s(ghostPosition + vector3{ 0, 0, 20 }, last_frame_cached.pov, screen_position_top); // Adjust for head height
							bool bottom_visible = util::w2s(ghostPosition, last_frame_cached.pov, screen_position_bottom); // Adjust for feet

							if (top_visible && bottom_visible) {
								float bottom_offset = 30.0f; // Ghost-specific bottom offset
								screen_position_bottom.y += bottom_offset;

								// Calculate box dimensions
								float box_height = screen_position_bottom.y - screen_position_top.y;
								float box_width = box_height * 1.0f; // Ghost-specific width-to-height ratio

								ImVec2 box_pos1 = ImVec2(screen_position_top.x - box_width * 0.5f, screen_position_top.y);
								ImVec2 box_pos2 = ImVec2(screen_position_top.x + box_width * 0.5f, screen_position_bottom.y);

								// Define corner line properties
								float corner_line_length = box_width * 0.25f; // Adjust for desired length
								float corner_thickness = 2.0f;               // Adjust for desired thickness
								ImU32 corner_color = ghostcolor;

								// Top-left corner lines
								overlay->draw_rect_filled(box_pos1, ImVec2(box_pos1.x + corner_line_length, box_pos1.y + corner_thickness), corner_color);
								overlay->draw_rect_filled(box_pos1, ImVec2(box_pos1.x + corner_thickness, box_pos1.y + corner_line_length), corner_color);

								// Top-right corner lines
								overlay->draw_rect_filled(ImVec2(box_pos2.x - corner_line_length, box_pos1.y), ImVec2(box_pos2.x, box_pos1.y + corner_thickness), corner_color);
								overlay->draw_rect_filled(ImVec2(box_pos2.x - corner_thickness, box_pos1.y), ImVec2(box_pos2.x, box_pos1.y + corner_line_length), corner_color);

								// Bottom-left corner lines
								overlay->draw_rect_filled(ImVec2(box_pos1.x, box_pos2.y - corner_thickness), ImVec2(box_pos1.x + corner_line_length, box_pos2.y), corner_color);
								overlay->draw_rect_filled(ImVec2(box_pos1.x, box_pos2.y - corner_line_length), ImVec2(box_pos1.x + corner_thickness, box_pos2.y), corner_color);

								// Bottom-right corner lines
								overlay->draw_rect_filled(ImVec2(box_pos2.x - corner_line_length, box_pos2.y - corner_thickness), box_pos2, corner_color);
								overlay->draw_rect_filled(ImVec2(box_pos2.x - corner_thickness, box_pos2.y - corner_line_length), box_pos2, corner_color);

								// Optional: Draw filled rectangle inside the box
								ImU32 fill_color = color_with_alpha_ghost;
								overlay->draw_rect_filled(
									ImVec2(box_pos1.x, box_pos1.y),
									ImVec2(box_pos2.x, box_pos2.y),
									fill_color
								);
							}
						}

						vector3 ghost_screen_position{};
						if (util::w2s(ghostPosition, last_frame_cached.pov, ghost_screen_position)) {
							auto distance = CalculateDistance(local_mec->get_net_location(), ghostPosition);

							// Render ghost name centered
							std::string ghost_name = "[" + name + "]" + "[G]" + (player_distance ? "[" + distance + "m]" : "");
							int ghost_text_width = ghost_name.length() * 7; // Approx character width in pixels
							ghost_screen_position.x -= ghost_text_width / 2;

							overlay->draw_text(ghost_screen_position, ghostcolor, ghost_name.c_str(), true);
						}
					}
				}
				else { // Show player ESP if ghost is close
					auto distance = CalculateDistance(local_mec->get_net_location(), position);
					double distanceDouble = std::stod(distance);

					if (distanceDouble <= esp_max_distance) {
						if (player_box) {
							vector3 screen_position_top, screen_position_bottom;
							bool top_visible = util::w2s(position + vector3{ 0, 0, 190 }, last_frame_cached.pov, screen_position_top); // Adjust for head height
							bool bottom_visible = util::w2s(position, last_frame_cached.pov, screen_position_bottom); // Adjust for feet

							if (top_visible && bottom_visible) {
								float bottom_offset = 15.0f;
								screen_position_bottom.y += bottom_offset;

								float box_height = screen_position_bottom.y - screen_position_top.y;
								float box_width = box_height * 0.6f;

								ImVec2 box_pos1 = ImVec2(screen_position_top.x - box_width * 0.5f, screen_position_top.y);
								ImVec2 box_pos2 = ImVec2(screen_position_top.x + box_width * 0.5f, screen_position_bottom.y);

								// Define corner line properties
								float corner_line_length = box_width * 0.2f; // Adjust for desired length
								float corner_thickness = 3.0f;              // Adjust for desired thickness
								ImU32 corner_color = color;

								// Top-left corner lines
								overlay->draw_rect_filled(box_pos1, ImVec2(box_pos1.x + corner_line_length, box_pos1.y + corner_thickness), corner_color);
								overlay->draw_rect_filled(box_pos1, ImVec2(box_pos1.x + corner_thickness, box_pos1.y + corner_line_length), corner_color);

								// Top-right corner lines
								overlay->draw_rect_filled(ImVec2(box_pos2.x - corner_line_length, box_pos1.y), ImVec2(box_pos2.x, box_pos1.y + corner_thickness), corner_color);
								overlay->draw_rect_filled(ImVec2(box_pos2.x - corner_thickness, box_pos1.y), ImVec2(box_pos2.x, box_pos1.y + corner_line_length), corner_color);

								// Bottom-left corner lines
								overlay->draw_rect_filled(ImVec2(box_pos1.x, box_pos2.y - corner_thickness), ImVec2(box_pos1.x + corner_line_length, box_pos2.y), corner_color);
								overlay->draw_rect_filled(ImVec2(box_pos1.x, box_pos2.y - corner_line_length), ImVec2(box_pos1.x + corner_thickness, box_pos2.y), corner_color);

								// Bottom-right corner lines
								overlay->draw_rect_filled(ImVec2(box_pos2.x - corner_line_length, box_pos2.y - corner_thickness), box_pos2, corner_color);
								overlay->draw_rect_filled(ImVec2(box_pos2.x - corner_thickness, box_pos2.y - corner_line_length), box_pos2, corner_color);

								// Filled rectangle inside the box
								ImU32 fill_color = color_with_alpha;
								overlay->draw_rect_filled(
									ImVec2(box_pos1.x, box_pos1.y),
									ImVec2(box_pos2.x, box_pos2.y),
									fill_color
								);
							}
						}

						vector3 screen_position{};
						if (util::w2s(position, last_frame_cached.pov, screen_position)) {
							std::string name_norm = "[" + name + "]" + (role == 4 ? "[D]" : "") + (player_distance ? "[" + distance + "m]" : "");

							int text_width = name_norm.length() * 7;
							screen_position.x -= text_width / 2;

							overlay->draw_text(screen_position, color, name_norm.c_str(), true);

							if (player_inventory) {
								auto mec_hand_item_data = mec->get_net_hand_item_new().Data_18;
								auto mec_hand_item_state = mec->get_net_hand_item_new().State_19;

								auto mec_bag_item_data = mec->get_net_bag_item_new().Data_18;
								auto mec_bag_item_state = mec->get_net_bag_item_new().State_19;

								std::string mec_hand_item_name = mec_hand_item_data ? mec_hand_item_data->get_name().read_string() : "";
								std::string mec_bag_item_name = mec_bag_item_data ? mec_bag_item_data->get_name().read_string() : "";

								struct ItemStateInfo {
									std::string text;
									ImU32 color;
								};

								auto format_item_state = [](const std::string& item_name, const auto& item_state) -> ItemStateInfo {
									ItemStateInfo info;
									info.color = ImGui::ColorConvertFloat4ToU32(screw_driver_color); // Default color

									if (item_name == "VENT FILTER") {
										info.text = "Clean: " + std::to_string(item_state.Value_8) + "%";
										info.color = ImGui::ColorConvertFloat4ToU32(vent_filter_color);
									}
									else if (item_name == "BATTERY") {
										info.text = "Charge: " + std::to_string(item_state.Value_8) + "%";
										info.color = ImGui::ColorConvertFloat4ToU32(battery_color);
									}
									else if (item_name == "REVOLVER" || item_name == "PISTOL" || item_name == "SHORTY" ||
										item_name == "SMG" || item_name == "RIFLE" || item_name == "SHOTGUN") {
										info.text = "Ammo: " + std::to_string(item_state.Value_8);
										info.color = ImGui::ColorConvertFloat4ToU32(weapon_color);
									}
									else if (item_name == "KNIFE" || item_name == "C4" || item_name == "DETONATOR") {
										info.text = "";
										info.color = ImGui::ColorConvertFloat4ToU32(weapon_color);
									}
									else if (item_name == "CONTAINER") {
										const char* colors[] = { "Empty", "Green", "Yellow", "Blue", "White", "Red", "Dirty" };
										int current_value = (item_state.Value_8 == -1) ? 6 : item_state.Value_8;
										info.text = std::string(colors[current_value]);
										info.color = ImGui::ColorConvertFloat4ToU32(container_color);
									}
									else if (item_name == "CASSETTE") {
										const char* cassette_titles[] = {
											"KHARMA", "Who Am I", "Burning Wish", "Cake", "Puzzle",
											"Sun", "Worship", "Royalty (Instrumental)", "Grave"
										};
										if (item_state.Value_8 >= 0 && item_state.Value_8 < IM_ARRAYSIZE(cassette_titles)) {
											info.text = "Song: " + std::string(cassette_titles[item_state.Value_8]);
										}
										else {
											info.text = "Song: Unknown";
										}
										info.color = ImGui::ColorConvertFloat4ToU32(screw_driver_color);
									}
									else if (item_name == "NAME") {
										const char* rice_options[] = { "White Rice", "Brown Rice", "Black Rice" };
										const char* fish_options[] = { "Salmon", "Tuna", "Cod", "Shrimp" };
										const char* container_colors[] = { "Green", "Yellow", "Blue", "White", "Red" };

										int value = item_state.Value_8;
										int rice_value = value / 100;
										int fish_value = (value / 10) % 10;
										int container_value = value % 10;

										std::string rice = (rice_value > 0 && rice_value <= IM_ARRAYSIZE(rice_options)) ? rice_options[rice_value - 1] : "Unknown";
										std::string fish = (fish_value > 0 && fish_value <= IM_ARRAYSIZE(fish_options)) ? fish_options[fish_value - 1] : "Unknown";
										std::string container = (container_value > 0 && container_value <= IM_ARRAYSIZE(container_colors)) ? container_colors[container_value - 1] : "Unknown";

										info.text = "" + rice + "|" + fish + "|" + container;
										info.color = ImGui::ColorConvertFloat4ToU32(screw_driver_color);
									}
									else if (item_name == "FUSE") {
										const char* fuse_colors[] = { "Red", "Yellow", "Blue" };
										std::string value_color = (item_state.Value_8 > 0 && item_state.Value_8 <= IM_ARRAYSIZE(fuse_colors))
											? fuse_colors[item_state.Value_8 - 1] : "Unknown";
										std::string time_color = (item_state.Time_15 > 0 && item_state.Time_15 <= IM_ARRAYSIZE(fuse_colors))
											? fuse_colors[item_state.Time_15 - 1] : "Unknown";

										info.text = value_color + "|" + time_color;
										info.color = ImGui::ColorConvertFloat4ToU32(fuse_color);
									}
									else if (item_name == "SAMPLE") {
										const char* sample_colors[] = { "Empty", "Green", "Yellow", "Blue", "White", "Red",
																		"Green Mix", "Yellow Mix", "Blue Mix", "White Mix" };
										int index = (item_state.Value_8 == 0)
											? 0
											: (item_state.Time_15 == 1 ? item_state.Value_8 + 5 : item_state.Value_8);

										info.text = ((index >= 0 && index < IM_ARRAYSIZE(sample_colors))
											? std::string(sample_colors[index])
											: "Unknown");
										info.color = ImGui::ColorConvertFloat4ToU32(sample_color);
									}
									else if (item_name == "RICE") {
										const char* rice_types[] = { "White", "Brown", "Black" };
										int rice_index = item_state.Value_8 - 1;

										info.text = ((rice_index >= 0 && rice_index < IM_ARRAYSIZE(rice_types))
											? std::string(rice_types[rice_index])
											: "Unknown");
										info.color = ImGui::ColorConvertFloat4ToU32(rice_color);
									}
									else if (item_name == "FISH") {
										const char* fish_types[] = { "Salmon", "Tuna", "Cod", "Shrimp" };
										int fish_index = item_state.Value_8 - 1;

										info.text = ((fish_index >= 0 && fish_index < IM_ARRAYSIZE(fish_types))
											? std::string(fish_types[fish_index])
											: "Unknown");
										info.color = ImGui::ColorConvertFloat4ToU32(screw_driver_color);
									}
									else if (item_name == "PACKAGE") {
										const char* package_types[] = { "Security", "Computers", "Botanic", "Restaurant", "Medical", "Tutorial", "Machine" };
										int package_index = item_state.Value_8 - 1;

										info.text = ((package_index >= 0 && package_index < IM_ARRAYSIZE(package_types))
											? std::string(package_types[package_index])
											: "Unknown");
										info.color = ImGui::ColorConvertFloat4ToU32(package_color);
									}
									else if (item_name == "GAZ BOTTLE") {
										const char* colors[] = { "Yellow", "Red", "Blue" };
										int color_index = item_state.Value_8;

										info.text = ((color_index >= 0 && color_index < IM_ARRAYSIZE(colors))
											? std::string(colors[color_index])
											: "Unknown");
										info.color = ImGui::ColorConvertFloat4ToU32(gaz_bottle_color);
									}

									return info;
									};

								// Use the helper function to format item state and retrieve text and color
								auto hand_item_info = format_item_state(mec_hand_item_name, mec_hand_item_state);
								auto bag_item_info = format_item_state(mec_bag_item_name, mec_bag_item_state);

								// Prepare the item name display strings
								std::string hand_item_display_text = !mec_hand_item_name.empty() ? "[" + mec_hand_item_name + "]" : "";
								std::string bag_item_display_text = !mec_bag_item_name.empty() ? "[" + mec_bag_item_name + "]" : "";

								// Prepare the item state display strings
								std::string hand_state_text = !hand_item_info.text.empty() ? "[" + hand_item_info.text + "]" : "";
								std::string bag_state_text = !bag_item_info.text.empty() ? "[" + bag_item_info.text + "]" : "";

								// Save the original X position for resetting after line breaks
								float original_x_position = screen_position.x;
								screen_position.y += 15;

								// Calculate text widths for positioning
								float hand_name_width = calculate_text_width(hand_item_display_text);
								float hand_state_width = calculate_text_width(hand_state_text);
								float bag_name_width = calculate_text_width(bag_item_display_text);
								float bag_state_width = calculate_text_width(bag_state_text);

								// Determine where the bag name and state should start
								float bag_name_x_position = original_x_position + (mec_hand_item_name.empty() ? 0 : (std::max)(hand_name_width, hand_state_width));
								float bag_state_x_position = bag_name_x_position;

								// Draw the first line: Hand and Bag item names
								if (!hand_item_display_text.empty() || !bag_item_display_text.empty()) {
									if (!hand_item_display_text.empty()) {
										overlay->draw_text(screen_position, hand_item_info.color, hand_item_display_text.c_str(), true);
									}

									if (!bag_item_display_text.empty()) {
										// Set the position for the bag name
										screen_position.x = bag_name_x_position;
										overlay->draw_text(screen_position, bag_item_info.color, bag_item_display_text.c_str(), true);
									}
								}

								// Reset X position and move Y down for the second line
								screen_position.y += 15;
								screen_position.x = original_x_position;

								// Draw the second line: Hand and Bag item states
								if (!hand_state_text.empty() || !bag_state_text.empty()) {
									if (!hand_state_text.empty()) {
										overlay->draw_text(screen_position, hand_item_info.color, hand_state_text.c_str(), true);
									}

									if (!bag_state_text.empty()) {
										// Set the position for the bag state
										screen_position.x = bag_state_x_position;
										overlay->draw_text(screen_position, bag_item_info.color, bag_state_text.c_str(), true);
									}
								}

							}
						}
					}
				}
			}
		}
	}

	for (auto item : world_item_cache) {
		if (!item) continue;

		auto data = item->get_data();
		auto item_name = data->get_name().read_string();
		auto item_state = item->get_item_state();
		int item_value = item_state.Value_8;
		int item_time = item_state.Time_15;
		ImU32 weapon_esp_color = ImGui::ColorConvertFloat4ToU32(weapon_color);
		ImU32 gaz_bottle_esp_color = ImGui::ColorConvertFloat4ToU32(gaz_bottle_color);
		ImU32 vent_filter_esp_color = ImGui::ColorConvertFloat4ToU32(vent_filter_color);
		ImU32 package_esp_color = ImGui::ColorConvertFloat4ToU32(package_color);
		ImU32 rice_esp_color = ImGui::ColorConvertFloat4ToU32(rice_color);
		ImU32 sample_esp_color = ImGui::ColorConvertFloat4ToU32(sample_color);
		ImU32 fuse_esp_color = ImGui::ColorConvertFloat4ToU32(fuse_color);
		ImU32 battery_esp_color = ImGui::ColorConvertFloat4ToU32(battery_color);
		ImU32 screw_driver_esp_color = ImGui::ColorConvertFloat4ToU32(screw_driver_color);
		ImU32 container_esp_color = ImGui::ColorConvertFloat4ToU32(container_color);

		if (item_name == "PISTOL" || item_name == "REVOLVER" || item_name == "SHORTY" || item_name == "SMG" || item_name == "RIFLE" || item_name == "SHOTGUN") {
			auto gun_data = (u_data_gun*)data;
			auto item_root = item->get_root_component();
			if (!item_root) continue;

			if (weapon_esp) {
				auto position = item_root->get_relative_location();
				auto distance = CalculateDistance(local_mec->get_net_location(), position);
				double distanceDouble = std::stod(distance);
				if (distanceDouble <= esp_max_distance) {
					vector3 screen_position{};
					if (util::w2s(position, last_frame_cached.pov, screen_position)) {
						// Calculate text width based on character count and font size
						std::string name_norm = "[" + item_name + "]" + (weapon_distance ? "[" + distance + "m]" : "");
						int text_width = name_norm.length() * 7; // Assume each character is approximately 6 pixels wide
						screen_position.x -= text_width / 2; // Shift the position left by half the text width

						overlay->draw_text(screen_position, weapon_esp_color, name_norm.c_str(), true); // Orange

						if (weapon_item_state) {
							screen_position.y += 15;
							overlay->draw_text(screen_position, weapon_esp_color, ("[Ammo: " + std::to_string(item_value) + "]").c_str(), true);
						}
					}
				}
			}
		}

		if (item_name == "KNIFE" || item_name == "C4" || item_name == "DETONATOR") {
			auto item_root = item->get_root_component();
			if (!item_root) continue;

			if (weapon_esp) {
				auto position = item_root->get_relative_location();
				auto distance = CalculateDistance(local_mec->get_net_location(), position);
				double distanceDouble = std::stod(distance);

				if (distanceDouble <= esp_max_distance) {
					vector3 screen_position{};
					if (util::w2s(position, last_frame_cached.pov, screen_position)) {
						// Calculate text width based on character count and font size
						std::string name_norm = "[" + item_name + "]" + (weapon_distance ? "[" + distance + "m]" : "");
						int text_width = name_norm.length() * 7; // Assume each character is approximately 6 pixels wide
						screen_position.x -= text_width / 2; // Shift the position left by half the text width

						overlay->draw_text(screen_position, weapon_esp_color, name_norm.c_str(), true); // Orange
					}
				}
			}
		}

		if (item_name == "GAZ BOTTLE" || item_name == "VENT FILTER" || item_name == "RICE" || item_name == "PACKAGE" || item_name == "SAMPLE") {
			auto item_root = item->get_root_component();
			if (!item_root) continue;

			if (primary_object_esp) {
				auto position = item_root->get_relative_location();
				auto distance = CalculateDistance(local_mec->get_net_location(), position);
				double distanceDouble = std::stod(distance);

				if (distanceDouble <= esp_max_distance) {
					vector3 screen_position{};
					if (util::w2s(position, last_frame_cached.pov, screen_position)) {

						if (item_name == "GAZ BOTTLE") {
							// Calculate text width based on character count and font size
							std::string name_norm = "[" + item_name + "]" + (primary_distance ? "[" + distance + "m]" : "");
							int text_width = name_norm.length() * 7; // Assume each character is approximately 6 pixels wide
							screen_position.x -= text_width / 2; // Shift the position left by half the text width

							overlay->draw_text(screen_position, gaz_bottle_esp_color, name_norm.c_str(), true); // Blue

							if (primary_item_state) {
								if (item_value == 0) {
									screen_position.y += 15;
									overlay->draw_text(screen_position, gaz_bottle_esp_color, "[Color: Yellow]", true);
								}
								else if (item_value == 1) {
									screen_position.y += 15;
									overlay->draw_text(screen_position, gaz_bottle_esp_color, "[Color: Red]", true);
								}
								else if (item_value == 2) {
									screen_position.y += 15;
									overlay->draw_text(screen_position, gaz_bottle_esp_color, "[Color: Blue]", true);
								}
							}
						}
						else if (item_name == "VENT FILTER") {
							// Calculate text width based on character count and font size
							std::string name_norm = "[" + item_name + "]" + (primary_distance ? "[" + distance + "m]" : "");
							int text_width = name_norm.length() * 7; // Assume each character is approximately 6 pixels wide
							screen_position.x -= text_width / 2; // Shift the position left by half the text width

							overlay->draw_text(screen_position, vent_filter_esp_color, name_norm.c_str(), true); // Light Blue

							if (primary_item_state) {
								screen_position.y += 15;
								overlay->draw_text(screen_position, vent_filter_esp_color, ("[Clean: " + std::to_string(item_value) + "%]").c_str(), true);
							}
						}
						else if (item_name == "RICE") {
							// Calculate text width based on character count and font size
							std::string name_norm = "[" + item_name + "]" + (primary_distance ? "[" + distance + "m]" : "");

							if (item_value == 1) {
								name_norm = "[WHITE RICE]" + (primary_distance ? "[" + distance + "m]" : "");
								int text_width = name_norm.length() * 7; // Assume each character is approximately 6 pixels wide
								screen_position.x -= text_width / 2; // Shift the position left by half the text width

								overlay->draw_text(screen_position, rice_esp_color, name_norm.c_str(), true);
							}
							else if (item_value == 2) {
								name_norm = "[BROWN RICE]" + (primary_distance ? "[" + distance + "m]" : "");
								int text_width = name_norm.length() * 7; // Assume each character is approximately 6 pixels wide
								screen_position.x -= text_width / 2; // Shift the position left by half the text width

								overlay->draw_text(screen_position, rice_esp_color, name_norm.c_str(), true);
							}
							else if (item_value == 3) {
								name_norm = "[BLACK RICE]" + (primary_distance ? "[" + distance + "m]" : "");
								int text_width = name_norm.length() * 7; // Assume each character is approximately 6 pixels wide
								screen_position.x -= text_width / 2; // Shift the position left by half the text width

								overlay->draw_text(screen_position, rice_esp_color, name_norm.c_str(), true);
							}
						}
						else if (item_name == "PACKAGE") {
							// Calculate text width based on character count and font size
							std::string name_norm = "[" + item_name + "]" + (primary_distance ? "[" + distance + "m]" : "");
							int text_width = name_norm.length() * 7; // Assume each character is approximately 6 pixels wide
							screen_position.x -= text_width / 2; // Shift the position left by half the text width

							overlay->draw_text(screen_position, package_esp_color, name_norm.c_str(), true); // Brown

							if (primary_item_state) {
								if (item_value == 1) {
									screen_position.y += 15;
									overlay->draw_text(screen_position, package_esp_color, "[Security]", true);
								}
								else if (item_value == 2) {
									screen_position.y += 15;
									overlay->draw_text(screen_position, package_esp_color, "[Computers]", true);
								}
								else if (item_value == 3) {
									screen_position.y += 15;
									overlay->draw_text(screen_position, package_esp_color, "[Botanic]", true);
								}
								else if (item_value == 4) {
									screen_position.y += 15;
									overlay->draw_text(screen_position, package_esp_color, "[Restaurant]", true);
								}
								else if (item_value == 5) {
									screen_position.y += 15;
									overlay->draw_text(screen_position, package_esp_color, "[Medical]", true);
								}
								else if (item_value == 6) {
									screen_position.y += 15;
									overlay->draw_text(screen_position, package_esp_color, "[Tutorial]", true);
								}
								else if (item_value == 7) {
									screen_position.y += 15;
									overlay->draw_text(screen_position, package_esp_color, "[Machine]", true);
								}
							}
						}
						else if (item_name == "SAMPLE") {
							// Calculate text width based on character count and font size
							std::string name_norm;
							std::string distance_str = primary_distance ? "[" + distance + "m]" : "";

							if (item_value == 1) {
								name_norm = (item_time == 1) ? "[GREEN MIX SAMPLE]" : "[GREEN SAMPLE]";
							}
							else if (item_value == 2) {
								name_norm = (item_time == 1) ? "[YELLOW MIX SAMPLE]" : "[YELLOW SAMPLE]";
							}
							else if (item_value == 3) {
								name_norm = (item_time == 1) ? "[BLUE MIX SAMPLE]" : "[BLUE SAMPLE]";
							}
							else if (item_value == 4) {
								name_norm = (item_time == 1) ? "[WHITE MIX SAMPLE]" : "[WHITE SAMPLE]";
							}
							else if (item_value == 5) {
								name_norm = "[RED SAMPLE]"; // Red doesn't mix, so no mix logic
							}

							name_norm += distance_str; // Append the distance if applicable

							int text_width = name_norm.length() * 7; // Assume each character is approximately 7 pixels wide
							screen_position.x -= text_width / 2;    // Center the text

							overlay->draw_text(screen_position, sample_esp_color, name_norm.c_str(), true);
						}
					}
				}
			}
		}

		if (item_name == "FUSE" || item_name == "BATTERY" || item_name == "SCREW DRIVER" || item_name == "CONTAINER") {
			auto item_root = item->get_root_component();
			if (!item_root) continue;

			if (secondary_object_esp) {
				auto position = item_root->get_relative_location();
				auto distance = CalculateDistance(local_mec->get_net_location(), position);
				double distanceDouble = std::stod(distance);

				if (distanceDouble <= esp_max_distance) {
					vector3 screen_position{};
					if (util::w2s(position, last_frame_cached.pov, screen_position)) {
						// Calculate text width based on character count and font size
						std::string name_norm = "[" + item_name + "]" + (secondary_distance ? "[" + distance + "m]" : "");
						int text_width = name_norm.length() * 7; // Assume each character is approximately 6 pixels wide
						screen_position.x -= text_width / 2; // Shift the position left by half the text width

						if (item_name == "FUSE") {
							overlay->draw_text(screen_position, fuse_esp_color, name_norm.c_str(), true); // Grey

							if (secondary_item_state) {
								const char* color_names[] = { "INVALID", "RED", "YELLOW", "BLUE" };

								int color_index_value = (item_value >= 1 && item_value <= 3) ? item_value : 0;
								int color_index_time = (item_time >= 1 && item_time <= 3) ? item_time : 0;

								std::string fuse_info = "[" + std::string(color_names[color_index_value]) + " | " + std::string(color_names[color_index_time]) + "]";

								screen_position.y += 15;
								overlay->draw_text(screen_position, fuse_esp_color, fuse_info.c_str(), true);
							}
						}
						else if (item_name == "BATTERY") {
							overlay->draw_text(screen_position, battery_esp_color, name_norm.c_str(), true); // Light Grey

							if (secondary_item_state) {
								screen_position.y += 15;
								overlay->draw_text(screen_position, battery_esp_color, ("[Charge: " + std::to_string(item_value) + "%]").c_str(), true);
							}
						}
						else if (item_name == "SCREW DRIVER") {
							overlay->draw_text(screen_position, screw_driver_esp_color, name_norm.c_str(), true); // White
						}
						else if (item_name == "CONTAINER") {
							overlay->draw_text(screen_position, container_esp_color, name_norm.c_str(), true); // Pink-sih purple

							if (secondary_item_state) {
								screen_position.y += 15;
								if (item_value == -1) {
									overlay->draw_text(screen_position, container_esp_color, "[Dirty]", true);
								}
								if (item_value == 0) {
									overlay->draw_text(screen_position, container_esp_color, "[Clean]", true);
								}
								else if (item_value == 1) {
									overlay->draw_text(screen_position, container_esp_color, "[Green]", true);
								}
								else if (item_value == 2) {
									overlay->draw_text(screen_position, container_esp_color, "[Yellow]", true);
								}
								else if (item_value == 3) {
									overlay->draw_text(screen_position, container_esp_color, "[Blue", true);
								}
								else if (item_value == 4) {
									overlay->draw_text(screen_position, container_esp_color, "[White]", true);
								}
								else if (item_value == 5) {
									overlay->draw_text(screen_position, container_esp_color, "[Red]", true);
								}
								else if (item_value == 6) {
									if (item_time == 1) {
										overlay->draw_text(screen_position, container_esp_color, "[White Rice]", true);
									}
									else if (item_time == 2) {
										overlay->draw_text(screen_position, container_esp_color, "[Brown Rice]", true);
									}
									else if (item_time == 3) {
										overlay->draw_text(screen_position, container_esp_color, "[Black Rice]", true);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	for (auto vent : task_vents_cache) {
		if (!vent) continue;
		auto role = local_mec->get_player_role();
		if (role == 3 || role == 4) {
			if (task_object_esp) {
				auto task_vents_array = vent->get_task_vents();
				auto task_vents = task_vents_array.list();

				if (task_vent) {
					ImU32 task_color = ImGui::ColorConvertFloat4ToU32(task_vent_color);

					for (auto taskVent : task_vents) {
						if (!taskVent) continue;
						auto ventRoot = taskVent->get_root();
						auto filter = taskVent->get_filter();
						if (!ventRoot) {
							std::cout << "vent root is null!" << std::endl;
							continue;
						}

						if (!filter) {
							std::cout << "Filter is null for vent!" << std::endl;
							continue;
						}

						if (ventRoot && filter) {
							auto filterState = filter->get_item_state();
							auto ventLocation = ventRoot->get_relative_location();
							auto distance = CalculateDistance(local_mec->get_net_location(), ventLocation);
							double distanceDouble = std::stod(distance);

							if (distanceDouble <= esp_max_distance) {
								vector3 screen_position{};
								if (util::w2s(ventLocation, last_frame_cached.pov, screen_position)) {
									// Calculate text width based on character count and font size
									std::string name_norm = "[VENT TASK]" + (task_object_distance ? "[" + distance + "m]" : "");
									int text_width = name_norm.length() * 7; // Assume each character is approximately 6 pixels wide
									screen_position.x -= text_width / 2; // Shift the position left by half the text width

									if (filterState.Value_8 != 100) {
										overlay->draw_text(screen_position, task_color, name_norm.c_str(), true);

										// Clean percentage display
										if (task_object_state) {
											screen_position.y += 15;
											overlay->draw_text(screen_position, task_color, ("[Clean: " + std::to_string(filterState.Value_8) + "%]").c_str(), true);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	for (auto machines : task_machines_cache) {
		if (!machines) continue;
		auto role = local_mec->get_player_role();

		if (role == 3 || role == 4) {
			if (task_object_esp) {
				auto task_machines_array = machines->get_machines();
				auto task_machines = task_machines_array.list();

				if (task_machine) {
					ImU32 task_color = ImGui::ColorConvertFloat4ToU32(task_machine_color);

					for (auto taskMachine : task_machines) {
						if (!taskMachine) continue;

						auto machine_bottles_array = taskMachine->get_bottles();
						auto task_bottles = machine_bottles_array.list();

						for (auto machineBottle : task_bottles) {
							if (!machineBottle) continue;
							auto bottleRoot = machineBottle->get_root();

							auto bottleReqLevel = machineBottle->get_request_level();
							auto bottleLevel = machineBottle->get_level();

							if (bottleLevel != bottleReqLevel && bottleRoot) {  // Only if levels don’t match
								auto bottleLocation = bottleRoot->get_relative_location();
								auto distance = CalculateDistance(local_mec->get_net_location(), bottleLocation);
								double distanceDouble = std::stod(distance);

								if (distanceDouble <= esp_max_distance) {
									vector3 screen_position{};
									if (util::w2s(bottleLocation, last_frame_cached.pov, screen_position)) {
										// Calculate text width based on character count and font size
										std::string name_norm = "[BOTTLE]" + (task_object_distance ? "[" + distance + "m]" : "");
										int text_width = name_norm.length() * 7; // Assume each character is approximately 6 pixels wide
										screen_position.x -= text_width / 2; // Shift the position left by half the text width

										overlay->draw_text(screen_position, task_color, name_norm.c_str(), true);

										// Display requested color
										if (task_object_state) {
											std::string bottleColor;
											switch (bottleReqLevel) {
											case 0: bottleColor = "Yellow"; break;
											case 1: bottleColor = "Red"; break;
											case 2: bottleColor = "Blue"; break;
											default: bottleColor = "Unknown"; break;
											}

											screen_position.y += 15;
											std::string text = "[Color: " + bottleColor + "]";
											overlay->draw_text(screen_position, task_color, text.c_str(), true);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	for (auto alim : task_alims_cache) {
		if (!alim) continue;
		auto role = local_mec->get_player_role();

		if (role == 3 || role == 4) {
			if (task_object_esp) {
				auto task_alim_array = alim->get_task_alims();
				auto task_alims = task_alim_array.list();

				if (task_alim) {
					ImU32 task_color = ImGui::ColorConvertFloat4ToU32(task_alim_color);

					for (auto taskAlim : task_alims) {
						if (!taskAlim) continue;
						auto alimRoot = taskAlim->get_root();
						auto alimBatteryLevel = taskAlim->get_battery_value();
						auto alimValue = taskAlim->get_task_value();
						auto alimFuseIn = taskAlim->get_in_color();
						auto alimFuseOut = taskAlim->get_out_color();

						if (!alimRoot) {
							std::cout << "alim root is null!" << std::endl;
							continue;
						}

						auto alimLocation = alimRoot->get_relative_location();
						alimLocation.z += 200;

						auto distance = CalculateDistance(local_mec->get_net_location(), alimLocation);
						double distanceDouble = std::stod(distance);

						if (distanceDouble <= esp_max_distance) {
							vector3 screen_position{};
							if (util::w2s(alimLocation, last_frame_cached.pov, screen_position)) {
								// Calculate text width based on character count and font size
								std::string name_norm = "[ALIMENTATION]" + (task_object_distance ? "[" + distance + "m]" : "");

								if (alimValue != 100) {
									overlay->draw_text(screen_position, task_color, name_norm.c_str(), true);

									if (task_object_state) {
										screen_position.y += 15;
										overlay->draw_text(screen_position, task_color, ("[Battery: " + std::to_string(alimBatteryLevel) + "%]").c_str(), true);

										// Define the colors corresponding to each value
										const char* fuse_colors[] = { "Invalid", "Red", "Yellow", "Blue" };

										// Ensure that alimFuseIn and alimFuseOut are within the valid range
										std::string fuse_in_color = (alimFuseIn >= 1 && alimFuseIn <= 3) ? fuse_colors[alimFuseIn] : "Invalid";
										std::string fuse_out_color = (alimFuseOut >= 1 && alimFuseOut <= 3) ? fuse_colors[alimFuseOut] : "Invalid";

										screen_position.y += 15;
										overlay->draw_text(screen_position, task_color, ("[" + fuse_in_color + " | " + fuse_out_color + "]").c_str(), true);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	for (auto delivery : task_delivery_cache) {
		if (!delivery) continue;
		auto role = local_mec->get_player_role();

		if (role == 3 || role == 4) {
			if (task_object_esp) {
				auto task_delivery_array = delivery->get_task_cases();
				auto task_cases = task_delivery_array.list();

				if (task_delivery) {
					ImU32 task_color = ImGui::ColorConvertFloat4ToU32(task_delivery_color);

					for (auto deliveryCase : task_cases) {
						if (!deliveryCase) continue;
						auto caseRoot = deliveryCase->get_root();
						if (!caseRoot) {
							std::cout << "delivery case root is null!" << std::endl;
							continue;
						}

						auto deliveryLocation = caseRoot->get_relative_location();
						auto goodPackage = deliveryCase->get_good_package();

						auto distance = CalculateDistance(local_mec->get_net_location(), deliveryLocation);
						double distanceDouble = std::stod(distance);

						if (distanceDouble <= esp_max_distance) {
							vector3 screen_position{};
							if (util::w2s(deliveryLocation, last_frame_cached.pov, screen_position)) {
								// Calculate text width based on character count and font size
								std::string name_norm = "[DELIVERY TASK]" + (task_object_distance ? "[" + distance + "m]" : "");
								int text_width = name_norm.length() * 7; // Assume each character is approximately 6 pixels wide
								screen_position.x -= text_width / 2; // Shift the position left by half the text width

								if (goodPackage != 2) {  // Display only if not in good condition
									overlay->draw_text(screen_position, task_color, name_norm.c_str(), true);
								}
							}
						}
					}
				}
			}
		}
	}

	for (auto pizzushi : task_pizzushi_cache) {
		if (!pizzushi) continue;
		auto role = local_mec->get_player_role();

		if (role == 3 || role == 4) {
			if (task_object_esp) {
				auto pizzushi_tables_array = pizzushi->get_task_tables();
				auto pizzushi_tables = pizzushi_tables_array.list();

				if (task_pizzushi) {
					ImU32 task_color = ImGui::ColorConvertFloat4ToU32(task_pizzushi_color);

					for (auto table : pizzushi_tables) {
						if (!table) continue;
						auto tableRoot = table->get_root();
						if (!tableRoot) {
							std::cout << "pizzushi table root is null!" << std::endl;
							continue;
						}

						auto tableLocation = tableRoot->get_relative_location();
						auto requestState = table->get_request_state();
						auto tableFinished = table->get_finished();

						auto distance = CalculateDistance(local_mec->get_net_location(), tableLocation);
						double distanceDouble = std::stod(distance);

						if (distanceDouble <= esp_max_distance) {
							vector3 screen_position{};
							if (util::w2s(tableLocation, last_frame_cached.pov, screen_position)) {
								// Calculate text width based on character count and font size
								std::string name_norm = "[PIZZUSHI]" + (task_object_distance ? "[" + distance + "m]" : "");
								int text_width = name_norm.length() * 7; // Assume each character is approximately 6 pixels wide
								screen_position.x -= text_width / 2; // Shift the position left by half the text width

								if (tableFinished != true) {
									overlay->draw_text(screen_position, task_color, name_norm.c_str(), true);

									if (task_object_state) {
										auto tableRice = table->get_rice_type();
										auto tableFish = table->get_fish_type();
										auto tableTopping = table->get_topping_type();

										const char* rice_types[] = { "Invalid", "White Rice", "Brown Rice", "Black Rice" };
										const char* fish_types[] = { "Invalid", "Salmon", "Tuna", "Cod", "Shrimp" };
										const char* topping_types[] = { "Invalid", "Green", "Yellow", "Blue", "White", "Red" };

										std::string riceType = (tableRice >= 1 && tableRice <= 3) ? rice_types[tableRice] : "Invalid";
										std::string fishType = (tableFish >= 1 && tableFish <= 4) ? fish_types[tableFish] : "Invalid";
										std::string toppingType = (tableTopping >= 1 && tableTopping <= 5) ? topping_types[tableTopping] : "Invalid"; // fix credited to sinusbot

										screen_position.y += 15;
										overlay->draw_text(screen_position, task_color, ("[" + riceType + "|" + fishType + "|" + toppingType + "]").c_str(), true);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	for (auto computers : task_data_cache) {
		if (!computers) continue;
		auto role = local_mec->get_player_role();

		if (role == 3 || role == 4) {
			if (task_object_esp) {
				auto task_source_pcs = computers->get_task_source_pc();
				auto task_target_pcs = computers->get_task_target_pc();
				auto source_pcs = task_source_pcs.list();
				auto target_pcs = task_target_pcs.list();

				if (task_computers) {
					ImU32 task_color = ImGui::ColorConvertFloat4ToU32(task_computer_color);

					for (auto source : source_pcs) {
						if (!source) continue;
						auto sourceRoot = source->get_root();
						if (!sourceRoot) {
							std::cout << "source pc root is null!" << std::endl;
							continue;
						}

						auto sourceLocation = sourceRoot->get_relative_location();
						auto distance = CalculateDistance(local_mec->get_net_location(), sourceLocation);
						double distanceDouble = std::stod(distance);

						if (distanceDouble <= esp_max_distance) {
							vector3 screen_position{};
							if (util::w2s(sourceLocation, last_frame_cached.pov, screen_position)) {
								auto sourceState = source->get_state();
								if (sourceState != 3) {
									auto sourceRoom = source->get_room();

									// Calculate text width based on character count and font size
									std::string name_norm = "[SOURCE: " + TranslateRoomName(sourceRoom) + "]" + (task_object_distance ? "[" + distance + "m]" : "");
									int text_width = name_norm.length() * 7; // Assume each character is approximately 6 pixels wide
									screen_position.x -= text_width / 2; // Shift the position left by half the text width

									overlay->draw_text(screen_position, task_color, name_norm.c_str(), true);

									if (task_object_state) {
										auto targetPC = source->get_other_pc();
										screen_position.y += 15;
										overlay->draw_text(screen_position, task_color, ("[TARGET: " + TranslateRoomName(targetPC->get_room()) + "]").c_str(), true);

									}
								}
							}
						}
					}

					for (auto target : target_pcs) {
						if (!target) continue;
						auto targetRoot = target->get_root();
						if (!targetRoot) {
							std::cout << "target pc root is null!" << std::endl;
							continue;
						}

						auto targetLocation = targetRoot->get_relative_location();

						auto distance = CalculateDistance(local_mec->get_net_location(), targetLocation);
						double distanceDouble = std::stod(distance);

						if (distanceDouble <= esp_max_distance) {
							vector3 screen_position{};
							if (util::w2s(targetLocation, last_frame_cached.pov, screen_position)) {
								if (target->get_other_pc()->get_state() != 3) {
									auto targetRoom = target->get_room();

									// Calculate text width based on character count and font size
									std::string name_norm = "[TARGET: " + TranslateRoomName(targetRoom) + "]" + (task_object_distance ? "[" + distance + "m]" : "");
									int text_width = name_norm.length() * 7; // Assume each character is approximately 6 pixels wide
									screen_position.x -= text_width / 2; // Shift the position left by half the text width

									overlay->draw_text(screen_position, task_color, name_norm.c_str(), true);

									if (task_object_state) {
										auto sourcePC = target->get_other_pc();
										screen_position.y += 15;
										overlay->draw_text(screen_position, task_color, ("[SOURCE: " + TranslateRoomName(sourcePC->get_room()) + "]").c_str(), true);

									}
								}
							}
						}
					}
				}
			}
		}
	}

	for (auto scanner : task_scanner_cache) {
		if (!scanner) continue;
		auto role = local_mec->get_player_role();

		if (role == 3 || role == 4) {
			if (task_object_esp) {
				if (task_scanners) {
					auto scanner_machine = scanner->get_scanner_ref();
					auto scanner_machine_root = scanner_machine->get_root_component();
					if (!scanner_machine_root) continue;

					auto scanner_machine_loc = scanner_machine_root->get_relative_location();
					auto mec_root = local_mec->get_root_component();
					auto mec_loc = mec_root->get_relative_location();
					auto mec2machine_distance = CalculateDistanceMeters(mec_loc, scanner_machine_loc);

					if (mec2machine_distance < 0.5) {
						auto scanner_screen = scanner->get_screen_ref();
						auto scanner_targets_array = scanner_screen->get_targets();
						auto scanner_targets = scanner_targets_array.list();

						f_camera_cache last_frame_cached = local_camera_manager->get_cached_frame_private();
						vector3 camera_rotation = last_frame_cached.pov.rotation;

						static float expanding_radius = 20.0f; // Initial size of the circle
						static const float max_expansion_radius = 30.0f; // Maximum size for the expanding circle
						const float max_distance = 100.0f; // Define the max distance for the clamping logic

						// Get the nearest dot value
						double nearest_dot = scanner_screen->get_nearest_dot();

						// Calculate the expansion factor (the smaller the value, the larger the circle)
						float expansion_factor = std::clamp(static_cast<float>(nearest_dot) / max_distance, 0.0f, 1.0f);
						expanding_radius = max_expansion_radius * (1.0f - expansion_factor); // Inverse relationship with the dot distance

						// Draw the cyan circle as the border 
						ImVec2 screen_center = ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
						float circle_radius = max_expansion_radius; // Fixed size for the cyan circle

						// Draw the cyan border circle (just the outline)
						ImU32 scanner_color = ImGui::ColorConvertFloat4ToU32(task_scanner_color);
						overlay->draw_circle(screen_center, circle_radius, scanner_color, 64, 2.0f); // Draw border with thickness 2.0f

						// Draw the expanding circle inside the cyan border
						ImU32 expanding_circle_color = IM_COL32(255, 255, 255, 128); // Semi-transparent white for expanding circle
						overlay->draw_circle_filled(screen_center, expanding_radius, expanding_circle_color, 64);

						// Draw the text inside the circle
						std::ostringstream ss;
						ss << std::fixed << std::setprecision(2) << nearest_dot;
						std::string nearest_dot_text = "[" + ss.str() + "]";

						ImVec2 text_size = ImGui::CalcTextSize(nearest_dot_text.c_str());
						overlay->draw_text(ImVec2(screen_center.x - text_size.x / 2, screen_center.y - text_size.y / 2), scanner_color, nearest_dot_text.c_str(), true);
					}
				}
			}
		}
	}

	for (auto weapon_case : weapon_case_cache) {
		if (!weapon_case) continue;

		auto role = local_mec->get_player_role();
		if (role == 3 || role == 4) {
			if (weapon_case_esp) {
				// get case esp bool
				ImU32 case_color = ImGui::ColorConvertFloat4ToU32(weapon_case_color);

				auto weapon_case_code = weapon_case->get_target_values();
				auto case_code = weapon_case_code.list();
				auto case_timer = weapon_case->get_opening_timer().Handle;
				auto open_delay = weapon_case->get_open_delay();

				auto box_to_open = weapon_case->get_box_to_open();
				auto case_weapon = box_to_open->get_selected_weapon_qsdsf();
				auto case_open = box_to_open->get_case_open();
				//if (!case_weapon) continue;

				auto case_weapon_state = box_to_open->get_item_slot();
				auto case_weapon_ammo = case_weapon_state->get_item_state().Value_8;

				// Build the string
				std::string weapon_case_code_string;
				std::ostringstream oss;

				for (const auto& byte : case_code) {
					oss << static_cast<int>(byte); // Convert UINT8 to integer and append directly
				}

				weapon_case_code_string = oss.str(); // Extract the string

				auto caseRoot = weapon_case->get_root_component();
				if (!caseRoot) {
					std::cout << "weapon case root is null!" << std::endl;
					continue;
				}

				auto caseLocation = caseRoot->get_relative_location();
				auto selected_weapon = box_to_open->get_selected_weapon();
				std::unordered_map<int, std::string> weapon_map = {
					{1, "Empty"},
					{2, "Pistol"},
					{3, "Revolver"},
					{4, "Shorty"}
				};

				std::string case_weapon_name = weapon_map.count(selected_weapon) ? weapon_map[selected_weapon] : "Unknown";

				if (!case_open && selected_weapon != 1) {
					auto distance = CalculateDistance(local_mec->get_net_location(), caseLocation);
					double distanceDouble = std::stod(distance);

					if (distanceDouble <= esp_max_distance) {
						vector3 screen_position{};
						if (util::w2s(caseLocation, last_frame_cached.pov, screen_position)) {
							// Calculate text width based on character count and font size
							std::string name_norm = "[CASE]" + (weapon_case_distance ? "[" + distance + "m]" : "");
							int text_width = name_norm.length() * 7; // Assume each character is approximately 6 pixels wide
							screen_position.x -= text_width / 2; // Shift the position left by half the text width

							overlay->draw_text(screen_position, case_color, name_norm.c_str(), true);
							if (weapon_case_state) {
								screen_position.y += 15;
								std::string weapon_case_text = "[Code: " + weapon_case_code_string + "]";
								overlay->draw_text(screen_position, case_color, weapon_case_text.c_str(), true);
							}
							if (weapon_case_info) {
								screen_position.y += 15;
								std::string weapon_case_text = "[" + case_weapon_name + "|Ammo:" + std::to_string(case_weapon_ammo) + "]";
								overlay->draw_text(screen_position, case_color, weapon_case_text.c_str(), true);
							}
						}
					}
				}
			}
		}
	}
}

int main() {
	InitializeItems();
	PopulateUniqueItems();
	LoadConfig();

	overlay = new c_overlay();

	if (mem::attach("LOCKDOWN Protocol  ") != 0) {
		std::cout << ("open the game") << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(2));
		exit(1);
	}

	mem::module_base = mem::get_module_base("LockdownProtocol-Win64-Shipping.exe", mem::process_id);
	if (!mem::module_base) {
		std::cout << "module base invalid" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(2));
		exit(1);
	}

	overlay = new c_overlay();

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	if (!overlay->get_window_handle()) {
		std::cout << "open medal.tv" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(2));
		exit(1);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	overlay->bind_render_callback(render_callback);

	std::thread cache_thread(cache_useful);
	cache_thread.detach();

	/*
	uintptr_t baseAddress = mem::module_base;
	uintptr_t offset = 0x67DF760;

	uintptr_t absoluteAddress = baseAddress + offset;

	std::cout << "Absolute Address: 0x" << std::hex << absoluteAddress << std::endl;
		*/


	while (true) {
		overlay->msg_loop();
	}
}