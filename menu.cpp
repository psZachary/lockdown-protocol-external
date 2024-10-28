#include "menu.h"
#include <Windows.h>
#include "overlay/imgui/imgui.h"
#include "config.h"
#include "globals.h"
#include "util.hpp"

using namespace globals;
using namespace config;

int selected_tab = 1;

void menu::draw()
{
	static bool menu_open = true;
	if (GetAsyncKeyState(VK_F1) & 1) {
		menu_open = !menu_open;
	}

	if (menu_open) {
		ImVec2 startPosition = ImVec2(20, 20);

		ImGui::SetNextWindowPos(startPosition, true ? ImGuiCond_Once : ImGuiCond_Always);

		ImGui::Begin("Hawk Tuah Protocol"); 
		auto cursor_position = util::cursor_position();
		ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(cursor_position.x, cursor_position.y), 5.f, IM_COL32(255,255,255,255));
		ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.1059f, 0.3765f, 0.6510f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.1059f, 0.3765f, 0.6510f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.1059f, 0.3765f, 0.6510f, 1.0f));

		float calculatedHeight = 0.0f;
		float itemHeight = ImGui::GetFrameHeightWithSpacing();
		float headerPadding = 15.0f;

		ImGui::BeginChild("Sidebar", ImVec2(75, 0), true);
		if (ImGui::Selectable("ESP", selected_tab == 1)) selected_tab = 1;
		if (ImGui::Selectable("PLAYER", selected_tab == 2)) selected_tab = 2;
		if (ImGui::Selectable("WEAPON", selected_tab == 3)) selected_tab = 3;
		if (ImGui::Selectable("ITEMS", selected_tab == 4)) selected_tab = 4;
		ImGui::EndChild();

		ImGui::SameLine();
		ImGui::BeginChild("Content");

		if (selected_tab == 1) { // ESP
			calculatedHeight += itemHeight + headerPadding;
			ImGui::Checkbox("[F2] Player ESP", &player_esp); calculatedHeight += itemHeight;
			ImGui::Checkbox("[F3] Weapon ESP", &weapon_esp); calculatedHeight += itemHeight;
			if (weapon_esp) {
				ImGui::Indent();
				ImGui::Checkbox("Show Details##weapon", &weapon_details);
				calculatedHeight += itemHeight;
				ImGui::Unindent();
			}
			ImGui::Checkbox("[F4] Object ESP", &primary_object_esp); calculatedHeight += itemHeight;
			if (primary_object_esp) {
				ImGui::Indent();
				ImGui::Checkbox("Show Details##primary", &primary_object_details);
				calculatedHeight += itemHeight;
				ImGui::Unindent();
			}
			ImGui::Checkbox("[F5] Other Object ESP", &secondary_object_esp); calculatedHeight += itemHeight;
			if (secondary_object_esp) {
				ImGui::Indent();
				ImGui::Checkbox("Show Details##secondary", &secondary_object_details);
				calculatedHeight += itemHeight;
				ImGui::Unindent();
			}
		}
		else if (selected_tab == 2) { // PLAYER
			calculatedHeight += itemHeight + headerPadding;
			ImGui::Checkbox("[F6] Speedhack", &speedhack); calculatedHeight += itemHeight;
			ImGui::Checkbox("[F7] God Mode", &god_mode); calculatedHeight += itemHeight;
			ImGui::Checkbox("[F8] Infinite Stamina", &infinite_stamina); calculatedHeight += itemHeight;
			ImGui::Checkbox("[F9] Revive", &living_state); calculatedHeight += itemHeight;
		}
		else if (selected_tab == 3) { // WEAPON
			if (ImGui::CollapsingHeader("MELEE", ImGuiTreeNodeFlags_DefaultOpen)) {
				calculatedHeight += itemHeight + headerPadding;
				ImGui::Checkbox("Fast Melee", &fast_melee); calculatedHeight += itemHeight;
				ImGui::Checkbox("Infinite Melee Range", &infinite_melee_range); calculatedHeight += itemHeight;
			}
			calculatedHeight += itemHeight;
			if (ImGui::CollapsingHeader("GUN", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Checkbox("Auto Fire", &auto_fire); calculatedHeight += itemHeight;
				ImGui::Checkbox("Rapid Fire", &rapid_fire); calculatedHeight += itemHeight;
				ImGui::Checkbox("No Recoil", &no_recoil); calculatedHeight += itemHeight;
				ImGui::Checkbox("Max Damage", &max_damage); calculatedHeight += itemHeight;
				ImGui::Checkbox("Infinite Ammo", &infinite_ammo); calculatedHeight += itemHeight;
			}
			calculatedHeight += itemHeight;
		}
		else if (selected_tab == 4) { // ITEMS
			auto hand_item = local_mec->get_hand_item();
			auto bag_item = local_mec->get_bag_item();
			auto hand_inventory = hand_item->get_can_inventory();

			if (ImGui::CollapsingHeader("INVENTORY", ImGuiTreeNodeFlags_DefaultOpen)) {
				if (hand_item) {

					std::string hand_item_name = hand_item->get_name().read_string();

					std::string display_name;
					if (hand_item_name == "NAME") {
						display_name = "PIZZUSHI";
					}
					else {
						display_name = hand_item_name;
					}
					ImGui::Text("Hand Item: %s", display_name.c_str());
					calculatedHeight += itemHeight;

					auto hand_state = local_mec->get_hand_state();
					if (hand_item->get_name().read_string() == "GAZ BOTTLE") {
						const char* colors[] = { "Yellow", "Red", "Blue" };
						int current_value = hand_state.Value_8;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10);
						if (ImGui::Combo("##Color", &current_value, colors, IM_ARRAYSIZE(colors))) {
							hand_state.Value_8 = current_value;
							local_mec->set_hand_state(hand_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (hand_item->get_name().read_string() == "PACKAGE") {
						const char* package_types[] = { "Security", "Computers", "Botanic", "Restaurant", "Medical", "Tutorial" , "Machine" };
						int package_value = hand_state.Value_8 - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10);
						if (ImGui::Combo("##PackageTypes", &package_value, package_types, IM_ARRAYSIZE(package_types))) {
							hand_state.Value_8 = package_value + 1;
							local_mec->set_hand_state(hand_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (hand_item->get_name().read_string() == "FISH") {
						const char* fish_types[] = { "Salmon", "Tuna", "Cod", "Shrimp" };
						int fish_value = hand_state.Value_8 - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10);
						if (ImGui::Combo("##FishType", &fish_value, fish_types, IM_ARRAYSIZE(fish_types))) {
							hand_state.Value_8 = fish_value + 1;
							local_mec->set_hand_state(hand_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (hand_item->get_name().read_string() == "RICE") {
						const char* rice_types[] = { "White", "Brown", "Black" };
						int rice_value = hand_state.Value_8 - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10);
						if (ImGui::Combo("##RiceType", &rice_value, rice_types, IM_ARRAYSIZE(rice_types))) {
							hand_state.Value_8 = rice_value + 1;
							local_mec->set_hand_state(hand_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (hand_item->get_name().read_string() == "PACKAGE") {
						const char* package_types[] = { "Security", "Computers", "Botanic", "Restaurant", "Medical", "Tutorial" , "Machine" };
						int package_value = hand_state.Value_8 - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10);
						if (ImGui::Combo("##PackageTypes", &package_value, package_types, IM_ARRAYSIZE(package_types))) {
							hand_state.Value_8 = package_value + 1;
							local_mec->set_hand_state(hand_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (hand_item->get_name().read_string() == "FISH") {
						const char* fish_types[] = { "Salmon", "Tuna", "Cod", "Shrimp" };
						int fish_value = hand_state.Value_8 - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10);
						if (ImGui::Combo("##FishType", &fish_value, fish_types, IM_ARRAYSIZE(fish_types))) {
							hand_state.Value_8 = fish_value + 1;
							local_mec->set_hand_state(hand_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (hand_item->get_name().read_string() == "RICE") {
						const char* rice_types[] = { "White", "Brown", "Black" };
						int rice_value = hand_state.Value_8 - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10);
						if (ImGui::Combo("##RiceType", &rice_value, rice_types, IM_ARRAYSIZE(rice_types))) {
							hand_state.Value_8 = rice_value + 1;
							local_mec->set_hand_state(hand_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (hand_item->get_name().read_string() == "CONTAINER") {
						const char* colors[] = { "Empty", "Green", "Yellow", "Blue", "White", "Red", "White Rice", "Brown Rice", "Black Rice" };
						int current_value = hand_state.Value_8;
						int selected_index = current_value;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10);
						if (ImGui::Combo("##Color", &selected_index, colors, IM_ARRAYSIZE(colors))) {
							if (selected_index >= 6) {
								hand_state.Value_8 = 6;
								switch (selected_index) {
								case 6: hand_state.Time_15 = 1; break;
								case 7: hand_state.Time_15 = 2; break;
								case 8: hand_state.Time_15 = 3; break;
								}
							}
							else {
								hand_state.Value_8 = selected_index;
								hand_state.Time_15 = 0;
							}
							local_mec->set_hand_state(hand_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (hand_item->get_name().read_string() == "SAMPLE") {
						const char* colors[] = { "Green", "Yellow", "Blue", "White", "Red" };
						int current_index = hand_state.Value_8 - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10);
						if (ImGui::Combo("##SampleColor", &current_index, colors, IM_ARRAYSIZE(colors))) {
							hand_state.Value_8 = current_index + 1;
							local_mec->set_hand_state(hand_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (hand_item->get_name().read_string() == "FUSE") {
						const char* fuse_colors[] = { "Red", "Yellow", "Blue" };
						int time_color = hand_state.Time_15 - 1;
						int value_color = hand_state.Value_8 - 1;

						float half_width = (ImGui::GetContentRegionAvail().x - 10) / 2;

						ImGui::SetNextItemWidth(half_width);
						if (ImGui::Combo("##ValueColor", &value_color, fuse_colors, IM_ARRAYSIZE(fuse_colors))) {
							hand_state.Value_8 = value_color + 1;
							local_mec->set_hand_state(hand_state);
						}
						calculatedHeight += itemHeight;

						ImGui::SameLine();
						ImGui::SetNextItemWidth(half_width);
						if (ImGui::Combo("##TimeColor", &time_color, fuse_colors, IM_ARRAYSIZE(fuse_colors))) {
							hand_state.Time_15 = time_color + 1;
							local_mec->set_hand_state(hand_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (hand_item->get_name().read_string() == "NAME") {
						const char* rice_options[] = { "White Rice", "Brown Rice", "Black Rice" };
						const char* fish_options[] = { "Salmon", "Tuna", "Cod", "Shrimp" };
						const char* container_colors[] = { "Green", "Yellow", "Blue", "White", "Red" };

						int value = hand_state.Value_8;
						int rice_value = value / 100;
						int fish_value = (value / 10) % 10;
						int container_value = value % 10;

						int rice_index = rice_value - 1;
						int fish_index = fish_value - 1;
						int container_index = container_value - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10);
						if (ImGui::Combo("##Rice", &rice_index, rice_options, IM_ARRAYSIZE(rice_options))) {
							rice_value = rice_index + 1;
						}
						calculatedHeight += itemHeight;
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10);
						if (ImGui::Combo("##Fish", &fish_index, fish_options, IM_ARRAYSIZE(fish_options))) {
							fish_value = fish_index + 1;
						}
						calculatedHeight += itemHeight;
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10);
						if (ImGui::Combo("##Container Color", &container_index, container_colors, IM_ARRAYSIZE(container_colors))) {
							container_value = container_index + 1;
						}
						calculatedHeight += itemHeight;

						hand_state.Value_8 = (rice_value * 100) + (fish_value * 10) + container_value;
						local_mec->set_hand_state(hand_state);
					} // Pizzushi
					else if (hand_item->get_name().read_string() == "VENT FILTER") {
						int clean_percentage = hand_state.Value_8;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 20);
						if (ImGui::SliderInt("%", &clean_percentage, 0, 100)) {
							hand_state.Value_8 = clean_percentage;
							local_mec->set_hand_state(hand_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (hand_item->get_name().read_string() == "BATTERY") {
						int charge_percentage = hand_state.Value_8;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 20);
						if (ImGui::SliderInt("%", &charge_percentage, 0, 100)) {
							hand_state.Value_8 = charge_percentage;
							local_mec->set_hand_state(hand_state);
						}
						calculatedHeight += itemHeight;
					}
				}
				else {
					ImGui::Text("Hand Item: AIR");
					calculatedHeight += itemHeight + 10;
				}
				if (bag_item) {
					std::string bag_item_name = bag_item->get_name().read_string();

					std::string display_name;
					if (bag_item_name == "NAME") {
						display_name = "PIZZUSHI";
					}
					else {
						display_name = bag_item_name;
					}
					ImGui::Text("Bag Item: %s", display_name.c_str());
					calculatedHeight += itemHeight;

					auto bag_state = local_mec->get_bag_state();
					if (bag_item->get_name().read_string() == "GAZ BOTTLE") {
						const char* colors[] = { "Yellow", "Red", "Blue" };
						int current_value = bag_state.Value_8;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10);
						if (ImGui::Combo("##Color", &current_value, colors, IM_ARRAYSIZE(colors))) {
							bag_state.Value_8 = current_value;
							local_mec->set_bag_state(bag_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (bag_item->get_name().read_string() == "PACKAGE") {
						const char* package_types[] = { "Security", "Computers", "Botanic", "Restaurant", "Medical", "Tutorial" , "Machine" };
						int package_value = bag_state.Value_8 - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10);
						if (ImGui::Combo("##PackageTypes", &package_value, package_types, IM_ARRAYSIZE(package_types))) {
							bag_state.Value_8 = package_value + 1;
							local_mec->set_bag_state(bag_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (bag_item->get_name().read_string() == "FISH") {
						const char* fish_types[] = { "Salmon", "Tuna", "Cod", "Shrimp" };
						int fish_value = bag_state.Value_8 - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10);
						if (ImGui::Combo("##FishType", &fish_value, fish_types, IM_ARRAYSIZE(fish_types))) {
							bag_state.Value_8 = fish_value + 1;
							local_mec->set_bag_state(bag_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (bag_item->get_name().read_string() == "RICE") {
						const char* rice_types[] = { "White", "Brown", "Black" };
						int rice_value = bag_state.Value_8 - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10);
						if (ImGui::Combo("##RiceType", &rice_value, rice_types, IM_ARRAYSIZE(rice_types))) {
							bag_state.Value_8 = rice_value + 1;
							local_mec->set_bag_state(bag_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (bag_item->get_name().read_string() == "CONTAINER") {
						const char* colors[] = { "Empty", "Green", "Yellow", "Blue", "White", "Red", "White Rice", "Brown Rice", "Black Rice" };
						int current_value = bag_state.Value_8;
						int selected_index = current_value;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10);
						if (ImGui::Combo("##Color", &selected_index, colors, IM_ARRAYSIZE(colors))) {
							if (selected_index >= 6) {
								bag_state.Value_8 = 6;
								switch (selected_index) {
								case 6: bag_state.Time_15 = 1; break;
								case 7: bag_state.Time_15 = 2; break;
								case 8: bag_state.Time_15 = 3; break;
								}
							}
							else {
								bag_state.Value_8 = selected_index;
								bag_state.Time_15 = 0;
							}
							local_mec->set_bag_state(bag_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (bag_item->get_name().read_string() == "SAMPLE") {
						const char* colors[] = { "Green", "Yellow", "Blue", "White", "Red" };
						int current_index = bag_state.Value_8 - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10);
						if (ImGui::Combo("##SampleColor", &current_index, colors, IM_ARRAYSIZE(colors))) {
							bag_state.Value_8 = current_index + 1;
							local_mec->set_bag_state(bag_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (bag_item->get_name().read_string() == "FUSE") {
						const char* fuse_colors[] = { "Red", "Yellow", "Blue" };
						int time_color = bag_state.Time_15 - 1;
						int value_color = bag_state.Value_8 - 1;

						float half_width = (ImGui::GetContentRegionAvail().x - 10) / 2;

						ImGui::SetNextItemWidth(half_width);
						if (ImGui::Combo("##ValueColor", &value_color, fuse_colors, IM_ARRAYSIZE(fuse_colors))) {
							bag_state.Value_8 = value_color + 1;
							local_mec->set_bag_state(bag_state);
						}
						calculatedHeight += itemHeight;

						ImGui::SameLine();
						ImGui::SetNextItemWidth(half_width);
						if (ImGui::Combo("##TimeColor", &time_color, fuse_colors, IM_ARRAYSIZE(fuse_colors))) {
							bag_state.Time_15 = time_color + 1;
							local_mec->set_bag_state(bag_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (bag_item->get_name().read_string() == "NAME") { // Pizzushi
						const char* rice_options[] = { "White Rice", "Brown Rice", "Black Rice" };
						const char* fish_options[] = { "Salmon", "Tuna", "Cod", "Shrimp" };
						const char* container_colors[] = { "Green", "Yellow", "Blue", "White", "Red" };

						int value = bag_state.Value_8;
						int rice_value = value / 100;
						int fish_value = (value / 10) % 10;
						int container_value = value % 10;

						int rice_index = rice_value - 1;
						int fish_index = fish_value - 1;
						int container_index = container_value - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10);
						if (ImGui::Combo("##Rice", &rice_index, rice_options, IM_ARRAYSIZE(rice_options))) {
							rice_value = rice_index + 1;
						}
						calculatedHeight += itemHeight;
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10);
						if (ImGui::Combo("##Fish", &fish_index, fish_options, IM_ARRAYSIZE(fish_options))) {
							fish_value = fish_index + 1;
						}
						calculatedHeight += itemHeight;
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10);
						if (ImGui::Combo("##Container Color", &container_index, container_colors, IM_ARRAYSIZE(container_colors))) {
							container_value = container_index + 1;
						}
						calculatedHeight += itemHeight;

						bag_state.Value_8 = (rice_value * 100) + (fish_value * 10) + container_value;
						local_mec->set_bag_state(bag_state);
					}
					else if (bag_item->get_name().read_string() == "VENT FILTER") {
						int clean_percentage = bag_state.Value_8;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 20);
						if (ImGui::SliderInt("%", &clean_percentage, 0, 100)) {
							bag_state.Value_8 = clean_percentage;
							local_mec->set_bag_state(bag_state);
						}
						if (!ImGui::IsItemDeactivatedAfterEdit())
							calculatedHeight += itemHeight;
					}
					else if (bag_item->get_name().read_string() == "BATTERY") {
						int charge_percentage = bag_state.Value_8;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 20);
						if (ImGui::SliderInt("%", &charge_percentage, 0, 100)) {
							bag_state.Value_8 = charge_percentage;
							local_mec->set_bag_state(bag_state);
						}
						calculatedHeight += itemHeight;
					}
				}
				else {
					ImGui::Text("Bag Item: EMPTY");
					calculatedHeight += itemHeight + 10;
				}
				calculatedHeight += itemHeight;
			}
			calculatedHeight += itemHeight;

			if (hand_item) {
				if (!hand_inventory) {
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10);
					if (ImGui::Button("Can Inventory", ImVec2(ImGui::GetContentRegionAvail().x - 10, 0))) {
						if (hand_item) {
							hand_item->set_can_inventory(!hand_inventory);
						}
					}
					calculatedHeight += itemHeight;
				}
				else {
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10);
					if (ImGui::Button("Can't Inventory", ImVec2(ImGui::GetContentRegionAvail().x - 10, 0))) {
						if (hand_item) {
							hand_item->set_can_inventory(!hand_inventory);
						}
					}
					calculatedHeight += itemHeight;
				}
			}
		}

		ImGui::EndChild();

		//ImGui::SetWindowSize(ImVec2(-1, calculatedHeight));
		if (calculatedHeight <= 200.0f) {
			ImGui::SetWindowSize(ImVec2(300.0f, 200.0f));
		}
		else {
			ImGui::SetWindowSize(ImVec2(300.0f, calculatedHeight));
		}


		ImGui::PopStyleColor(3);

		ImGui::End();
	}
}
