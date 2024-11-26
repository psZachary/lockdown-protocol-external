#pragma once
#include <vector>
#include "game_structures.hpp"
#include <d3d11.h>

namespace globals {
	using namespace protocol::engine::sdk;
	using namespace protocol::game::sdk;

	inline u_world* gworld = 0;
	inline a_game_state_base* game_state = 0;
	inline u_game_instance* owning_instance = 0;
	inline u_localplayer* local_player = 0;
	inline a_player_controller* local_controller = 0;
	inline a_player_camera_manager* local_camera_manager = 0;
	inline mec_pawn* local_mec = 0;

	inline std::vector < mec_pawn* > player_cache{};
	inline std::vector < world_item* > world_item_cache{};
	inline std::vector < task_vents* > task_vents_cache{};
	inline std::vector < task_machines* > task_machines_cache{};
	inline std::vector < task_alimentations* > task_alims_cache{};
	inline std::vector < task_deliveries* > task_delivery_cache{};
	inline std::vector < task_pizzushis* > task_pizzushi_cache{};
	inline std::vector < task_data* > task_data_cache{};
	inline std::vector < task_scanner* > task_scanner_cache{};
	inline std::vector < a_weapon_case_code_c* > weapon_case_cache{};
}