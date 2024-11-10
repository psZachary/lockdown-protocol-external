#include <iostream>
#include <thread>
#include <string>
#include "overlay/overlay.h"
#include "mem.hpp"
#include "game_structures.hpp"
#include "util.hpp"
#include "config.h"
#include "menu.h"
#include "globals.h"
#include "ItemProperties.h"
#include <cmath>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include "data_cache.h"

using namespace menu;
using namespace config;
using namespace globals;
using namespace protocol::engine::sdk;
using namespace protocol::game::sdk;

std::unordered_map<std::string, ItemProperties> itemData;

void InitializeItems() {
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
				if (class_name.find("Mec_C") != std::string::npos) {
					temp_player_cache.push_back((mec_pawn*)actor);
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

	f_camera_cache last_frame_cached = local_camera_manager->get_cached_frame_private();
	auto players = game_state->player_array();

	if (GetAsyncKeyState(config::esp_hotkey) & 1) {
		esp_enabled = !esp_enabled;
		player_esp = esp_enabled;
		weapon_esp = esp_enabled;
		task_object_esp = esp_enabled;
		primary_object_esp = esp_enabled;
		secondary_object_esp = esp_enabled;
		//std::cout << "Fire Spread:" << local_mec->get_fire_spread() << std::endl;
	}
	if (GetAsyncKeyState(config::speedhack_hotkey) & 1) {
		speedhack = !speedhack;
	}
	if (GetAsyncKeyState(config::god_mode_hotkey) & 1) {
		god_mode = !god_mode;
	}
	if (GetAsyncKeyState(config::infinite_stamina_hotkey) & 1) {
		infinite_stamina = !infinite_stamina;
	}
	if (GetAsyncKeyState(config::fast_melee_hotkey) & 1) {
		fast_melee = !fast_melee;
	}
	if (GetAsyncKeyState(config::infinite_melee_range_hotkey) & 1) {
		infinite_melee_range = !infinite_melee_range;
	}
	if (GetAsyncKeyState(config::auto_fire_hotkey) & 1) {
		auto_fire = !auto_fire;
	}
	if (GetAsyncKeyState(config::rapid_fire_hotkey) & 1) {
		rapid_fire = !rapid_fire;
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

	if (infinite_ammo) {
		if (hand_item) {
			auto item_name = hand_item->get_name().read_string();
			if (item_name == "SHORTY" || item_name == "PISTOL" || item_name == "REVOLVER" || item_name == "SHOTGUN" || item_name == "RIFLE" || item_name == "SMG") {
				auto hand_state = local_mec->get_hand_state();
				hand_state.Value_8 = 99;
				local_mec->set_hand_state(hand_state);
			}
			else {
				std::cout << "Error: " << item_name << " is not a valid gun." << std::endl;
				infinite_ammo = !infinite_ammo;
			}
		}
	}

	if (living_state) {
		local_mec->set_alive(true);
		local_mec->set_health(100);
		living_state = !living_state;
	}

	if (infinite_stamina) {
		local_mec->set_stamina(1.);
	}

	if (god_mode) {
		local_mec->set_health(100);
	}

	//static double fric = local_mec->get_friction();
	//std::cout << *reinterpret_cast<std::uint64_t*>(&fric);
	if (speedhack) {
		local_mec->set_acceleration(vector2(9999.0, 9999.0));
		local_mec->set_max_speed(2000.0);
		local_mec->set_friction(100000);
	}
	else {
		local_mec->set_acceleration(vector2(100.0, 100.0));
		local_mec->set_max_speed(800.0);
		local_mec->set_friction(0);
	}

	if (hand_item) {
		if (fast_melee || infinite_melee_range) {
			if (util::get_name_from_fname(hand_item->class_private()->fname_index()).find("Data_Melee_C") != std::string::npos) {
				auto mtype = melee_item_data->get_melee_type();

				if (fast_melee) {
					mtype->set_cast_time(0.05);
					mtype->set_recover_time(0.05);
					mtype->set_stun(0.0);
					mtype->set_cost(0);
				}
				if (infinite_melee_range) {
					mtype->set_range(10000);
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
					gun_data->set_fire_rate(0.02);
				}
				if (no_recoil) {
					local_mec->set_fire_spread(0.0);

					gun_data->set_shake_intensity(0.0);

					gun_data->set_oscillation_reactivity(0.0);
					gun_data->set_walk_oscillation(0.0);
					gun_data->set_run_oscillation(0.0);
					gun_data->set_stand_oscillation(0.0);

					gun_data->set_recoil_reactivity(0.0);
					gun_data->set_walk_precision(0.0);
					gun_data->set_air_precision(0.0);
					gun_data->set_run_precision(0.0);
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
				auto role = mec->get_player_role();

				auto mec_root = mec->get_root_component();
				if (!mec_root) continue;

				auto position = mec_root->get_relative_location();
				vector3 screen_position{};
				if (util::w2s(position, last_frame_cached.pov, screen_position)) {
					auto color = role == 4 ? IM_COL32(255, 0, 0, 255) : IM_COL32(0, 255, 0, 255);
					auto name_norm = "[" + name + "]" + (role == 4 ? " [D]" : "");

					overlay->draw_text(screen_position, color, name_norm.c_str(), true);

					if (player_distance) {
						screen_position.y += 15;
						auto distance = CalculateDistance(local_mec->get_net_location(), position);
						overlay->draw_text(screen_position, color, ("[" + distance + "m]").c_str(), true);
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

		if (item_name == "PISTOL" || item_name == "REVOLVER" || item_name == "SHORTY" || item_name == "SMG" || item_name == "RIFLE" || item_name == "SHOTGUN") {
			auto gun_data = (u_data_gun*)data;
			auto item_root = item->get_root_component();
			if (!item_root) continue;

			if (weapon_esp) {
				auto position = item_root->get_relative_location();
				vector3 screen_position{};
				if (util::w2s(position, last_frame_cached.pov, screen_position)) {
					if (item_name == "PISTOL") {
						overlay->draw_text(screen_position, IM_COL32(250, 92, 0, 255), "[PISTOL]", true); // Orange
					}
					else if (item_name == "REVOLVER") {
						overlay->draw_text(screen_position, IM_COL32(250, 92, 0, 255), "[REVOLVER]", true);
					}
					else if (item_name == "SHORTY") {
						overlay->draw_text(screen_position, IM_COL32(250, 92, 0, 255), "[SHORTY]", true);
					}
					else if (item_name == "SMG") {
						overlay->draw_text(screen_position, IM_COL32(250, 92, 0, 255), "[SMG]", true);
					}
					else if (item_name == "RIFLE") {
						overlay->draw_text(screen_position, IM_COL32(250, 92, 0, 255), "[RIFLE]", true);
					}
					else if (item_name == "SHOTGUN") {
						overlay->draw_text(screen_position, IM_COL32(250, 92, 0, 255), "[SHOTGUN]", true);
					}
					if (weapon_item_state) {
						screen_position.y += 15;
						overlay->draw_text(screen_position, IM_COL32(250, 92, 0, 255), ("[Ammo: " + std::to_string(item_value) + "]").c_str(), true);
					}
					if (weapon_distance) {
						screen_position.y += 15;
						auto distance = CalculateDistance(local_mec->get_net_location(), position);
						overlay->draw_text(screen_position, IM_COL32(250, 92, 0, 255), ("[" + distance + "m]").c_str(), true);
					}
				}
			}
		}

		if (item_name == "KNIFE" || item_name == "C4" || item_name == "DETONATOR") {
			auto item_root = item->get_root_component();
			if (!item_root) continue;

			if (weapon_esp) {
				auto position = item_root->get_relative_location();
				vector3 screen_position{};

				if (util::w2s(position, last_frame_cached.pov, screen_position)) {
					if (item_name == "KNIFE") {
						overlay->draw_text(screen_position, IM_COL32(250, 92, 0, 255), "[KNIFE]", true); // Orange
					}
					else if (item_name == "C4") {
						overlay->draw_text(screen_position, IM_COL32(250, 92, 0, 255), "[C4]", true);
					}
					else if (item_name == "DETONATOR") {
						overlay->draw_text(screen_position, IM_COL32(250, 92, 0, 255), "[DETONATOR]", true);
					}
					if (weapon_distance) {
						screen_position.y += 15;
						auto distance = CalculateDistance(local_mec->get_net_location(), position);
						overlay->draw_text(screen_position, IM_COL32(250, 92, 0, 255), ("[" + distance + "m]").c_str(), true);
					}
				}
			}
		}

		if (item_name == "GAZ BOTTLE" || item_name == "VENT FILTER" || item_name == "RICE" || item_name == "PACKAGE" || item_name == "SAMPLE") {
			auto item_root = item->get_root_component();
			if (!item_root) continue;

			if (primary_object_esp) {
				auto position = item_root->get_relative_location();
				vector3 screen_position{};

				if (util::w2s(position, last_frame_cached.pov, screen_position)) {
					if (item_name == "GAZ BOTTLE") {
						overlay->draw_text(screen_position, IM_COL32(0, 83, 250, 255), "[GAZ BOTTLE]", true); // Blue

						if (primary_item_state) {
							if (item_value == 0) {
								screen_position.y += 15;
								overlay->draw_text(screen_position, IM_COL32(0, 83, 250, 255), "[Color: Yellow]", true);
							}
							else if (item_value == 1) {
								screen_position.y += 15;
								overlay->draw_text(screen_position, IM_COL32(0, 83, 250, 255), "[Color: Red]", true);
							}
							else if (item_value == 2) {
								screen_position.y += 15;
								overlay->draw_text(screen_position, IM_COL32(0, 83, 250, 255), "[Color: Blue]", true);
							}
						}
						if (primary_distance) {
							screen_position.y += 15;
							auto distance = CalculateDistance(local_mec->get_net_location(), position);
							overlay->draw_text(screen_position, IM_COL32(0, 83, 250, 255), ("[" + distance + "m]").c_str(), true);
						}
					}
					else if (item_name == "VENT FILTER") {
						overlay->draw_text(screen_position, IM_COL32(65, 115, 217, 255), "[VENT FILTER]", true); // Light Blue

						if (primary_item_state) {
							screen_position.y += 15;
							overlay->draw_text(screen_position, IM_COL32(65, 115, 217, 255), ("[Clean: " + std::to_string(item_value) + "%]").c_str(), true);
						}
						if (primary_distance) {
							screen_position.y += 15;
							auto distance = CalculateDistance(local_mec->get_net_location(), position);
							overlay->draw_text(screen_position, IM_COL32(65, 115, 217, 255), ("[" + distance + "m]").c_str(), true);
						}
					}
					else if (item_name == "RICE") {
						if (item_value == 1) {
							overlay->draw_text(screen_position, IM_COL32(201, 169, 122, 255), "[WHITE RICE]", true);
						}
						else if (item_value == 2) {
							overlay->draw_text(screen_position, IM_COL32(201, 169, 122, 255), "[BROWN RICE]", true);
						}
						else if (item_value == 3) {
							overlay->draw_text(screen_position, IM_COL32(201, 169, 122, 255), "[BLACK RICE]", true);
						}
						if (primary_distance) {
							screen_position.y += 15;
							auto distance = CalculateDistance(local_mec->get_net_location(), position);
							overlay->draw_text(screen_position, IM_COL32(201, 169, 122, 255), ("[" + distance + "m]").c_str(), true);
						}
					}
					else if (item_name == "PACKAGE") {
						overlay->draw_text(screen_position, IM_COL32(87, 47, 24, 255), "[PACKAGE]", true); // Brown

						if (primary_item_state) {
							if (item_value == 1) {
								screen_position.y += 15;
								overlay->draw_text(screen_position, IM_COL32(87, 47, 24, 255), "[Security]", true);
							}
							else if (item_value == 2) {
								screen_position.y += 15;
								overlay->draw_text(screen_position, IM_COL32(87, 47, 24, 255), "[Computers]", true);
							}
							else if (item_value == 3) {
								screen_position.y += 15;
								overlay->draw_text(screen_position, IM_COL32(87, 47, 24, 255), "[Botanic]", true);
							}
							else if (item_value == 4) {
								screen_position.y += 15;
								overlay->draw_text(screen_position, IM_COL32(87, 47, 24, 255), "[Restaurant]", true);
							}
							else if (item_value == 5) {
								screen_position.y += 15;
								overlay->draw_text(screen_position, IM_COL32(87, 47, 24, 255), "[Medical]", true);
							}
							else if (item_value == 6) {
								screen_position.y += 15;
								overlay->draw_text(screen_position, IM_COL32(87, 47, 24, 255), "[Tutorial]", true);
							}
							else if (item_value == 7) {
								screen_position.y += 15;
								overlay->draw_text(screen_position, IM_COL32(87, 47, 24, 255), "[Machine]", true);
							}
						}
						if (primary_distance) {
							screen_position.y += 15;
							auto distance = CalculateDistance(local_mec->get_net_location(), position);
							overlay->draw_text(screen_position, IM_COL32(87, 47, 24, 255), ("[" + distance + "m]").c_str(), true);
						}
					}
					else if (item_name == "SAMPLE") {
						if (item_value == 1) {
							overlay->draw_text(screen_position, IM_COL32(92, 6, 191, 255), "[GREEN SAMPLE]", true);
						}
						else if (item_value == 2) {
							overlay->draw_text(screen_position, IM_COL32(92, 6, 191, 255), "[YELLOW SAMPLE]", true);
						}
						else if (item_value == 3) {
							overlay->draw_text(screen_position, IM_COL32(92, 6, 191, 255), "[BLUE SAMPLE]", true);
						}
						else if (item_value == 4) {
							overlay->draw_text(screen_position, IM_COL32(92, 6, 191, 255), "[WHITE SAMPLE]", true);
						}
						else if (item_value == 5) {
							overlay->draw_text(screen_position, IM_COL32(92, 6, 191, 255), "[RED SAMPLE]", true);
						}
						if (primary_distance) {
							screen_position.y += 15;
							auto distance = CalculateDistance(local_mec->get_net_location(), position);
							overlay->draw_text(screen_position, IM_COL32(92, 6, 191, 255), ("[" + distance + "m]").c_str(), true);
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
				vector3 screen_position{};

				if (util::w2s(position, last_frame_cached.pov, screen_position)) {
					if (item_name == "FUSE") {
						overlay->draw_text(screen_position, IM_COL32(105, 105, 105, 255), "[FUSE]", true); // Grey

						if (secondary_item_state) {
							const char* color_names[] = { "INVALID", "RED", "YELLOW", "BLUE" };

							int color_index_value = (item_value >= 1 && item_value <= 3) ? item_value : 0;
							int color_index_time = (item_time >= 1 && item_time <= 3) ? item_time : 0;

							std::string fuse_info = "[" + std::string(color_names[color_index_value]) + " | " + std::string(color_names[color_index_time]) + "]";

							screen_position.y += 15;
							overlay->draw_text(screen_position, IM_COL32(105, 105, 105, 255), fuse_info.c_str(), true);
						}
						if (secondary_distance) {
							screen_position.y += 15;
							auto distance = CalculateDistance(local_mec->get_net_location(), position);
							overlay->draw_text(screen_position, IM_COL32(105, 105, 105, 255), ("[" + distance + "m]").c_str(), true);
						}
					}
					else if (item_name == "BATTERY") {
						overlay->draw_text(screen_position, IM_COL32(189, 189, 189, 255), "[BATTERY]", true); // Light Grey

						if (secondary_item_state) {
							screen_position.y += 15;
							overlay->draw_text(screen_position, IM_COL32(189, 189, 189, 255), ("[Charge: " + std::to_string(item_value) + "%]").c_str(), true);
						}
						if (secondary_distance) {
							screen_position.y += 15;
							auto distance = CalculateDistance(local_mec->get_net_location(), position);
							overlay->draw_text(screen_position, IM_COL32(189, 189, 189, 255), ("[" + distance + "m]").c_str(), true);
						}
					}
					else if (item_name == "SCREW DRIVER") {
						overlay->draw_text(screen_position, IM_COL32(255, 255, 255, 255), "[SCREW DRIVER]", true); // Tan-ish
						if (secondary_distance) {
							screen_position.y += 15;
							auto distance = CalculateDistance(local_mec->get_net_location(), position);
							overlay->draw_text(screen_position, IM_COL32(255, 255, 255, 255), ("[" + distance + "m]").c_str(), true);
						}
					}
					else if (item_name == "CONTAINER") {
						overlay->draw_text(screen_position, IM_COL32(136, 0, 145, 255), "[CONTAINER]", true); // Pink-sih purple

						if (secondary_item_state) {
							screen_position.y += 15;
							if (item_value == -1) {
								overlay->draw_text(screen_position, IM_COL32(136, 0, 145, 255), "[Dirty]", true);
							}
							if (item_value == 0) {
								overlay->draw_text(screen_position, IM_COL32(136, 0, 145, 255), "[Clean]", true);
							}
							else if (item_value == 1) {
								overlay->draw_text(screen_position, IM_COL32(136, 0, 145, 255), "[Green]", true);
							}
							else if (item_value == 2) {
								overlay->draw_text(screen_position, IM_COL32(136, 0, 145, 255), "[Yellow]", true);
							}
							else if (item_value == 3) {
								overlay->draw_text(screen_position, IM_COL32(136, 0, 145, 255), "[Blue", true);
							}
							else if (item_value == 4) {
								overlay->draw_text(screen_position, IM_COL32(136, 0, 145, 255), "[White]", true);
							}
							else if (item_value == 5) {
								overlay->draw_text(screen_position, IM_COL32(136, 0, 145, 255), "[Red]", true);
							}
							else if (item_value == 6) {
								if (item_time == 1) {
									overlay->draw_text(screen_position, IM_COL32(136, 0, 145, 255), "[White Rice]", true);
								}
								else if (item_time == 2) {
									overlay->draw_text(screen_position, IM_COL32(136, 0, 145, 255), "[Brown Rice]", true);
								}
								else if (item_time == 3) {
									overlay->draw_text(screen_position, IM_COL32(136, 0, 145, 255), "[Black Rice]", true);
								}
							}
						}
						if (secondary_distance) {
							screen_position.y += 15;
							auto distance = CalculateDistance(local_mec->get_net_location(), position);
							overlay->draw_text(screen_position, IM_COL32(136, 0, 145, 255), ("[" + distance + "m]").c_str(), true);
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
							vector3 screen_position{};
							auto filterState = filter->get_item_state();
							auto ventLocation = ventRoot->get_relative_location();

							if (util::w2s(ventLocation, last_frame_cached.pov, screen_position)) {
								if (filterState.Value_8 != 100) {
									overlay->draw_text(screen_position, IM_COL32(255, 255, 0, 255), "[VENT TASK]", true);

									// Clean percentage display
									if (task_object_state) {
										screen_position.y += 15;
										overlay->draw_text(screen_position, IM_COL32(255, 255, 0, 255), ("[Clean: " + std::to_string(filterState.Value_8) + "%]").c_str(), true);
									}

									// Distance calculation
									if (task_object_distance) {
										screen_position.y += 15;
										auto distance = CalculateDistance(local_mec->get_net_location(), ventLocation);
										overlay->draw_text(screen_position, IM_COL32(255, 255, 0, 255), ("[" + distance + "m]").c_str(), true);
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
								vector3 screen_position{};
								auto bottleLocation = bottleRoot->get_relative_location();

								if (util::w2s(bottleLocation, last_frame_cached.pov, screen_position)) {
									overlay->draw_text(screen_position, IM_COL32(255, 255, 0, 255), "[BOTTLE]", true);

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
										overlay->draw_text(screen_position, IM_COL32(255, 255, 0, 255), text.c_str(), true);
									}

									// Distance calculation
									if (task_object_distance) {
										screen_position.y += 15;
										auto distance = CalculateDistance(local_mec->get_net_location(), bottleLocation);
										overlay->draw_text(screen_position, IM_COL32(255, 255, 0, 255), ("[" + distance + "m]").c_str(), true);
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

						vector3 screen_position{};
						auto alimLocation = alimRoot->get_relative_location();
						alimLocation.z += 200;
						alimLocation.x -= 25;

						if (util::w2s(alimLocation, last_frame_cached.pov, screen_position)) {
							if (alimValue != 100) {
								overlay->draw_text(screen_position, IM_COL32(255, 255, 0, 255), "[ALIMENTATION]", true);

								if (task_object_state) {
									screen_position.y += 15;
									overlay->draw_text(screen_position, IM_COL32(255, 255, 0, 255), ("[Battery: " + std::to_string(alimBatteryLevel) + "%]").c_str(), true);

									// Define the colors corresponding to each value
									const char* fuse_colors[] = { "Invalid", "Red", "Yellow", "Blue" };

									// Ensure that alimFuseIn and alimFuseOut are within the valid range
									std::string fuse_in_color = (alimFuseIn >= 1 && alimFuseIn <= 3) ? fuse_colors[alimFuseIn] : "Invalid";
									std::string fuse_out_color = (alimFuseOut >= 1 && alimFuseOut <= 3) ? fuse_colors[alimFuseOut] : "Invalid";

									screen_position.y += 15;
									overlay->draw_text(screen_position, IM_COL32(255, 255, 0, 255), ("[" + fuse_in_color + " | " + fuse_out_color + "]").c_str(), true);
								}
								// Distance calculation
								if (task_object_distance) {
									screen_position.y += 15;
									auto distance = CalculateDistance(local_mec->get_net_location(), alimLocation);
									overlay->draw_text(screen_position, IM_COL32(255, 255, 0, 255), ("[" + distance + "m]").c_str(), true);
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
					for (auto deliveryCase : task_cases) {
						if (!deliveryCase) continue;
						auto caseRoot = deliveryCase->get_root();
						if (!caseRoot) {
							std::cout << "delivery case root is null!" << std::endl;
							continue;
						}

						vector3 screen_position{};
						auto deliveryLocation = caseRoot->get_relative_location();
						auto goodPackage = deliveryCase->get_good_package();

						if (util::w2s(deliveryLocation, last_frame_cached.pov, screen_position)) {
							if (goodPackage != 2) {  // Display only if not in good condition
								overlay->draw_text(screen_position, IM_COL32(255, 255, 0, 255), "[DELIVERY TASK]", true);

								// Distance calculation
								if (task_object_distance) {
									screen_position.y += 15;
									auto distance = CalculateDistance(local_mec->get_net_location(), deliveryLocation);
									overlay->draw_text(screen_position, IM_COL32(255, 255, 0, 255), ("[" + distance + "m]").c_str(), true);
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
					for (auto table : pizzushi_tables) {
						if (!table) continue;
						auto tableRoot = table->get_root();
						if (!tableRoot) {
							std::cout << "pizzushi table root is null!" << std::endl;
							continue;
						}

						vector3 screen_position{};
						auto tableLocation = tableRoot->get_relative_location();
						auto requestState = table->get_request_state();
						auto tableFinished = table->get_finished();

						if (util::w2s(tableLocation, last_frame_cached.pov, screen_position)) {
							if (tableFinished != true) {
								overlay->draw_text(screen_position, IM_COL32(255, 255, 0, 255), "[PIZZUSHI]", true);

								if (task_object_state) {
									auto tableRice = table->get_rice_type();
									auto tableFish = table->get_fish_type();
									auto tableTopping = table->get_topping_type();

									const char* rice_types[] = { "Invalid", "White Rice", "Brown Rice", "Black Rice" };
									const char* fish_types[] = { "Invalid", "Salmon", "Tuna", "Cod", "Shrimp" };
									const char* topping_types[] = { "Invalid", "Green", "Yellow", "Blue", "White", "Red" };

									std::string riceType = (tableRice >= 1 && tableRice <= 3) ? rice_types[tableRice] : "Invalid";
									std::string fishType = (tableFish >= 1 && tableFish <= 4) ? fish_types[tableFish] : "Invalid";
									std::string toppingType = (tableTopping >= 1 && tableTopping <= 4) ? topping_types[tableTopping] : "Invalid";

									screen_position.y += 15;
									overlay->draw_text(screen_position, IM_COL32(255, 255, 0, 255), ("[" + riceType + " | " + fishType + " | " + toppingType + "]").c_str(), true);
								}
								// Distance calculation
								if (task_object_distance) {
									screen_position.y += 15;
									auto distance = CalculateDistance(local_mec->get_net_location(), tableLocation);
									overlay->draw_text(screen_position, IM_COL32(255, 255, 0, 255), ("[" + distance + "m]").c_str(), true);
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
					for (auto source : source_pcs) {
						if (!source) continue;
						auto sourceRoot = source->get_root();
						if (!sourceRoot) {
							std::cout << "source pc root is null!" << std::endl;
							continue;
						}

						vector3 screen_position{};
						auto sourceLocation = sourceRoot->get_relative_location();
						if (util::w2s(sourceLocation, last_frame_cached.pov, screen_position)) {
							auto sourceState = source->get_state();
							if (sourceState != 3) {
								auto sourceRoom = source->get_room();

								overlay->draw_text(screen_position, IM_COL32(255, 255, 0, 255), ("[Source: " + TranslateRoomName(sourceRoom) + "]").c_str(), true);

								if (task_object_state) {
									auto targetPC = source->get_other_pc();
									screen_position.y += 15;
									overlay->draw_text(screen_position, IM_COL32(255, 255, 0, 255), ("[Target: " + TranslateRoomName(targetPC->get_room()) + "]").c_str(), true);

								}
								// Distance calculation
								if (task_object_distance) {
									screen_position.y += 15;
									auto distance = CalculateDistance(local_mec->get_net_location(), sourceLocation);
									overlay->draw_text(screen_position, IM_COL32(255, 255, 0, 255), ("[" + distance + "m]").c_str(), true);
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

						vector3 screen_position{};
						auto targetLocation = targetRoot->get_relative_location();
						if (util::w2s(targetLocation, last_frame_cached.pov, screen_position)) {
							if (target->get_other_pc()->get_state() != 3) {
								auto targetRoom = target->get_room();

								overlay->draw_text(screen_position, IM_COL32(255, 255, 0, 255), ("[Target: " + TranslateRoomName(targetRoom) + "]").c_str(), true);

								if (task_object_state) {
									auto sourcePC = target->get_other_pc();
									screen_position.y += 15;
									overlay->draw_text(screen_position, IM_COL32(255, 255, 0, 255), ("[Source: " + TranslateRoomName(sourcePC->get_room()) + "]").c_str(), true);

								}
								// Distance calculation
								if (task_object_distance) {
									screen_position.y += 15;
									auto distance = CalculateDistance(local_mec->get_net_location(), targetLocation);
									overlay->draw_text(screen_position, IM_COL32(255, 255, 0, 255), ("[" + distance + "m]").c_str(), true);
								}
							}
						}
					}
				}
			}
		}
	}


	/*
	for (auto scanner : task_scanner_cache) {
		if (!scanner) continue;
		auto role = local_mec->get_player_role();

		if (role == 3 || role == 4) {
			if (task_object_esp) {
				auto task_scanner = scanner->get_scanner_ref();
				std::cout << "scanner ref found!" << std::endl;
				auto task_mecs_array = task_scanner->get_mecs();
				std::cout << "scanner mec array found!" << std::endl;
				auto task_mecs = task_mecs_array.list();
				std::cout << "scanner mecs put into array!" << std::endl;
				if (task_scan) {
					for (auto scanner_mec : task_mecs) {
						std::cout << "scanner mec loop!" << std::endl;
						if (!scanner_mec) continue;
						auto scannerRoot = scanner_mec->get_root_component();
						if (!scannerRoot) {
							std::cout << "scanner root is null!" << std::endl;
							continue;
						}
						std::cout << "scanner root found!" << std::endl;
						vector3 screen_position{};
						FVector scannerLocation = scanner_mec->get_net_location();
						vector3 scannerLocationVec3{ scannerLocation.X, scannerLocation.Y, scannerLocation.Z };
						std::cout << "scanner location:" << scannerLocationVec3.x << "| " << scannerLocationVec3.y << " | " << scannerLocationVec3.z << std::endl;
						if (util::w2s(scannerLocationVec3, last_frame_cached.pov, screen_position)) {
							overlay->draw_text(screen_position, IM_COL32(255, 255, 0, 255), "[SCANNER?]", true);
							std::cout << "scanner pos cast found!" << std::endl;
						}
					}
				}
			}
		}
	}
	*/
}

int main() {
	InitializeItems();
	PopulateUniqueItems();

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

	while (true) {
		overlay->msg_loop();
	}
}