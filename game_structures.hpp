#pragma once

#include "ufield.hpp"
#include "macros.hpp"
#include "game_math.hpp"
#include <vector>
#include <cwchar> 
#include <string>
#include <map>
#include "mem.hpp"
#include "fstr_color_link.hpp"


namespace util {
	std::string get_name_from_fname(int key);
}

namespace protocol {
	namespace engine {

		constexpr uintptr_t GWORLD = 143247552;
		constexpr uintptr_t GNAMES = 140767680;
		constexpr uintptr_t GOBJECTS = 141699376;
		constexpr uintptr_t PROCESSEVENT = 21283536;
		constexpr uintptr_t PROCESSEVENT_INDEX = 79;
		constexpr uintptr_t APPENDSTRING = 19029504;

		namespace sdk {
			struct fuobjectitem
			{
				// Pointer to the allocated object
				uintptr_t object;
				// Internal flags
				int32_t flags;
				// UObject Owner Cluster Index
				int32_t cluster_root_index;
				int32_t serial_number;
			};

			class fuobjectarray {
			public:
				uintptr_t chunk_table;
				uintptr_t allocated;
				int32_t max_elem;
				int32_t num_elem;
				int32_t max_chunk;
				int32_t num_chunk;
			};

			struct fname {
				int index;
				int number;
			};
			class wchar_holder
			{
			public:
				wchar_t data[250];
			};

			class u_struct : public u_field {
			public:
				// Pointer to the parent struct (if any)
				GET_OFFSET(0x40, super_struct, u_struct*);

				// Linked list of fields (properties, functions, etc.)
				GET_OFFSET(0x48, children, u_field*);

				// Child properties (additional fields specific to this struct)
				GET_OFFSET(0x50, child_properties, void*); // Use `void*` or define `f_field` if available.

				// Total size of the struct
				GET_OFFSET(0x58, size, int32_t);

				// Minimum alignment for the struct
				GET_OFFSET(0x5C, min_alignment, int32_t);
			};

			class u_function : public u_struct {
			public:
				using FNativeFuncPtr = void (*)(void* Context, void* TheStack, void* Result);

				// Function flags (e.g., blueprint callable, server, client, etc.)
				GET_OFFSET(0xB0, function_flags, uint32_t);

				// Native function execution pointer (if applicable)
				GET_OFFSET(0xD8, exec_function, FNativeFuncPtr);
			};

			struct f_bone_node {
				fname name;
				int32_t parent_idx;
				char _pad[0x4];
			};

			struct FStr_ItemState {
				int32_t Value_8;
				int32_t Time_15;
			};

			struct FStr_WeaponCase_Step {
				int Value;
				int Target;
			};

			struct FTimerHandle {
				UINT64 Handle;
			};

			struct FStr_ScannerDot {
				vector2 Rotation_9;
				int32_t Process_5;
			};

			struct FStr_SkinSet {
				int Color_8; // 0 through 10
				INT32 Skin_18;
			};

			template <typename T>
			class t_array {
			public:
				uintptr_t _data;
				int32_t count;
				int32_t _max;

				T data() {
					return mem::rpm<T>(_data + 0x0);
				}

				T at(int32_t index) {
					return mem::rpm<T>(_data + (index * sizeof(T)));
				}

				void set(int32_t index, T value)
				{
					mem::wpm<T>(_data + (index * sizeof(T)), value);
				}

				std::vector<T> list() {
					std::vector<T> list{};
					try
					{
						if (!count || count <= 0) return list;

						//T* buffer = new T[count];
						//read_raw(_data, buffer, count * sizeof(T));
						for (int i = 0; i < count; i++) {
							T entry = at(i);//rpm<T>(_data + (i * sizeof(T));//buffer[i];

							list.push_back(entry);
						}
					}
					catch (std::bad_alloc e)
					{
					}
					catch (...)
					{
					}
					//delete[] buffer;
					return list;
				}
			};

			struct FStr_WeaponCase_Result {
				t_array<UINT8> Values;
				t_array<int> Types;
				int State;
				FStr_WeaponCase_Step Steps;
			};

			template<typename KT, typename VT>
			class t_pair
			{
			public:
				KT first;
				VT second;
			};

			template<typename ELT>
			class t_set_element
			{
			public:
				ELT value;
				int32_t hash_next_id;
				int32_t hash_index;
			};
			template <typename KY, typename V>
			class t_map {
			public:
				t_array<t_set_element<t_pair<KY, V>>> elements;

				std::map<KY, V> map() {
					std::vector<t_set_element<t_pair<KY, V>>> elements = pthis->elements.list();
					std::map<KY, V> map{};
					for (const auto& element : elements) {
						map[element.value.first] = element.value.second;
					}
					return map;
				}
			};

			struct f_minimal_view_info
			{
				vector3 location;
				vector3 rotation;
				float fov;
				float desiredfov;
			};

			struct f_camera_cache
			{
				char pad_0x0[0x10];
				f_minimal_view_info pov;
			};
			struct f_transform {
				quat rotation;
				vector3 translation;
				char pad34[0x4];
				vector3 scale3d;

				matrix4x4_t to_matrix_with_scale()
				{
					matrix4x4_t m;
					m._41 = translation.x;
					m._42 = translation.y;
					m._43 = translation.z;

					double x2 = rotation.x + rotation.x;
					double y2 = rotation.y + rotation.y;
					double z2 = rotation.z + rotation.z;

					double xx2 = rotation.x * x2;
					double yy2 = rotation.y * y2;
					double zz2 = rotation.z * z2;
					m._11 = (1.0f - (yy2 + zz2)) * scale3d.x;
					m._22 = (1.0f - (xx2 + zz2)) * scale3d.y;
					m._33 = (1.0f - (xx2 + yy2)) * scale3d.z;

					double yz2 = rotation.y * z2;
					double wx2 = rotation.w * x2;
					m._32 = (yz2 - wx2) * scale3d.z;
					m._23 = (yz2 + wx2) * scale3d.y;

					double xy2 = rotation.x * y2;
					double wz2 = rotation.w * z2;
					m._21 = (xy2 - wz2) * scale3d.y;
					m._12 = (xy2 + wz2) * scale3d.x;

					double xz2 = rotation.x * z2;
					double wy2 = rotation.w * y2;
					m._31 = (xz2 + wy2) * scale3d.z;
					m._13 = (xz2 - wy2) * scale3d.x;

					m._14 = 0.0f;
					m._24 = 0.0f;
					m._34 = 0.0f;
					m._44 = 1.0f;

					return m;
				}
			};

			struct FVector {
				double X;  // Offset: 0x0
				double Y;  // Offset: 0x8
				double Z;  // Offset: 0x10
			};

			struct ft_view_target {
				f_minimal_view_info pov; // Offset 0x10
			};

			struct FRotator {
				double Pitch;  // Offset: 0x0
				double Yaw;  // Offset: 0x8
				double Roll;  // Offset: 0x10
			};

			class FloatVector : public u_object {
			public:
				// Getter for FVector at offset
				FVector get_net_location() {
					return mem::rpm<FVector>(pthis + 0x588);
				}

				// Setter for FVector at offset
				void set_net_location(const FVector& value) {
					mem::wpm<FVector>(pthis + 0x588, value);
				}
			};

			// fix later, fuk u bditt
			class fstring : public t_array<wchar_t> {
			public:
				std::string read_string() {
					if (count <= 0 || count > 500) {
						return std::string();
					}

					// Read the wide character string from memory
					auto test = mem::rpm<wchar_holder>(_data);

					// Convert from wide string to UTF-8
					int utf8_len = WideCharToMultiByte(CP_UTF8, 0, test.data, -1, nullptr, 0, nullptr, nullptr);
					if (utf8_len <= 0) {
						return std::string();
					}

					std::string utf8_string(utf8_len - 1, '\0'); // -1 to exclude the null terminator
					WideCharToMultiByte(CP_UTF8, 0, test.data, -1, &utf8_string[0], utf8_len, nullptr, nullptr);

					return utf8_string;
				}
			};

			class FTextData {
			public:

			};

			class FText {
			public:
				std::string read_string() {
					auto s = mem::rpm<fstring>(pthis + 0x28);
					return s.read_string();
				}

			};

			class u_scene_component : public u_object {
			public:
				vector3 get_relative_rotation() {
					return mem::rpm<vector3>(pthis + 0x0140);
				}
				void set_relative_rotation(vector3 set) {
					if (pthis < 0x1000) return;
					mem::wpm<vector3>(pthis + 0x0140, set);
				}
				vector3 get_relative_location() {
					return mem::rpm<vector3>(pthis + 0x0128);
				}

				void set_relative_location(vector3 set) {
					if (pthis < 0x1000) return;
					mem::wpm<vector3>(pthis + 0x0128, set);
				}

				void set_component_velocity(vector3 new_velocity) {
					if (pthis < 0x1000) return;
					mem::wpm<vector3>(pthis + 0x0170, new_velocity);
				}

				vector3 get_velocity() {
					return mem::rpm<vector3>(pthis + pthis + 0x0170);
				}

				f_transform get_comp_to_world() {
					//uintptr_t c2wptr = mem::rpm<uintptr_t>(pthis + 0x1C0);
					return mem::rpm<f_transform>(pthis + 0x1D0);
				}

				void set_relative_scale(vector3 scale3d) {
					if (pthis < 0x1000) return;

					mem::wpm<vector3>(pthis + 0x0158, scale3d);
				}

				vector3 get_relative_scale() {
					return mem::rpm<vector3>(pthis + 0x0158);
				}

				f_transform get_transform() {
					f_transform t{ };
					t.scale3d = get_relative_scale();
					t.translation = get_relative_location();
					return t;
				}

				bool get_b_visible() {
					uint8_t bitfield = mem::rpm<uint8_t>(pthis + 0x188);
					return (bitfield >> 5) & 0x1; // Extract bit 5
				}

				void set_b_visible(bool value) {
					uint8_t bitfield = mem::rpm<uint8_t>(pthis + 0x188);
					if (value)
						bitfield |= (1 << 5); // Set bit 5
					else
						bitfield &= ~(1 << 5); // Clear bit 5
					mem::wpm<uint8_t>(pthis + 0x188, bitfield);
				}

				bool get_hidden_in_game() {
					uint8_t bitfield = mem::rpm<uint8_t>(pthis + 0x189);
					return (bitfield >> 3) & 0x1; // Extract bit 3
				}

				void set_hidden_in_game(bool value) {
					uint8_t bitfield = mem::rpm<uint8_t>(pthis + 0x189);
					if (value)
						bitfield |= (1 << 3); // Set bit 3
					else
						bitfield &= ~(1 << 3); // Clear bit 3
					mem::wpm<uint8_t>(pthis + 0x189, bitfield);
				}
			};

			class u_capsule_component : public u_object {
			public:
				OFFSET(0x588, capsule_half_height, float);
				OFFSET(0x58C, capsule_radius, float);
			};

			class u_sphere_component : public u_object {
			public:
				OFFSET(0x588, sphere_radius, float);
			};

			class character_movement_component : public u_object {
			public:
				OFFSET(0x178, jump_z_velocity, float);
				OFFSET(0x170, gravity_scale, float);
				OFFSET(0x1A4, movement_mode, int); //0:none | 1:walk | 2:navwalk | 3:fall | 4:swim | 5:fly | 6:custom
			};

			class a_actor : public u_object {
			public:
				GET_OFFSET(0x0158, owner, a_actor*);
				GET_OFFSET(0x01B8, root_component, u_scene_component*);
				OFFSET(0x0068, custom_time_dilation, float);
			};

			class a_pawn : public a_actor {
			public:
				class a_player_state* player_state() {
					return mem::rpm<a_player_state*>(pthis + 0x02C8);
				}
			};

			class a_player_camera_manager {
			public:
				OFFSET(0x1410, cached_frame_private, f_camera_cache);
				OFFSET(0x1C50, last_cached_frame_private, f_camera_cache);
				OFFSET(0x0340, view_target, ft_view_target);
				OFFSET(0x256C, view_pitch_min, float);
				OFFSET(0x2570, view_pitch_max, float);
				OFFSET(0x2574, view_yaw_min, float);
				OFFSET(0x2578, view_yaw_max, float);
				OFFSET(0x257C, view_roll_min, float);
				OFFSET(0x2580, view_roll_max, float);
			};

			class a_player_state : public a_actor {
			public:
				GET_OFFSET(0x0320, pawn_private, a_pawn*);
				GET_OFFSET(0x0340, player_name_private, fstring);
				GET_OFFSET(0x02F8, saved_network_address, fstring);
				OFFSET(0x02A8, score, float);
				OFFSET(0x02AC, player_id, int);
			};

			class a_game_state_base {
			public:
				t_array<a_player_state*> player_array() {
					return mem::rpm<t_array<a_player_state*>>(pthis + 0x02C0);
				}
			};

			class a_controller : public a_actor {
			public:
				OFFSET(0x0320, control_rotation, vector3);
			};

			class a_player_controller : public a_controller {
			public:
				GET_OFFSET(0x0360, camera_manager, a_player_camera_manager*);
				GET_OFFSET(0x0378, target_view_rotation, FRotator);
				GET_OFFSET(0x03A8, smooth_target_view_rotation_speed, float);
				GET_OFFSET(0x0350, pawn, a_pawn*);
			};

			class u_player : public u_object {
			public:
				GET_OFFSET(0x30, player_controller, a_player_controller*);
			};

			class u_localplayer : public u_player {
			public:

			};

			class u_game_instance {
			public:
				u_localplayer* get_localplayer() {
					auto lclplrs = mem::rpm<t_array<u_localplayer*>>(pthis + 0x38);
					return lclplrs.at(0);
				}
			};

			class u_level {
			public:
				GET_OFFSET(0x00A0, actors, t_array<a_actor*>);
			};

			class u_world {
			public:
				static u_world* get_world(uintptr_t process_base) {
					return mem::rpm<u_world*>(process_base + GWORLD);
				}
				GET_OFFSET(0x30, persistent_level, u_level*);
				GET_OFFSET(0x0160, game_state, a_game_state_base*);
				GET_OFFSET(0x01D8, owning_game_instance, u_game_instance*);
				GET_OFFSET(0x0178, levels, t_array<u_level*>);
			};

			class u_skeletal_mesh_component : public u_scene_component {
			public:

				t_array<f_transform> cached_bone_space_transforms() {
					return mem::rpm<t_array<f_transform>>(pthis + 0x750);
				}
				f_transform get_bone(int bone_id) {
					uintptr_t bone_arr = mem::rpm<uintptr_t>(pthis + 0x4C0);
					if (!bone_arr) bone_arr = mem::rpm<uintptr_t>(pthis + 0x4C0 + 0x10);
					if (!bone_arr) bone_arr = mem::rpm<uintptr_t>(pthis + 0x4C0 - 0x10);
					if (!bone_arr) return f_transform{};

					return mem::rpm<f_transform>(bone_arr + (static_cast<unsigned long long>(bone_id) * 0x30));
				};
			};

			class a_character : public a_pawn {
			public:
				u_skeletal_mesh_component* mesh() {
					return mem::rpm<u_skeletal_mesh_component*>(pthis + 0x0280);
				}
				GET_OFFSET(0x320, CharacterMovement, character_movement_component*);
			};

			struct u_attribute_set : public u_object {

			};

			class u_anim_instance : public u_object {
			public:

			};

			struct u_skeletal_mesh_socket {
				char pad0x28[0x28];
				fname socket_name;
				fname bone_name;
				vector3 relative_location;
				quat relative_rotation;
				vector3 relative_scale;
				bool b_force_always_animated;
			};
		}
	}
	namespace game {
		using namespace engine::sdk;
		namespace sdk {
			class u_camera_component : public u_object {
			public:
				OFFSET(0x0230, field_of_view, float);
				OFFSET(0x023C, ortho_width, float);
				OFFSET(0x0254, aspect_ratio, float);
			};
			class u_static_mesh : public u_object {
			public:
				// tbd
			};
			class u_data_skin_ghost : public u_object {
			public:
				OFFSET(0x70, mesh_h, u_static_mesh*);
				OFFSET(0x78, mesh_v, u_static_mesh*);
			};
			class u_save_skin : public u_object {
			public:
				OFFSET(0x28, setting, FStr_SkinSet);
			};
			class u_data_item_throwtype : public u_object {
			public:
				OFFSET(0x30, throw_force, int32_t);
				OFFSET(0x34, vertical_force, int32_t);
				OFFSET(0x38, vertical_offset, int32_t);
				OFFSET(0x40, restitution, double);
				OFFSET(0x48, gravity, double);
				OFFSET(0x50, drag, double);
				OFFSET(0x58, radius, double);
			};
			class u_data_item : public u_object {
			public:
				GET_OFFSET(0x38, name, fstring);
				GET_OFFSET(0x128, use_name, fstring);
				OFFSET(0x1A8, throw_type, u_data_item_throwtype*);
				OFFSET(0x198, can_inventory, bool);
			};
			struct FStr_Item {
				u_data_item* Data_18;
				FStr_ItemState State_19;
			};
			class u_data_meleetype : public u_object {
			public:
				OFFSET(0x0060, recover_time, double);
				OFFSET(0x0058, cast_time, double);
				OFFSET(0x0068, range, int32_t);
				OFFSET(0x0038, stun, double);
				OFFSET(0x0030, heal, int32_t);
				OFFSET(0x0034, stamina, int32_t);
				OFFSET(0x0048, crit_stun, double);
				OFFSET(0x0040, crit_heal, int32_t);
				OFFSET(0x0044, crit_stamina, int32_t);
				OFFSET(0x0050, cost, int32_t);
			};
			class a_voice_source : public u_object {
			public:
				OFFSET(0x2E0, target_distance, float);
				OFFSET(0x328, deviation_ratio, double);
				OFFSET(0x330, delta, float);
				OFFSET(0x338, distance_factor, double);
			};
			class u_data_melee : public u_data_item {
			public:
				GET_OFFSET(0x02D8, melee_type, u_data_meleetype*)
			};
			class u_data_gun : public u_data_item {
			public:
				GET_OFFSET(0x2C8, am_hand_fire, uintptr_t);
				GET_OFFSET(0x2D0, am_body_fire, uintptr_t);
				OFFSET(0x2D8, damage, int32_t);
				OFFSET(0x2DC, crit, int32_t);
				OFFSET(0x2E0, stamina_damage, int32_t);
				OFFSET(0x2E4, crit_stamina, int32_t);
				OFFSET(0x2E8, fire_rate, double);
				OFFSET(0x2F0, auto_fire, bool);
				OFFSET(0x2F4, impact_type, int32_t);
				OFFSET(0x2F8, recoil_spread, double);
				OFFSET(0x300, recoil_interp, double);
				OFFSET(0x308, recoil_recover, double);
				OFFSET(0x310, offset_vertical, double);
				OFFSET(0x318, offset_horizontal, double);
				OFFSET(0x320, recoil_elevation, double);
				OFFSET(0x328, shot_oscillation, double);
				OFFSET(0x330, stand_precision, double);
				OFFSET(0x338, stand_oscillation, double);
				OFFSET(0x340, walk_precision, double);
				OFFSET(0x348, walk_oscillation, double);
				OFFSET(0x350, run_precision, double);
				OFFSET(0x358, run_oscillation, double);
				OFFSET(0x360, air_precision, double);
				OFFSET(0x368, precision_exhausted, double);
				OFFSET(0x370, precision_reactivity, double);
				OFFSET(0x378, sprint_recover, double);
				OFFSET(0x380, oscillation_reactivity, double);
				OFFSET(0x388, recoil_reactivity, double);
				OFFSET(0x390, pattern, int32_t);
				OFFSET(0x398, shake_intensity, double);
				OFFSET(0x3A0, capacity, int32_t);
				OFFSET(0x3A8, stun, double);
				GET_OFFSET(0x3B0, stamina_usage, int32_t);
				GET_OFFSET(0x3B8, pattern_recoil, uintptr_t);
				GET_OFFSET(0x3C0, pattern_spread, uintptr_t);
				GET_OFFSET(0x3C8, pattern_effects, uintptr_t);
			};
			class world_item : public a_actor {
			public:
				GET_OFFSET(0x03D8, data, u_data_item*);
				GET_OFFSET(0x0468, distance, float);
				OFFSET(0x0338, location, FVector);
				OFFSET(0x03E8, item_state, FStr_ItemState);
			};
			class a_itemslot_c : public a_actor {
			public:
				OFFSET(0x02F0, item_state, FStr_ItemState);
			};
			class a_weapon_case_box_c : public a_actor {
			public:
				OFFSET(0x02F8, case_open, bool);
				OFFSET(0x0308, item_slot, a_itemslot_c*);
				OFFSET(0x0300, selected_weapon_qsdsf, u_data_gun*);
				OFFSET(0x0310, selected_weapon, int);
			};
			class a_weapon_case_code_c : public a_actor {
			public:
				OFFSET(0x02D8, default_scene_root, u_scene_component*);
				OFFSET(0x0330, target_values, t_array<UINT8>);
				OFFSET(0x02F0, process_values, t_array<UINT8>);
				OFFSET(0x0340, process_index, INT32);
				OFFSET(0x0360, result_values, t_array<UINT8>);
				OFFSET(0x0388, box_to_open, a_weapon_case_box_c*);
				OFFSET(0x0358, step, FStr_WeaponCase_Step);
				OFFSET(0x0300, result, FStr_WeaponCase_Result);
				OFFSET(0x0398, open_delay, INT32);
				OFFSET(0x03A0, opening_timer, FTimerHandle);
			};
			class a_vent_c : public a_actor {
			public:
				GET_OFFSET(0x0360, task_vent, bool);
				GET_OFFSET(0x0320, filter, a_itemslot_c*);
				GET_OFFSET(0x0300, root, u_scene_component*);
				OFFSET(0x0350, lock_state, int);
				OFFSET(0x0375, sector, int);
				OFFSET(0x0354, clean_request, int32_t);
				OFFSET(0x0318, energy, int32_t);
			};
			class task_vents : public a_actor {
			public:
				GET_OFFSET(0x0390, task_vents, t_array<a_vent_c*>);
			};
			class a_bottle_slot_c : public a_actor {
			public:
				GET_OFFSET(0x031C, request_level, int32_t);
				GET_OFFSET(0x02F4, level, int32_t);
				GET_OFFSET(0x02D0, root, u_scene_component*);
			};
			class a_machine_pannel_c : public a_actor {
			public:
				GET_OFFSET(0x0330, bottles, t_array<a_bottle_slot_c*>);
			};
			class task_machines : public a_actor {
			public:
				GET_OFFSET(0x0398, machines, t_array<a_machine_pannel_c*>);
			};
			class a_alarm_button_c : public a_actor {
			public:
				OFFSET(0x02E0, state, int);
				OFFSET(0x02C8, root, u_scene_component*);
			};
			class a_scanner_machine_c : public a_actor {
			public:
				GET_OFFSET(0x02B8, mecs, t_array<a_pawn*>);
				GET_OFFSET(0x02D0, scan_rotation, FRotator);
				GET_OFFSET(0x02B0, default_scene_root, u_scene_component*);
			};
			class uw_scanner_targetdot_c : public u_object {
			public:
				GET_OFFSET(0x0320, location, vector2);
				GET_OFFSET(0x0318, radius, double);
				GET_OFFSET(0x02F8, rotation, FRotator);
			};
			class a_scanner_screen_c : public a_actor {
			public:
				GET_OFFSET(0x03C0, targets, t_array<FStr_ScannerDot>);
				GET_OFFSET(0x03E0, nearest_dot, double);
				GET_OFFSET(0x0338, timeline_value, float);
				GET_OFFSET(0x033C, timeline_direction, int); // 0 forward | 1 backward
				GET_OFFSET(0x0354, process, int32_t);
				GET_OFFSET(0x03A8, main_rotation, FRotator);
				GET_OFFSET(0x03D0, h_angles, t_array<int32_t>);
				GET_OFFSET(0x0410, targets_process, t_array<int32_t>);
				GET_OFFSET(0x0420, old_targets_process, t_array<int32_t>);
				GET_OFFSET(0x03F0, main_rotation_target, FRotator);
			};
			class task_scanner : public a_actor {
			public:
				GET_OFFSET(0x0378, scanner_ref, a_scanner_machine_c*);
				GET_OFFSET(0x0380, screen_ref, a_scanner_screen_c*);
			};
			class u_text_block : public u_object {
			public:
				GET_OFFSET(0x170, text, FText*);
			};
			class uw_datapc_ui_c : public u_object {
			public:
				GET_OFFSET(0x288, head_text, u_text_block*);
			};
			class a_data_pc_c : public a_actor {
			public:
				GET_OFFSET(0x0318, root, u_scene_component*);
				GET_OFFSET(0x02D8, map_position, u_scene_component*);
				GET_OFFSET(0x0328, state, int);
				GET_OFFSET(0x032C, process, int32_t);
				GET_OFFSET(0x0348, room, int32_t);
				GET_OFFSET(0x034C, index_message, int32_t);
				GET_OFFSET(0x0360, other_pc, a_data_pc_c*);
				GET_OFFSET(0x0320, w_screen, uw_datapc_ui_c*);
			};
			class task_data : public a_actor {
			public:
				GET_OFFSET(0x0370, source_pc, t_array<a_data_pc_c*>);
				GET_OFFSET(0x0380, target_pc, t_array<a_data_pc_c*>);
				GET_OFFSET(0x0390, task_source_pc, t_array<a_data_pc_c*>);
				GET_OFFSET(0x03A0, task_target_pc, t_array<a_data_pc_c*>);
			};
			class a_rez_charger_c : public a_actor {
			public:
				GET_OFFSET(0x0308, root, u_scene_component*);
				GET_OFFSET(0x0360, rez, a_itemslot_c*);
				GET_OFFSET(0x0358, battery_l, a_itemslot_c*);
				GET_OFFSET(0x0340, battery_r, a_itemslot_c*);
				GET_OFFSET(0x0368, charge_l, int);
				GET_OFFSET(0x0350, charge_r, int);
				GET_OFFSET(0x0318, process, int);
				GET_OFFSET(0x0338, state, int); // 0, 1, 2, 3, 4, 5, 6, 7
			};
			class a_alimbox_c : public a_actor {
			public:
				GET_OFFSET(0x0318, root, u_scene_component*);
				GET_OFFSET(0x035C, battery_value, int32_t);
				GET_OFFSET(0x0398, batteries_count, int32_t);
				GET_OFFSET(0x0350, in_color, int32_t);
				GET_OFFSET(0x0354, out_color, int32_t);
				GET_OFFSET(0x03D0, task_value, int32_t);
				GET_OFFSET(0x0420, finished, bool);
				GET_OFFSET(0x0368, alimented, bool);
			};
			class task_alimentations : public a_actor {
			public:
				GET_OFFSET(0x03C0, task_alims, t_array<a_alimbox_c*>);
			};
			class a_deliverycase_c : public a_actor {
			public:
				GET_OFFSET(0x02F0, root, u_scene_component*);
				GET_OFFSET(0x0348, good_package, uint8_t);
			};
			class task_deliveries : public a_actor {
			public:
				GET_OFFSET(0x0380, task_cases, t_array<a_deliverycase_c*>);
			};
			class a_pizzushi_table_c : public a_actor {
			public:
				GET_OFFSET(0x0318, root, u_scene_component*);
				GET_OFFSET(0x0324, rice_type, int32_t);
				GET_OFFSET(0x0328, fish_type, int32_t);
				GET_OFFSET(0x032C, topping_type, int32_t);
				GET_OFFSET(0x0350, finished, bool);
				GET_OFFSET(0x0338, request_state, int32_t);
			};
			class task_pizzushis : public a_actor {
			public:
				GET_OFFSET(0x0388, task_tables, t_array<a_pizzushi_table_c*>);
			};
			class a_gm_c : public u_object {
			public:
				GET_OFFSET(0x0468, player_colors_app, t_array<FStr_ColorLink>);
			};
			class a_pc_c : public u_object {
			public:
				GET_OFFSET(0x0998, gm_ref, a_gm_c*);
			};
			class u_data_player : public u_object {
			public:
				OFFSET(0x30, body_armor_1, int32_t);
				OFFSET(0x34, body_armor_2, int32_t);
				OFFSET(0x38, body_armor_3, int32_t);
				OFFSET(0x3C, head_armor_1, int32_t);
				OFFSET(0x40, head_armor_2, int32_t);
				OFFSET(0x44, head_armor_3, int32_t);
				OFFSET(0x1B8, regen_hp_speed, double);
				OFFSET(0x1A8, min_regen, double);
				OFFSET(0x1B0, max_regen, double);
				OFFSET(0x0058, speed1, double);
				OFFSET(0x60, speed2, double);
				OFFSET(0x0068, speed3, double);
				OFFSET(0x70, slow1, double);
				OFFSET(0x78, slow2, double);
				OFFSET(0x80, slow3, double);
				OFFSET(0x88, default_speed, double);
				OFFSET(0x1D0, player_map_location_x, vector2);
				OFFSET(0x1E0, player_map_location_y, vector2);
				OFFSET(0x1F0, player_map_result_x, vector2);
				OFFSET(0x200, player_map_result_y, vector2);
			};
			class mec_pawn : public a_pawn {
			public:
				OFFSET(0x0C19, player_role, int);
				OFFSET(0x0C48, stamina, double);
				OFFSET(0x0580, aim_location, FVector);
				OFFSET(0x05C8, net_aim_target, FVector);
				OFFSET(0x05E8, net_aim, FVector);
				OFFSET(0x0868, interaction_aim, FVector);
				OFFSET(0x0990, aim_offset_smooth, FVector);
				OFFSET(0x09A8, aim_offset, FVector);
				OFFSET(0x09F8, aim_offset_target, FVector);
				OFFSET(0x0780, target_lock_rotation, FRotator);
				OFFSET(0x0A10, aim_oscilation_factor, double);
				OFFSET(0x0810, health, int);
				OFFSET(0x0CA9, alive, bool);
				OFFSET(0x04E4, on_floor, bool);
				OFFSET(0x05E0, net_floor, bool);
				OFFSET(0x0D82, can_play, bool);
				OFFSET(0x1068, in_game, bool);
				OFFSET(0x0578, run, bool);
				OFFSET(0x0579, walk, bool);
				OFFSET(0x0720, acceleration, vector2);
				OFFSET(0x05B0, net_velocity, FVector);
				OFFSET(0x0618, fire_spread, double);
				OFFSET(0x0628, vertical_recoil, double);
				OFFSET(0x0630, horizontal_recoil, double);
				OFFSET(0x08D8, max_speed, double);
				OFFSET(0x0B38, recoil_offset, vector3);
				OFFSET(0x0B50, recoil_offset_target, vector3);
				OFFSET(0x1008, final_recoil, vector3);
				OFFSET(0x1028, shot_spread, double);
				OFFSET(0x07B0, walk_spread, double);
				OFFSET(0x07B8, jump_spread, double);
				OFFSET(0x0BF8, walk_spread_ratio, double);
				OFFSET(0x07C0, walk_spread_target, double);
				OFFSET(0x0A20, lateral_spread, double);
				OFFSET(0x0B90, mec_speed, double);
				OFFSET(0x0620, recovery, double);
				OFFSET(0x0DE0, friction, double);
				OFFSET(0x0980, body_armor_color, double);
				OFFSET(0x0CAB, request_fly, bool);
				OFFSET(0x1060, lock_movements, bool);
				OFFSET(0x06B8, move_input, FVector);
				OFFSET(0x06B1, move_input_state, int);
				OFFSET(0x06D0, net_move_input_state, int);
				OFFSET(0x0EC4, voice_steps, int32_t);
				OFFSET(0x0EC8, max_voice_steps, int32_t);
				OFFSET(0x0598, net_location, FVector);
				OFFSET(0x1144, skin_color, int32_t);
				OFFSET(0x0880, hand_state, FStr_ItemState);
				OFFSET(0x08AC, net_item_state, FStr_ItemState);
				OFFSET(0x0960, net_cammo, double);
				OFFSET(0x0890, bag_state, FStr_ItemState);
				OFFSET(0x0A40, player_data, u_data_player*);
				OFFSET(0x06A0, hand_item, u_data_item*);
				OFFSET(0x0848, net_hand_item, u_data_item*);
				OFFSET(0x10E8, net_hand_item_new, FStr_Item);
				OFFSET(0x0888, bag_item, u_data_item*);
				OFFSET(0x1178, net_bag_item, u_data_item*);
				OFFSET(0x1198, net_bag_item_new, FStr_Item);
				OFFSET(0x03B0, camera, u_camera_component*);
				OFFSET(0x0398, absolute_rotation, u_scene_component*);
				OFFSET(0x0430, orientation, u_scene_component*);
				OFFSET(0x1250, skin_save, u_save_skin*);
				OFFSET(0x11E8, skin_set, FStr_SkinSet);
				OFFSET(0x0418, body_collider, u_capsule_component*);
				OFFSET(0x0420, head_collider, u_sphere_component*);
				OFFSET(0x0388, ghost_root, u_scene_component*);
				OFFSET(0x0820, audio_voice, a_voice_source*);
				OFFSET(0x0818, pc_ref, a_pc_c*);
				OFFSET(0x1328, net_fish, int);
				OFFSET(0x1143, new_var, bool);
				OFFSET(0x900, heal_buff, int);
				OFFSET(0x1315, wink_lock, bool);
				OFFSET(0x1142, net_emote_primary, bool);

				a_character* get_player_character() {
					return reinterpret_cast<a_character*>(this);
				};
			};
		}
	}
}

#undef pthis