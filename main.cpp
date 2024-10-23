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

	static bool esp_enabled = true;
	static bool gun_esp_enabled = true;
	static bool speedhack = false;
	static bool fast_melee = true;
	static bool max_damage = true;
	static bool infinite_stamina = true;
	static bool auto_fire = true;
	static bool rapid_fire = true;
	static bool no_recoil = true;
	static bool infinite_melee_range = true;

	if (menu_open) {
		ImGui::Begin("Hawk Tuah Protocol");

		ImGui::Checkbox("Player ESP", &esp_enabled);
		ImGui::Checkbox("Gun ESP", &gun_esp_enabled);
		ImGui::Checkbox("Infinite Stamina", &infinite_stamina);
		ImGui::Checkbox("Speedhack", &speedhack);
		ImGui::Checkbox("Fast Melee", &fast_melee);
		ImGui::Checkbox("Infinite Melee Range", &infinite_melee_range);
		ImGui::Checkbox("Auto Fire", &auto_fire);
		ImGui::Checkbox("Rapid Fire", &rapid_fire);
		ImGui::Checkbox("Max Damage", &max_damage);
		ImGui::Checkbox("No Recoil", &no_recoil);

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

	if (infinite_stamina) {
		local_mec->set_stamina(1.);
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
			if (esp_enabled) {
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
						auto name_norm = "[" + name + "]" + (role == 4 ? " [DISSIDENT]" : "");
						overlay->draw_text(screen_position, color, name_norm.c_str(), true);
					}
				}
			}
			if (class_name.find("WorldItem_C") != std::string::npos) {
				world_item* item = (world_item*)actor;
				if (!item) continue;

				auto data = item->get_data();
				auto item_name = data->get_name().read_string();

				if (item_name == "PISTOL" || item_name == "REVOLVER" || item_name == "SHORTY") {
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

					if (gun_esp_enabled) {
						auto position = item_root->get_relative_location();
						vector3 screen_position{};
						if (util::w2s(position, last_frame_cached.pov, screen_position)) {
							overlay->draw_text(screen_position, IM_COL32(255, 69, 0, 255), "[GUN]", true);
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