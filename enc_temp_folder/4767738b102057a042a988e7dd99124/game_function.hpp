#ifndef GAME_FUNCTIONS_HPP
#define GAME_FUNCTIONS_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include "game_structures.hpp"
#include "util.hpp"
#include "mem.hpp"

using namespace protocol::engine::sdk;
using namespace protocol::game::sdk;
using namespace protocol::engine;

// Global item class mapping
inline std::unordered_map<std::string, std::string> item_class_map = {
	{"REVOLVER", "DA_RevolverDefault"},
	{"PISTOL", "DA_PistolDefault"},
	{"SHOTGUN", "DA_Shotgun"},
	{"SHORTY", "DA_ShortyDefault"},
	{"SMG", "DA_SMG"},
	{"RIFLE", "DA_Rifle"},
	{"C4", "DA_C4"},
};

std::vector<uintptr_t> find_objects(const std::string& name_find);

u_data_item* AssignToItemData(const std::string& target_name);

// Check if an object matches a class name
inline bool is_a(uintptr_t object_address, const std::string& class_name, bool exact_match = false) {
	if (!object_address) return false;

	auto object = reinterpret_cast<u_object*>(object_address);
	auto object_class = object->class_private();

	while (object_class) {
		int fname_idx = object_class->fname_index();
		std::string current_class_name = util::get_name_from_fname(fname_idx);

		if (exact_match) {
			if (current_class_name == class_name) return true;
		}
		else {
			if (current_class_name.find(class_name) != std::string::npos) return true;
		}

		object_class = object_class->super();
	}

	return false;
}

// Function to capitalize strings
inline std::string ToUpperCase(const std::string& str) {
	std::string upper_case = str;
	std::transform(upper_case.begin(), upper_case.end(), upper_case.begin(), ::toupper);
	return upper_case;
}

// Find objects by name
inline std::vector<uintptr_t> find_objects(const std::string& name_find) {
	std::vector<uintptr_t> objs;
	constexpr auto elements_per_chunk = 64 * 1024;
	auto gobjects = mem::rpm<fuobjectarray>(mem::module_base + GOBJECTS);

	for (int i = 0; i < gobjects.num_chunk; i++) {
		auto chunk_start = mem::rpm<uintptr_t>(gobjects.chunk_table + (i * 0x8));
		for (int j = 0; j < elements_per_chunk; j++) {
			auto item = mem::rpm<fuobjectitem>(chunk_start + (j * sizeof(fuobjectitem)));
			if (item.object == 0) continue;

			auto obj = reinterpret_cast<u_object*>(item.object);
			std::string name = util::get_name_from_fname(obj->fname_index());
			std::string class_name = util::get_name_from_fname(obj->class_private()->fname_index());

			if (name.find(name_find) != std::string::npos) {
				objs.push_back(item.object);
			}
		}
	}
	return objs;
}

// Assign item data
// Assign item data
inline u_data_item* AssignToItemData(const std::string& target_name) {
	// Convert target_name to uppercase
	std::string upper_name = ToUpperCase(target_name);
	
	if (item_class_map.find(upper_name) == item_class_map.end()) {
		return nullptr;
	}

	std::string mapped_name = item_class_map[upper_name];
	std::vector<uintptr_t> found_objects = find_objects(mapped_name);

	// Fallback for KNIFE or ambiguous matches
	if (found_objects.empty() && upper_name == "KNIFE") {
		found_objects = find_objects("Knife");
	}

	if (found_objects.empty()) {
		return nullptr;
	}

	uintptr_t selected_object = 0;
	for (uintptr_t obj_addr : found_objects) {
		auto obj = reinterpret_cast<u_object*>(obj_addr);
		std::string obj_name = util::get_name_from_fname(obj->fname_index());
		std::string class_name = util::get_name_from_fname(obj->class_private()->fname_index());

		if (obj_name.find(mapped_name) != std::string::npos) {
			if ((upper_name == "KNIFE" && is_a(obj_addr, "Data_Melee_C")) ||
				(is_a(obj_addr, "Data_Gun_C") && upper_name != "KNIFE")) {
				selected_object = obj_addr;
				break; // Stop searching once a valid match is found
			}
		}
	}

	if (selected_object != 0) {
		auto obj = reinterpret_cast<u_object*>(selected_object);
		std::string obj_name = util::get_name_from_fname(obj->fname_index());
		return reinterpret_cast<u_data_item*>(selected_object);
	}

	return nullptr;
}

using NetSpawnNewItemFunc = void(__fastcall*)(
	FVector,         // Location
	FVector,         // Mec Velocity
	FVector,         // Drop Direction
	u_data_item*,   // Data
	FStr_ItemState   // Item State
	);

// Inline function to call NetSpawnNewItem
inline void NetSpawnNewItem(
	FVector location,
	FVector mec_velocity,
	FVector drop_direction,
	u_data_item* data,
	FStr_ItemState item_state
) {
	// Resolve the function pointer using the offset
	auto spawn_item_func = reinterpret_cast<NetSpawnNewItemFunc>(mem::module_base + 0x1CF0950);

	if (!spawn_item_func) {
		std::cerr << "NetSpawnNewItem function not found!" << std::endl;
		return;
	}

	if (!data) {
		std::cerr << "Invalid UData_Item_C pointer. Aborting spawn." << std::endl;
		return;
	}

	// Call the function
	spawn_item_func(location, mec_velocity, drop_direction, data, item_state);
}

#endif
