#include "menu.h"
#include <Windows.h>
#include "overlay/imgui/imgui.h"
#include "config.h"
#include "globals.h"

using namespace globals;
using namespace config;

void menu::draw()
{
	static bool menu_open = true;
	if (GetAsyncKeyState(VK_F1) & 1) {
		menu_open = !menu_open;
	}

	if (menu_open) {

		ImGui::Begin("Hawk Tuah Protocol", 0, ImGuiWindowFlags_AlwaysAutoResize);

		// imvec4 - rbga - 1.0 = 100% (or 255) 
		ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.1059f, 0.3765f, 0.6510f, 1.0f));

		if (ImGui::CollapsingHeader("ESP", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Checkbox("[F2] Player ESP", &player_esp);
			ImGui::Checkbox("[F3] Weapon ESP", &weapon_esp);
			ImGui::Checkbox("[F4] Object ESP", &primary_object_esp);
			ImGui::Checkbox("[F5] Other Object ESP", &secondary_object_esp);
		}
		if (ImGui::CollapsingHeader("PLAYER", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Checkbox("[F6] Speedhack", &speedhack);
			ImGui::Checkbox("[F7] God Mode", &god_mode);
			ImGui::Checkbox("[F8] Infinite Stamina", &infinite_stamina);
			ImGui::Checkbox("[F9] Revive", &living_state);
		}

		if (ImGui::CollapsingHeader("WEAPON", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Checkbox("Fast Melee", &fast_melee);
			ImGui::Checkbox("Infinite Melee Range", &infinite_melee_range);
			ImGui::Separator();
			ImGui::Checkbox("Auto Fire", &auto_fire);
			ImGui::Checkbox("Rapid Fire", &rapid_fire);
			ImGui::Checkbox("No Recoil", &no_recoil);
			ImGui::Checkbox("Max Damage", &max_damage);
			ImGui::Checkbox("Infinite Ammo", &infinite_ammo);
		}
		if (ImGui::CollapsingHeader("ITEMS", ImGuiTreeNodeFlags_DefaultOpen)) {
			auto hand_item = local_mec->get_hand_item();
			auto bag_item = local_mec->get_bag_item();
			auto hand_inventory = hand_item->get_can_inventory();

			if (hand_item) {
				ImGui::Text("Hand Item: %s", hand_item->get_name().read_string().c_str());
				auto hand_state = local_mec->get_hand_state();
				//std::cout << "Hand item changed: " << hand_item->get_name().read_string().c_str() << " - " << hand_state.Value_8 << " | " << hand_state.Time_15 << std::endl;

				if (hand_item->get_name().read_string() == "GAZ BOTTLE") {
					const char* colors[] = { "Yellow", "Red", "Blue" };
					int current_value = hand_state.Value_8;

					ImGui::SetNextItemWidth(75.0f);
					if (ImGui::Combo("##Color", &current_value, colors, IM_ARRAYSIZE(colors))) {
						hand_state.Value_8 = current_value;
						local_mec->set_hand_state(hand_state);
					}
				}
				else if (hand_item->get_name().read_string() == "PACKAGE") {
					const char* package_types[] = { "Security", "Computers", "Botanic", "Restaurant", "Medical", "Tutorial" , "Machine" };
					int package_value = hand_state.Value_8 - 1;

					ImGui::SetNextItemWidth(95.0f);
					if (ImGui::Combo("##PackageTypes", &package_value, package_types, IM_ARRAYSIZE(package_types))) {
						hand_state.Value_8 = package_value + 1;
						local_mec->set_hand_state(hand_state);
					}
				}
				else if (hand_item->get_name().read_string() == "RICE") {
					const char* rice_types[] = { "White", "Brown", "Black" };
					int rice_value = hand_state.Value_8 - 1;

					ImGui::SetNextItemWidth(75.0f);
					if (ImGui::Combo("##RiceType", &rice_value, rice_types, IM_ARRAYSIZE(rice_types))) {
						hand_state.Value_8 = rice_value + 1;
						local_mec->set_hand_state(hand_state);
					}
				}
				else if (hand_item->get_name().read_string() == "CONTAINER") {
					const char* colors[] = { "Empty", "Green", "Yellow", "Blue", "White", "Red" };
					int current_value = hand_state.Value_8;

					ImGui::SetNextItemWidth(75.0f);
					if (ImGui::Combo("##Color", &current_value, colors, IM_ARRAYSIZE(colors))) {
						hand_state.Value_8 = current_value;
						local_mec->set_hand_state(hand_state);
					}
				}
				else if (hand_item->get_name().read_string() == "SAMPLE") {
					const char* colors[] = { "Green", "Yellow", "Blue", "White", "Red" };
					int current_index = hand_state.Value_8 - 1;

					ImGui::SetNextItemWidth(75.0f);
					if (ImGui::Combo("##SampleColor", &current_index, colors, IM_ARRAYSIZE(colors))) {
						hand_state.Value_8 = current_index + 1;
						local_mec->set_hand_state(hand_state);
					}
				}
				else if (hand_item->get_name().read_string() == "FUSE") {
					const char* fuse_colors[] = { "Red", "Yellow", "Blue" };
					int time_color = hand_state.Time_15 - 1;
					int value_color = hand_state.Value_8 - 1;

					ImGui::SetNextItemWidth(75.0f);
					if (ImGui::Combo("##ValueColor", &value_color, fuse_colors, IM_ARRAYSIZE(fuse_colors))) {
						hand_state.Value_8 = value_color + 1;
						local_mec->set_hand_state(hand_state);
					}

					ImGui::SameLine();
					ImGui::SetNextItemWidth(75.0f);
					if (ImGui::Combo("##TimeColor", &time_color, fuse_colors, IM_ARRAYSIZE(fuse_colors))) {
						hand_state.Time_15 = time_color + 1;
						local_mec->set_hand_state(hand_state);
					}
				}
				else if (hand_item->get_name().read_string() == "VENT FILTER") {
					int clean_percentage = hand_state.Value_8;

					ImGui::SetNextItemWidth(90.0f);
					if (ImGui::SliderInt("Clean %", &clean_percentage, 0, 100)) {
						hand_state.Value_8 = clean_percentage;
						local_mec->set_hand_state(hand_state);
					}
				}
				else if (hand_item->get_name().read_string() == "BATTERY") {
					int charge_percentage = hand_state.Value_8;

					ImGui::SetNextItemWidth(90.0f);
					if (ImGui::SliderInt("Charge %", &charge_percentage, 0, 100)) {
						hand_state.Value_8 = charge_percentage;
						local_mec->set_hand_state(hand_state);
					}
				}
			}
			else {
				ImGui::Text("Hand Item: AIR");
			}
			if (bag_item) {
				ImGui::Text("bag Item: %s", bag_item->get_name().read_string().c_str());
				auto bag_state = local_mec->get_bag_state();
				//std::cout << "bag item changed: " << bag_item->get_name().read_string().c_str() << " - " << bag_state.Value_8 << " | " << bag_state.Time_15 << std::endl;

				if (bag_item->get_name().read_string() == "GAZ BOTTLE") {
					const char* colors[] = { "Yellow", "Red", "Blue" };
					int current_value = bag_state.Value_8;

					ImGui::SetNextItemWidth(75.0f);
					if (ImGui::Combo("##Color", &current_value, colors, IM_ARRAYSIZE(colors))) {
						bag_state.Value_8 = current_value;
						local_mec->set_bag_state(bag_state);
					}
				}
				else if (bag_item->get_name().read_string() == "PACKAGE") {
					const char* package_types[] = { "Security", "Computers", "Botanic", "Restaurant", "Medical", "Tutorial" , "Machine" };
					int package_value = bag_state.Value_8 - 1;

					ImGui::SetNextItemWidth(95.0f);
					if (ImGui::Combo("##PackageTypes", &package_value, package_types, IM_ARRAYSIZE(package_types))) {
						bag_state.Value_8 = package_value + 1;
						local_mec->set_bag_state(bag_state);
					}
				}
				else if (bag_item->get_name().read_string() == "RICE") {
					const char* rice_types[] = { "White", "Brown", "Black" };
					int rice_value = bag_state.Value_8 - 1;

					ImGui::SetNextItemWidth(75.0f);
					if (ImGui::Combo("##RiceType", &rice_value, rice_types, IM_ARRAYSIZE(rice_types))) {
						bag_state.Value_8 = rice_value + 1;
						local_mec->set_bag_state(bag_state);
					}
				}
				else if (bag_item->get_name().read_string() == "CONTAINER") {
					const char* colors[] = { "Empty", "Green", "Yellow", "Blue", "White", "Red" };
					int current_value = bag_state.Value_8;

					ImGui::SetNextItemWidth(75.0f);
					if (ImGui::Combo("##Color", &current_value, colors, IM_ARRAYSIZE(colors))) {
						bag_state.Value_8 = current_value;
						local_mec->set_bag_state(bag_state);
					}
				}
				else if (bag_item->get_name().read_string() == "SAMPLE") {
					const char* colors[] = { "Green", "Yellow", "Blue", "White", "Red" };
					int current_index = bag_state.Value_8 - 1;

					ImGui::SetNextItemWidth(75.0f);
					if (ImGui::Combo("##SampleColor", &current_index, colors, IM_ARRAYSIZE(colors))) {
						bag_state.Value_8 = current_index + 1;
						local_mec->set_bag_state(bag_state);
					}
				}
				else if (bag_item->get_name().read_string() == "FUSE") {
					const char* fuse_colors[] = { "Red", "Yellow", "Blue" };
					int time_color = bag_state.Time_15 - 1;
					int value_color = bag_state.Value_8 - 1;

					ImGui::SetNextItemWidth(75.0f);
					if (ImGui::Combo("##ValueColor", &value_color, fuse_colors, IM_ARRAYSIZE(fuse_colors))) {
						bag_state.Value_8 = value_color + 1;
						local_mec->set_bag_state(bag_state);
					}

					ImGui::SameLine();
					ImGui::SetNextItemWidth(75.0f);
					if (ImGui::Combo("##TimeColor", &time_color, fuse_colors, IM_ARRAYSIZE(fuse_colors))) {
						bag_state.Time_15 = time_color + 1;
						local_mec->set_bag_state(bag_state);
					}
				}
				else if (bag_item->get_name().read_string() == "VENT FILTER") {
					int clean_percentage = bag_state.Value_8;

					ImGui::SetNextItemWidth(90.0f);
					if (ImGui::SliderInt("Clean %", &clean_percentage, 0, 100)) {
						bag_state.Value_8 = clean_percentage;
						local_mec->set_bag_state(bag_state);
					}
				}
				else if (bag_item->get_name().read_string() == "BATTERY") {
					int charge_percentage = bag_state.Value_8;

					ImGui::SetNextItemWidth(90.0f);
					if (ImGui::SliderInt("Charge %", &charge_percentage, 0, 100)) {
						bag_state.Value_8 = charge_percentage;
						local_mec->set_bag_state(bag_state);
					}
				}
			}
			else {
				ImGui::Text("Bag Item: EMPTY");
			}

			if (!hand_inventory) {
				if (ImGui::Button("Can Inventory")) {
					if (hand_item) {
						hand_item->set_can_inventory(!hand_inventory);
					}
				}
			}
			else {
				if (ImGui::Button("Can't Inventory")) {
					if (hand_item) {
						hand_item->set_can_inventory(!hand_inventory);
					}
				}
			}
		}
		ImGui::Text("[        ONI EDIT        ]");

		// Revert to the default style color
		ImGui::PopStyleColor();

		ImGui::End();
	}
}
