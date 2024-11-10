#pragma once

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

	// Player
	inline bool speedhack = false;
	inline bool infinite_stamina = false;
	inline bool god_mode = false;
	inline bool living_state = false;

	// Weapons
	// Melee
	inline bool fast_melee = false;
	inline bool infinite_melee_range = false;
	// Guns
	inline bool max_damage = false;
	inline bool auto_fire = false;
	inline bool rapid_fire = false;
	inline bool no_recoil = false;
	inline bool infinite_ammo = false;

	// Inventory
	inline bool can_inventory = false;

	// ESP
	inline bool esp_enabled = true;

	inline bool player_esp = true;
	inline bool player_distance = true;

	inline bool weapon_esp = true;
	inline bool weapon_item_state = true;
	inline bool weapon_distance = false;

	inline bool primary_object_esp = true;
	inline bool primary_item_state = false;
	inline bool primary_distance = false;

	inline bool secondary_item_state = false;
	inline bool secondary_distance = false;
	inline bool secondary_object_esp = true;

	inline bool task_object_esp = true;
	inline bool task_object_distance = true;
	inline bool task_object_state = true;

	inline bool task_delivery = true;
	inline bool task_machine = true;
	inline bool task_vent = true;
	inline bool task_alim = true;
	inline bool task_pizzushi = true;
	inline bool task_computers = true;
}