#include <iostream>
#include <thread>
#include <string>
#include "overlay/overlay.h"
#include "mem.hpp"
#include "game_structures.hpp"
#include "util.hpp"

using namespace protocol::engine::sdk;
using namespace protocol::game::sdk;

static void render_callback() {
	std::this_thread::sleep_for(std::chrono::milliseconds(2));

	static bool menu_open = true;
	if (GetAsyncKeyState(VK_F1) & 1) {
		menu_open = !menu_open;
	}

	static bool player_esp = true;
	static bool weapon_esp = true;
	static bool primary_object_esp = true;
	static bool secondary_object_esp = true;
	static bool speedhack = false;
	static bool fast_melee = false;
	static bool max_damage = false;
	static bool infinite_stamina = false;
	static bool god_mode = false;
	static bool auto_fire = false;
	static bool rapid_fire = false;
	static bool no_recoil = true;
	static bool infinite_melee_range = false;
	static bool living_state = false;


	if (GetAsyncKeyState(VK_F2) & 1) {
		player_esp = !player_esp;
	}
	if (GetAsyncKeyState(VK_F3) & 1) {
		weapon_esp = !weapon_esp;
	}
	if (GetAsyncKeyState(VK_F4) & 1) {
		primary_object_esp = !primary_object_esp;
	}
	if (GetAsyncKeyState(VK_F5) & 1) {
		secondary_object_esp = !secondary_object_esp;
	}
	if (GetAsyncKeyState(VK_F6) & 1) {
		speedhack = !speedhack;
	}
	if (GetAsyncKeyState(VK_F7) & 1) {
		god_mode = !god_mode;
	}
	if (GetAsyncKeyState(VK_F8) & 1) {
		infinite_stamina = !infinite_stamina;
	}
	if (GetAsyncKeyState(VK_F9) & 1) {
		living_state = !living_state;
	}

	if (menu_open) {
		ImGui::Begin("Hawk Tuah Protocol");
		ImGui::Text("[         ESP          ]");
		ImGui::Checkbox("[F2] Player ESP", &player_esp);
		ImGui::Checkbox("[F3] Weapon ESP", &weapon_esp);
		ImGui::Checkbox("[F4] Primary Object ESP", &primary_object_esp);
		ImGui::Checkbox("[F5] Other Object ESP", &secondary_object_esp);
		ImGui::Text("[        PLAYER        ]");
		ImGui::Checkbox("[F6] Speedhack", &speedhack);
		ImGui::Checkbox("[F7] God Mode", &god_mode);
		ImGui::Checkbox("[F8] Infinite Stamina", &infinite_stamina);
		ImGui::Checkbox("[F9] Revive", &living_state); 
		ImGui::Text("[        WEAPON        ]");
		ImGui::Checkbox("Fast Melee", &fast_melee);
		ImGui::Checkbox("Infinite Melee Range", &infinite_melee_range);
		ImGui::Text("------------------------");
		ImGui::Checkbox("Auto Fire", &auto_fire);
		ImGui::Checkbox("Rapid Fire", &rapid_fire);
		ImGui::Checkbox("No Recoil", &no_recoil);
		ImGui::Checkbox("Max Damage", &max_damage);
		ImGui::Text("[       ONI EDIT       ]");		
		ImGui::End();
	}

	u_world* gworld = u_world::get_world(mem::module_base);
	if (!gworld) return;
	a_game_state_base* game_state = gworld->get_game_state();
	if (!game_state) return;
	u_game_instance* owning_instance = gworld->get_owning_game_instance();
	if (!owning_instance) return;
	u_localplayer* local_player = owning_instance->get_localplayer();
	if (!local_player) return;
	a_player_controller* local_controller = local_player->get_player_controller();
	if (!local_controller) return;
	a_player_camera_manager* local_camera_manager = local_controller->get_camera_manager();
	if (!local_camera_manager) return;
	f_camera_cache last_frame_cached = local_camera_manager->get_cached_frame_private();
	auto players = game_state->player_array();

	auto local_mec = (mec_pawn*)local_controller->get_pawn();

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

	if (speedhack) {
		local_mec->set_acceleration(vector2(9999.0, 9999.0));
		local_mec->set_max_speed(2000.0);
		local_mec->set_friction(100000);
	}
	
	if (fast_melee || infinite_melee_range) {
		auto hand_item = (u_data_melee*)local_mec->get_hand_item();
		if (util::get_name_from_fname(hand_item->class_private()->fname_index()).find("Data_Melee_C") != std::string::npos) {
			auto mtype = hand_item->get_melee_type();
			if (fast_melee) {
				mtype->set_cast_time(0.05);
				mtype->set_recover_time(0.05);
				mtype->set_stun(0.0);
			}
			if (infinite_melee_range)
				mtype->set_range(10000);
		}
	}

	auto levels = gworld->get_levels();
	for (auto level : levels.list()) {
		auto actors = level->get_actors();
		for (auto actor : actors.list()) {
			auto class_name = util::get_name_from_fname(actor->class_private()->fname_index());
			auto name = util::get_name_from_fname(actor->outer()->fname_index());
			if (player_esp) {
				if (class_name.find("Mec_C") != std::string::npos) {
					auto mec = (mec_pawn*)(actor);
					auto state = mec->player_state();
					if (!state) continue;
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
					}
				}
			}
			if (class_name.find("WorldItem_C") != std::string::npos) {
				world_item* item = (world_item*)actor;
				if (!item) continue;

				auto data = item->get_data();
				auto item_name = data->get_name().read_string();

				if (item_name == "PISTOL" || item_name == "REVOLVER" || item_name == "SHORTY" || item_name == "SMG" || item_name == "RIFLE" || item_name == "SHOTGUN") {
					auto gun_data = (u_data_gun*)data;
					auto item_root = item->get_root_component();
					if (!item_root) continue;
					
					if (auto_fire)
						gun_data->set_auto_fire(true);
					if (rapid_fire)
						gun_data->set_fire_rate(0.1);
					if (max_damage)
						gun_data->set_damage(10000);
					
					if (no_recoil) {
						gun_data->set_shake_intensity(0.0);

						gun_data->set_oscillation_reactivity(0.0);
						gun_data->set_walk_oscillation(0.0);
						gun_data->set_run_oscillation(0.0);
						gun_data->set_run_precision(0.0);
						gun_data->set_stand_oscillation(0.0);

						gun_data->set_recoil_reactivity(0.0);
						gun_data->set_walk_precision(0.0);
						gun_data->set_air_precision(0.0);
						gun_data->set_run_precision(0.0);
					}

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
							}
							else if (item_name == "VENT FILTER") {
								overlay->draw_text(screen_position, IM_COL32(65, 115, 217, 255), "[VENT FILTER]", true); // Light Blue
							}
							else if (item_name == "RICE") {
								overlay->draw_text(screen_position, IM_COL32(201, 169, 122, 255), "[RICE]", true); // Tan-ish
							}
							else if (item_name == "PACKAGE") {
								overlay->draw_text(screen_position, IM_COL32(87, 47, 24, 255), "[PACKAGE]", true); // Brown
							}
							else if (item_name == "SAMPLE") {
								overlay->draw_text(screen_position, IM_COL32(92, 6, 191, 255), "[SAMPLE]", true); // Purple
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
							}
							else if (item_name == "BATTERY") {
								overlay->draw_text(screen_position, IM_COL32(189, 189, 189, 255), "[BATTERY]", true); // Light Grey
							}
							else if (item_name == "SCREW DRIVER") {
								overlay->draw_text(screen_position, IM_COL32(255, 255, 255, 255), "[SCREW DRIVER]", true); // Tan-ish
							}
							else if (item_name == "CONTAINER") {
								overlay->draw_text(screen_position, IM_COL32(136, 0, 145, 255), "[CONTAINER]", true); // Pink-sih purple
							}
						}
					}
				}
			}
		}
	}

}


int main()
{
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

	while (true) {
		overlay->msg_loop();
	}
}