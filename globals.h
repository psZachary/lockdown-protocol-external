#pragma once
#include <vector>
#include <unordered_map>
#include "game_structures.hpp"
#include <d3d11.h>
#include <unordered_set>

namespace globals {
	using namespace protocol::engine::sdk;
	using namespace protocol::game::sdk;

	using tProcessEvent = void(*)(u_object*, u_function*, void*);
	inline tProcessEvent process_event = nullptr;

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
	inline std::vector < a_alarm_button_c* > alarm_button_cache{};
	inline std::vector < a_rez_charger_c* > rez_charger_cache{};
	inline std::vector < a_weapon_case_code_c* > weapon_case_cache{};

	struct s_font {
		ImFont* im_font = nullptr;
		float font_size = 14.0f;
	};

	inline s_font default_font;

	inline std::unordered_set<std::string> inserted_names;
}