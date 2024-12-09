#include "menu.h"
#include <string>
#include <vector>
#include <Windows.h>
#include "overlay/imgui/imgui.h"
#include "config.h"
#include "globals.h"
#include "keybinds.hpp"
#include "data_cache.h"
#include <unordered_set>
#include "util.hpp"
#include "game_function.hpp"

using namespace globals;
using namespace config;

int selected_tab = 1;

// Global containers for item names and item data mapping
std::vector<std::string> item_names;
std::unordered_map<std::string, u_data_item*> unique_item_data;

void MenuTooltip(const char* text) {
	ImGui::SameLine();
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(450.0f);
		ImGui::TextUnformatted(text);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void PopulateUniqueItems() {
	static std::unordered_set<std::string> seen_names;

	for (auto& item : world_item_cache) {
		if (!item) continue;

		auto item_data = item->get_data();
		std::string name = item_data->get_name().read_string();

		if (seen_names.find(name) != seen_names.end()) {
			continue;
		}

		seen_names.insert(name);
		unique_item_data[name] = item_data;
		item_names.push_back(name);
	}
}

std::string MenuCalculateDistance(const FVector& location1, const vector3& location2) {
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

double MenuCalculateDistanceMeters(const vector3& location1, const vector3& location2) {
	double dx = location1.x - location2.x;
	double dy = location1.y - location2.y;
	double dz = location1.z - location2.z;

	return std::sqrt(dx * dx + dy * dy + dz * dz) / 100.0;
}

void menu::draw()
{
	static std::string selected_item_name = "";

	static bool menu_open = true;

	if (GetAsyncKeyState(menu_hotkey) & 1) {
		menu_open = !menu_open;
	}

	if (GetAsyncKeyState(player_list_hotkey) & 1) {
		player_list = !player_list;
	}

	if (menu_open) {
		ImVec2 startPosition = ImVec2(20, 20);

		ImGui::SetNextWindowPos(startPosition, true ? ImGuiCond_Once : ImGuiCond_Always);

		ImGui::Begin("Hawk Tuah Protocol - Oni Edition v3.1 UNRELEASED");

		auto cursor_position = util::cursor_position();
		ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(cursor_position.x, cursor_position.y), 5.f, IM_COL32(255, 255, 255, 255));

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
		ImGui::Separator();
		ImGui::Text("SETTINGS");
		if (ImGui::Button("Save")) {
			SaveConfig();
		}
		if (ImGui::Button("Reload")) {
			LoadConfig();
		}
		ImGui::EndChild();

		ImGui::SameLine();
		ImGui::BeginChild("Content");

		// ESP
		if (selected_tab == 1) {
			calculatedHeight += itemHeight + headerPadding;

			if (ImGui::Checkbox("ESP##Master", &esp_enabled)) {
				player_esp = esp_enabled;
				weapon_esp = esp_enabled;
				task_object_esp = esp_enabled;
				primary_object_esp = esp_enabled;
				secondary_object_esp = esp_enabled;
				esp_radar = esp_enabled;
				weapon_case_esp = esp_enabled;
			}
			ImGui::SameLine();
			ImHotkey("##ESPHotkey", &esp_hotkey);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			double min_distance = 0.0;       // Minimum value
			double max_distance = 200.0;   // Maximum value

			ImGui::SliderScalar("##esp_range", ImGuiDataType_Double, &esp_max_distance, &min_distance, &max_distance, "Distance: %.2f");

			calculatedHeight += itemHeight;

			if (esp_enabled) {

				// Tabs for each ESP category
				if (ImGui::BeginTabBar("ESPTabBar")) {

					// Player ESP Tab
					if (ImGui::BeginTabItem("Player")) {
						ImGui::Checkbox("Enabled##PlayerESP", &player_esp);
						ImGui::SameLine();
						ImGui::Checkbox("Show on Radar##PlayerRadar", &player_radar);

						// Begin custom side-by-side child sections
						float halfWidth = (ImGui::GetContentRegionAvail().x) / 2;
						ImGui::BeginChild("DetailsSection", ImVec2(halfWidth, 0), true);

						if (ImGui::CollapsingHeader("Details##PlayerESPDetails", ImGuiTreeNodeFlags_DefaultOpen)) {
							ImGui::Checkbox("Distance##PlayerDistance", &player_distance);
							ImGui::SameLine();
							ImGui::Checkbox("Inventory##PlayerInventory", &player_inventory);
							ImGui::Checkbox("Box##PlayerBox", &player_box);
							ImGui::Separator();
							ImGui::Checkbox("Ghost##PlayerGhost", &player_ghost);
							ImGui::Checkbox("Show Ghost on Radar##PlayerRadar", &ghost_radar);
							ImGui::Separator();
							ImGui::Checkbox("Player List##PlayerList", &player_list);
							ImGui::SameLine();
							ImHotkey("##PlyaerListHotkey", &player_list_hotkey);
						}
						ImGui::EndChild();

						ImGui::SameLine();
						ImGui::BeginChild("ColorsSection", ImVec2(halfWidth - 10, 0), true);
						if (ImGui::CollapsingHeader("Colors##PlayerESPColors", ImGuiTreeNodeFlags_DefaultOpen)) {
							ImGui::ColorEdit4("Employee Color", (float*)&employee_color, ImGuiColorEditFlags_AlphaBar);
							ImGui::ColorEdit4("Dissident Color", (float*)&dissident_color, ImGuiColorEditFlags_AlphaBar);
							if (player_ghost) {
								ImGui::ColorEdit4("Employee Ghost Color", (float*)&ghost_employee_color, ImGuiColorEditFlags_AlphaBar);
								ImGui::ColorEdit4("Dissident Ghost Color", (float*)&ghost_dissident_color, ImGuiColorEditFlags_AlphaBar);
							}
						}
						ImGui::EndChild();

						calculatedHeight += itemHeight * 9.5;

						ImGui::EndTabItem();
					}

					// World ESP Tab
					if (ImGui::BeginTabItem("World")) {
						ImGui::Checkbox("Enabled##WorldESP", &task_object_esp);
						ImGui::SameLine(); MenuTooltip("Only works in self hosted lobbies.");

						// Begin custom side-by-side child sections
						float halfWidth = (ImGui::GetContentRegionAvail().x) / 2;

						// Details Section
						ImGui::BeginChild("DetailsSection", ImVec2(halfWidth, 0), true);
						if (ImGui::CollapsingHeader("Details##WorldESPDetails", ImGuiTreeNodeFlags_DefaultOpen)) {
							ImGui::Checkbox("Task State##TaskItemState", &task_object_state);
							ImGui::Checkbox("Distance##TaskDistance", &task_object_distance);
						}
						if (ImGui::CollapsingHeader("Colors##WorldESPColors", ImGuiTreeNodeFlags_DefaultOpen)) {
							ImGui::ColorEdit4("Delivery Color", (float*)&task_delivery_color, ImGuiColorEditFlags_AlphaBar);
							ImGui::ColorEdit4("Pressure Color", (float*)&task_machine_color, ImGuiColorEditFlags_AlphaBar);
							ImGui::ColorEdit4("Vent Color", (float*)&task_vent_color, ImGuiColorEditFlags_AlphaBar);
							ImGui::ColorEdit4("Alimentation Color", (float*)&task_alim_color, ImGuiColorEditFlags_AlphaBar);
							ImGui::ColorEdit4("Pizzushi Color", (float*)&task_pizzushi_color, ImGuiColorEditFlags_AlphaBar);
							ImGui::ColorEdit4("Computers Color", (float*)&task_computer_color, ImGuiColorEditFlags_AlphaBar);
							ImGui::ColorEdit4("Scanner Color", (float*)&task_scanner_color, ImGuiColorEditFlags_AlphaBar);
						}
						ImGui::EndChild();

						ImGui::SameLine();

						// Task Locations Section
						ImGui::BeginChild("TaskLocationsSection", ImVec2(halfWidth - 10, 0), true);
						if (ImGui::CollapsingHeader("Task Locations##WorldESPTasks", ImGuiTreeNodeFlags_DefaultOpen)) {
							ImGui::Checkbox("Delivery##TaskLocation", &task_delivery);
							ImGui::Checkbox("Pressure##TaskLocation", &task_machine);
							ImGui::Checkbox("Vents##TaskLocation", &task_vent);
							ImGui::Checkbox("Alimentations##TaskLocation", &task_alim);
							ImGui::Checkbox("Pizzushi##TaskLocation", &task_pizzushi);
							ImGui::Checkbox("Computers##TaskLocation", &task_computers);
							ImGui::Checkbox("Scanner##TaskLocation", &task_scanners);
						}
						ImGui::EndChild();

						calculatedHeight += itemHeight * 13.5;

						ImGui::EndTabItem();
					}

					// Weapon ESP Tab
					if (ImGui::BeginTabItem("Weapon")) {
						ImGui::Checkbox("Enabled##WeaponESP", &weapon_esp);
						ImGui::SameLine();
						ImGui::Checkbox("Weapon Cases##WeaponCases", &weapon_case_esp);
						// Begin custom side-by-side child sections
						float halfWidth = (ImGui::GetContentRegionAvail().x) / 2;
						ImGui::BeginChild("DetailsSection", ImVec2(halfWidth, 0), true);
						if (ImGui::CollapsingHeader("Details##WeaponESPDetails", ImGuiTreeNodeFlags_DefaultOpen)) {
							ImGui::Checkbox("Item State##WeaponItemState", &weapon_item_state);
							ImGui::Checkbox("Distance##WeaponDistance", &weapon_distance);
							ImGui::Checkbox("Show on Radar##WeaponRadar", &weapon_radar);
							ImGui::Separator();
							ImGui::Checkbox("Case Code##WeaponCaseState", &weapon_case_state);
							ImGui::SameLine(); MenuTooltip("Only works in self hosted lobbies.");
							ImGui::Checkbox("Case Distance##WeaponCaseDistance", &weapon_case_distance);
							ImGui::Checkbox("Case Weapon##WeaponCaseInfo", &weapon_case_info);
						}
						ImGui::EndChild();

						ImGui::SameLine();
						ImGui::BeginChild("ColorsSection", ImVec2(halfWidth - 10, 0), true);
						if (ImGui::CollapsingHeader("Color##WeaponESPColors", ImGuiTreeNodeFlags_DefaultOpen)) {
							ImGui::ColorEdit4("Weapon Color", (float*)&weapon_color, ImGuiColorEditFlags_AlphaBar);
							ImGui::ColorEdit4("Weapon Case Color", (float*)&weapon_case_color, ImGuiColorEditFlags_AlphaBar);
						}
						ImGui::EndChild();

						calculatedHeight += itemHeight * 9.5;

						ImGui::EndTabItem();
					}

					// Primary Object ESP Tab
					if (ImGui::BeginTabItem("Object")) {
						ImGui::Checkbox("Enabled##PrimaryObjectESP", &primary_object_esp);

						// Begin custom side-by-side child sections
						float halfWidth = (ImGui::GetContentRegionAvail().x) / 2;
						ImGui::BeginChild("DetailsSection", ImVec2(halfWidth, 0), true);
						if (ImGui::CollapsingHeader("Details##PrimaryObjectDetails", ImGuiTreeNodeFlags_DefaultOpen)) {
							ImGui::Checkbox("Item State##PrimaryItemState", &primary_item_state);
							ImGui::Checkbox("Distance##PrimaryDistance", &primary_distance);
							ImGui::Checkbox("Show on Radar##PrimaryRadar", &primary_radar);
						}
						ImGui::EndChild();

						ImGui::SameLine();
						ImGui::BeginChild("ColorsSection", ImVec2(halfWidth - 10, 0), true);
						if (ImGui::CollapsingHeader("Colors##PrimaryObjectESPColors", ImGuiTreeNodeFlags_DefaultOpen)) {
							ImGui::ColorEdit4("Gaz Bottle Color", (float*)&gaz_bottle_color, ImGuiColorEditFlags_AlphaBar);
							ImGui::ColorEdit4("Vent Filter Color", (float*)&vent_filter_color, ImGuiColorEditFlags_AlphaBar);
							ImGui::ColorEdit4("Rice Color", (float*)&rice_color, ImGuiColorEditFlags_AlphaBar);
							ImGui::ColorEdit4("Package Color", (float*)&package_color, ImGuiColorEditFlags_AlphaBar);
							ImGui::ColorEdit4("Sample Color", (float*)&sample_color, ImGuiColorEditFlags_AlphaBar);
						}
						ImGui::EndChild();

						calculatedHeight += itemHeight * 8.5;

						ImGui::EndTabItem();
					}

					// Secondary Object ESP Tab
					if (ImGui::BeginTabItem("Other Object")) {
						ImGui::Checkbox("Enabled##SecondaryObjectESP", &secondary_object_esp);

						// Begin custom side-by-side child sections
						float halfWidth = (ImGui::GetContentRegionAvail().x) / 2;
						ImGui::BeginChild("DetailsSection", ImVec2(halfWidth, 0), true);
						if (ImGui::CollapsingHeader("Details##SecondaryObjectDetails", ImGuiTreeNodeFlags_DefaultOpen)) {
							ImGui::Checkbox("Item State##SecondaryItemState", &secondary_item_state);
							ImGui::Checkbox("Distance##SecondaryDistance", &secondary_distance);
							ImGui::Checkbox("Show on Radar##SecondaryRadar", &secondary_radar);
						}
						ImGui::EndChild();

						ImGui::SameLine();
						ImGui::BeginChild("ColorsSection", ImVec2(halfWidth - 10, 0), true);
						if (ImGui::CollapsingHeader("Colors##SecondaryObjectESPColors", ImGuiTreeNodeFlags_DefaultOpen)) {
							ImGui::ColorEdit4("Fuse Color", (float*)&fuse_color, ImGuiColorEditFlags_AlphaBar);
							ImGui::ColorEdit4("Battery Color", (float*)&battery_color, ImGuiColorEditFlags_AlphaBar);
							ImGui::ColorEdit4("Screw Driver Color", (float*)&screw_driver_color, ImGuiColorEditFlags_AlphaBar);
							ImGui::ColorEdit4("Container Color", (float*)&container_color, ImGuiColorEditFlags_AlphaBar);
						}
						ImGui::EndChild();

						calculatedHeight += itemHeight * 7.5;

						ImGui::EndTabItem();
					}

					// Radar
					if (ImGui::BeginTabItem("Radar")) {
						ImGui::Checkbox("Radar##ESPRadar", &esp_radar);

						if (ImGui::CollapsingHeader("Options##RadarOptions", ImGuiTreeNodeFlags_DefaultOpen)) {
							// Add radar position combo box here
							static const char* positionOptions[] = {
								"Top Left", "Top Middle", "Top Right",
								"Middle Left", "Middle Right",
								"Bottom Left", "Bottom Middle", "Bottom Right"
							};

							// Find the current index of the radar position
							int currentRadarPositionIndex = 0;
							for (int i = 0; i < IM_ARRAYSIZE(positionOptions); ++i) {
								if (esp_radar_position == positionOptions[i]) {
									currentRadarPositionIndex = i;
									break;
								}
							}

							// Create combo box
							ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
							if (ImGui::Combo("##RadarPosition", &currentRadarPositionIndex, positionOptions, IM_ARRAYSIZE(positionOptions))) {
								esp_radar_position = positionOptions[currentRadarPositionIndex]; // Update string on selection
							}

							float esp_radar_scale_float = static_cast<float>(esp_radar_scale);
							ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
							if (ImGui::SliderFloat("##esp_radar_scale", &esp_radar_scale_float, 0.05f, 0.25f, "Scale: %.02f")) {
								esp_radar_scale = static_cast<double>(esp_radar_scale_float);
							}
						}

						ImGui::EndTabItem();
					}

					ImGui::EndTabBar();
				}
			}
		}
		// PLAYER
		else if (selected_tab == 2) {
			// Begin custom side-by-side child sections
			float halfWidth = (ImGui::GetContentRegionAvail().x) / 2;
			ImGui::BeginChild("DetailsSection", ImVec2(halfWidth, 0), true);
			if (ImGui::CollapsingHeader("Toggle Hacks##PlayerToggleHacks", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Checkbox("Speedhack", &speedhack);
				ImGui::SameLine();
				ImHotkey("##SpeedhackHotkey", &speedhack_hotkey);

				ImGui::Checkbox("God Mode", &god_mode);
				ImGui::SameLine();
				ImHotkey("##GodmodeHotkey", &god_mode_hotkey);

				ImGui::Checkbox("Infinite Stamina", &infinite_stamina);
				ImGui::SameLine();
				ImHotkey("##InfstamHotkey", &infinite_stamina_hotkey);

				//ImGui::BeginDisabled(true);
				ImGui::Checkbox("Aimbot", &aimbot);
				ImGui::SameLine();
				ImHotkey("##AimbotHotkey", &aimbot_hotkey);
				ImGui::Text("Hold Key:");
				ImGui::SameLine();
				ImHotkey("##AimbotHoldKey", &aimbot_hold_key);
				//ImGui::EndDisabled();

				if (ImGui::Button("Revive")) {
					local_mec->set_alive(true);
					local_mec->set_health(100);
				}
			}
			ImGui::EndChild();

			ImGui::SameLine();

			ImGui::BeginChild("OthersSection", ImVec2(halfWidth - 10, 0), true);

			if (ImGui::CollapsingHeader("Details##PlayerHackDetails", ImGuiTreeNodeFlags_DefaultOpen)) {
				if (speedhack) {
					float max_speed_float = static_cast<float>(max_speed);
					float friction_float = static_cast<float>(friction);

					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					if (ImGui::SliderFloat("##max_speed", &max_speed_float, 0.0f, 5000.0f, "Max Speed: %.1f")) {
						max_speed = static_cast<double>(max_speed_float);
					}
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					if (ImGui::SliderFloat("##friction", &friction_float, 0.0f, 100000.0f, "Friction: %.1f")) {
						friction = static_cast<double>(friction_float);
					}
				}

				ImGui::Separator();

				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				if (ImGui::SliderInt("##fov", &player_fov, 20, 150, "FOV: %d")) {
					local_mec->get_camera()->set_field_of_view(player_fov);
				}
			}
			ImGui::EndChild();

			calculatedHeight += itemHeight * 9.5;
		}
		// WEAPON
		else if (selected_tab == 3) {
			// Begin custom side-by-side child sections
			float halfWidth = (ImGui::GetContentRegionAvail().x) / 2;
			ImGui::BeginChild("DetailsSection", ImVec2(halfWidth, 0), true);
			if (ImGui::CollapsingHeader("MELEE", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Checkbox("Fast Melee", &fast_melee);
				ImGui::SameLine();
				ImHotkey("##FastMeleeHotkey", &fast_melee_hotkey);

				ImGui::Checkbox("Long Melee Range", &infinite_melee_range);
				ImGui::SameLine();
				ImHotkey("##InfMeleeRangeHotkey", &infinite_melee_range_hotkey);
			}
			if (ImGui::CollapsingHeader("GUN", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Checkbox("Auto Fire", &auto_fire);
				ImGui::SameLine();
				ImHotkey("##AutoFireHotkey", &auto_fire_hotkey);

				ImGui::Checkbox("Rapid Fire", &rapid_fire);
				ImGui::SameLine();
				ImHotkey("##RapidFireHotkey", &rapid_fire_hotkey);

				ImGui::Checkbox("No Recoil", &no_recoil);
				ImGui::SameLine();
				ImHotkey("##NoRecoilHotkey", &no_recoil_hotkey);

				ImGui::Checkbox("Max Damage", &max_damage);
				ImGui::SameLine();
				ImHotkey("##MaxDamageHotkey", &max_damage_hotkey);

				ImGui::Checkbox("Infinite Ammo", &infinite_ammo);
			}
			ImGui::EndChild();

			ImGui::SameLine();

			ImGui::BeginChild("OthersSection", ImVec2(halfWidth - 10, 0), true);
			if (fast_melee || infinite_melee_range) {
				if (ImGui::CollapsingHeader("Melee Details##MeleeDetails", ImGuiTreeNodeFlags_DefaultOpen)) {
					if (fast_melee) {
						float cast_time_float = static_cast<float>(cast_time);
						float recover_time_float = static_cast<float>(recover_time);
						float stun_float = static_cast<float>(stun);

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::SliderFloat("##cast_time", &cast_time_float, 0.0f, 0.5f, "Cast Time: %.02f")) {
							cast_time = static_cast<double>(cast_time_float);
						}
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::SliderFloat("##recover_time", &recover_time_float, 0.0f, 1.0f, "Recover Time: %.02f")) {
							recover_time = static_cast<double>(recover_time_float);
						}
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::SliderFloat("##stun", &stun_float, 0.0f, 1.0f, "Stun: %.02f")) {
							stun = static_cast<double>(stun_float);
						}
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						ImGui::SliderInt("##cost", &cost, 0, 40, "Cost: %d");
					}
					if (infinite_melee_range) {
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						ImGui::SliderInt("##range", &range, 0, 10000, "Range: %d");
					}

				}
			}
			if (rapid_fire || no_recoil || infinite_ammo) {
				if (ImGui::CollapsingHeader("Gun Details##GunDetails", ImGuiTreeNodeFlags_DefaultOpen)) {
					if (rapid_fire) {
						float rapid_fire_rate_float = static_cast<float>(rapid_fire_rate);

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::SliderFloat("##rapid_fire_rate", &rapid_fire_rate_float, 0.0f, 1.0f, "Fire Rate: %.02f")) {
							rapid_fire_rate = static_cast<double>(rapid_fire_rate_float);
						}
					}
					if (no_recoil) {
						float movement_osc_float = static_cast<float>(movement_osc);
						float osc_reactivity_float = static_cast<float>(osc_reactivity);
						float movement_prec_float = static_cast<float>(movement_prec);
						float recoil_react_float = static_cast<float>(recoil_react);
						float shake_intensity_float = static_cast<float>(shake_intensity);
						float fire_spread_float = static_cast<float>(fire_spread);

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::SliderFloat("##movement_osc", &movement_osc_float, 0.0f, 20.0f, "Oscillation: %.02f")) {
							movement_osc = static_cast<double>(movement_osc_float);
						}
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::SliderFloat("##osc_reactivity", &osc_reactivity_float, 0.0f, 100.0f, "Osc. Reactivity: %.02f")) {
							osc_reactivity = static_cast<double>(osc_reactivity_float);
						}
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::SliderFloat("##movement_prec", &movement_prec_float, 0.0f, 40.0f, "Move Precision: %.02f")) {
							movement_prec = static_cast<double>(movement_prec_float);
						}
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::SliderFloat("##recoil_react", &recoil_react_float, 0.0f, 50.0f, "Recoil: %.02f")) {
							recoil_react = static_cast<double>(recoil_react_float);
						}
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::SliderFloat("##shake_intensity", &shake_intensity_float, 0.0f, 10.0f, "Shake Intensity: %.02f")) {
							shake_intensity = static_cast<double>(shake_intensity_float);
						}
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::SliderFloat("##fire_spread", &fire_spread_float, 0.0f, 20.0f, "Fire Spread: %.02f")) {
							fire_spread = static_cast<double>(fire_spread_float);
						}
					}
					if (infinite_ammo) {
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						ImGui::SliderInt("##ammo", &ammo_count, 0, 12, "Ammo: %d");
					}
				}
			}
			ImGui::EndChild();
			calculatedHeight += itemHeight * 12;
		}
		// ITEMS
		else if (selected_tab == 4) {
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
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 70);
					if (ImGui::BeginCombo("##ChangeHandItem", selected_item_name.empty() ? "Select Item" : selected_item_name.c_str())) {
						if (ImGui::Selectable("EMPTY", selected_item_name == "EMPTY")) {
							selected_item_name = "EMPTY";
							local_mec->set_hand_item(nullptr); // Clear the hand item
						}

						PopulateUniqueItems();

						// List of items that require AssignToItemData
						std::unordered_set<std::string> special_items = { "SHORTY", "PISTOL", "REVOLVER", "SMG", "RIFLE", "SHOTGUN", "DETONATOR", "C4" };

						// Ensure all special items are added to the list if missing
						for (const auto& special_item : special_items) {
							if (std::find(item_names.begin(), item_names.end(), special_item) == item_names.end()) {
								item_names.push_back(special_item); // Add missing special items
							}
						}

						// Now list all the items
						for (const auto& item_name : item_names) {
							if (ImGui::Selectable(item_name.c_str(), item_name == selected_item_name)) {
								selected_item_name = item_name;

								// Check if the item is one of the special ones
								if (special_items.find(item_name) != special_items.end()) {
									// Use AssignToItemData for these items
									auto item_data = AssignToItemData(item_name);
									if (item_data) {
										local_mec->set_hand_item(item_data);
									}
									else {
										std::cout << "Failed to assign item data for: " << item_name << std::endl;
									}
								}
								else {
									// Use unique_item_data for regular items
									auto item_data = unique_item_data[item_name];
									if (item_data) {
										local_mec->set_hand_item(item_data);
									}
								}

								// Reset combo box selection after changing the item
								selected_item_name = "";
							}
						}

						ImGui::EndCombo();
					}
					// Show the checkbox for locking
					ImGui::SameLine();
					if (ImGui::Checkbox("Lock", &lock_hand_item)) {
						if (lock_hand_item) {
							// Save the current hand item and state
							locked_hand_item = hand_item;
							locked_hand_state = local_mec->get_hand_state();
						}
						else {
							// Clear the locked data when unlocking
							locked_hand_item = nullptr;
						}
					}

					auto hand_state = local_mec->get_hand_state();
					if (hand_item_name == "GAZ BOTTLE") {
						const char* colors[] = { "Yellow", "Red", "Blue" };
						int current_value = hand_state.Value_8;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##HandColor", &current_value, colors, IM_ARRAYSIZE(colors))) {
							hand_state.Value_8 = current_value;
							local_mec->set_hand_state(hand_state);
						}
					}
					else if (hand_item_name == "PACKAGE") {
						const char* package_types[] = { "Security", "Computers", "Botanic", "Restaurant", "Medical", "Tutorial" , "Machine" };
						int package_value = hand_state.Value_8 - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##HandPackageTypes", &package_value, package_types, IM_ARRAYSIZE(package_types))) {
							hand_state.Value_8 = package_value + 1;
							local_mec->set_hand_state(hand_state);
						}
					}
					else if (hand_item_name == "FISH") {
						const char* fish_types[] = { "Salmon", "Tuna", "Cod", "Shrimp" };
						int fish_value = hand_state.Value_8 - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##HandFishType", &fish_value, fish_types, IM_ARRAYSIZE(fish_types))) {
							hand_state.Value_8 = fish_value + 1;
							local_mec->set_hand_state(hand_state);
						}
					}
					else if (hand_item_name == "RICE") {
						const char* rice_types[] = { "White", "Brown", "Black" };
						int rice_value = hand_state.Value_8 - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##HandRiceType", &rice_value, rice_types, IM_ARRAYSIZE(rice_types))) {
							hand_state.Value_8 = rice_value + 1;
							local_mec->set_hand_state(hand_state);
						}
					}
					else if (hand_item_name == "CONTAINER") {
						const char* colors[] = { "Empty", "Green", "Yellow", "Blue", "White", "Red", "White Rice", "Brown Rice", "Black Rice", "Dirty" };
						int current_value = (hand_state.Value_8 == -1) ? 9 : hand_state.Value_8; // Map -1 (Dirty) to the new index
						int selected_index = current_value;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##HandColor", &selected_index, colors, IM_ARRAYSIZE(colors))) {
							if (selected_index == 9) {
								// Handle "Dirty" selection
								hand_state.Value_8 = -1;
							}
							else if (selected_index >= 6) {
								hand_state.Value_8 = 6;
								switch (selected_index) {
								case 6: hand_state.Time_15 = 1; break;
								case 7: hand_state.Time_15 = 2; break;
								case 8: hand_state.Time_15 = 3; break;
								}
							}
							else {
								hand_state.Value_8 = selected_index;

								if (selected_index == 5) {
									hand_state.Time_15 = 1; // Set Time_15 to 1 for Red
								}
								else {
									hand_state.Time_15 = 0;
								}
							}
							local_mec->set_hand_state(hand_state);
						}
					}
					else if (hand_item_name == "SAMPLE") {
						const char* colors[] = { "Empty", "Green", "Yellow", "Blue", "White", "Red", "Green Mix", "Yellow Mix", "Blue Mix", "White Mix" };
						int current_index = (hand_state.Value_8 == 0) ? 0 :
							(hand_state.Time_15 == 1 ? hand_state.Value_8 + 5 : hand_state.Value_8);

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##HandSampleColor", &current_index, colors, IM_ARRAYSIZE(colors))) {
							if (current_index == 0) { // Empty
								hand_state.Value_8 = 0;
								hand_state.Time_15 = 0;
							}
							else if (current_index == 5) { // Red
								hand_state.Value_8 = 5;
								hand_state.Time_15 = 1;
							}
							else if (current_index >= 6) { // Mixes
								hand_state.Value_8 = current_index - 5; // Map mix index back to unmixed Value_8
								hand_state.Time_15 = 1; // Mixes have Time_15 of 1
							}
							else { // Standard colors
								hand_state.Value_8 = current_index;
								hand_state.Time_15 = 0; // Standard colors have Time_15 of 0
							}
							local_mec->set_hand_state(hand_state);
						}
					}
					else if (hand_item_name == "FUSE") {
						const char* fuse_colors[] = { "Red", "Yellow", "Blue" };
						int time_color = hand_state.Time_15 - 1;
						int value_color = hand_state.Value_8 - 1;

						float half_width = (ImGui::GetContentRegionAvail().x) / 2;

						ImGui::SetNextItemWidth(half_width);
						if (ImGui::Combo("##HandValueColor", &value_color, fuse_colors, IM_ARRAYSIZE(fuse_colors))) {
							hand_state.Value_8 = value_color + 1;
							local_mec->set_hand_state(hand_state);
						}

						ImGui::SameLine();
						ImGui::SetNextItemWidth(half_width);
						if (ImGui::Combo("##HandTimeColor", &time_color, fuse_colors, IM_ARRAYSIZE(fuse_colors))) {
							hand_state.Time_15 = time_color + 1;
							local_mec->set_hand_state(hand_state);
						}
					}
					else if (hand_item_name == "NAME") {
						const char* rice_options[] = { "White Rice", "Brown Rice", "Black Rice" };
						const char* fish_options[] = { "Salmon", "Tuna", "Cod", "Shrimp" };
						const char* container_colors[] = { "Green", "Yellow", "Blue", "White", "Red" };

						// Default value for Value_8 (1st option for rice, fish, and container)
						if (hand_state.Value_8 <= 0) {
							hand_state.Value_8 = 111; // Default to "White Rice", "Salmon", "Green"
							local_mec->set_hand_state(hand_state); // Ensure it's set in-game
						}

						int value = hand_state.Value_8;
						int rice_value = value / 100;
						int fish_value = (value / 10) % 10;
						int container_value = value % 10;

						// Initialize indices with default values (0 for the first option)
						int rice_index = (rice_value > 0) ? rice_value - 1 : 0;
						int fish_index = (fish_value > 0) ? fish_value - 1 : 0;
						int container_index = (container_value > 0) ? container_value - 1 : 0;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##HandRice", &rice_index, rice_options, IM_ARRAYSIZE(rice_options))) {
							rice_value = rice_index + 1;
						}
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##HandFish", &fish_index, fish_options, IM_ARRAYSIZE(fish_options))) {
							fish_value = fish_index + 1;
						}
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##HandContainerColor", &container_index, container_colors, IM_ARRAYSIZE(container_colors))) {
							container_value = container_index + 1;
						}

						hand_state.Value_8 = (rice_value * 100) + (fish_value * 10) + container_value;
						local_mec->set_hand_state(hand_state);
					} // Pizzushi
					else if (hand_item_name == "CASSETTE") {
						const char* cassette_titles[] = {
							"KHARMA",
							"Who Am I",
							"Burning Wish",
							"Cake",
							"Puzzle",
							"Sun",
							"Worship",
							"Royalty (Instrumental)",
							"Grave",
						}; // new list credit to sinusbot

						int current_value = hand_state.Value_8;
						int selected_index = current_value;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##HandCassette", &selected_index, cassette_titles, IM_ARRAYSIZE(cassette_titles))) {
							hand_state.Value_8 = selected_index;
							local_mec->set_hand_state(hand_state);
						}
					}
					else if (hand_item_name == "VENT FILTER") {
						int clean_percentage = hand_state.Value_8;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::SliderInt("##HandClean%", &clean_percentage, 0, 100, "Clean: %d%")) {
							hand_state.Value_8 = clean_percentage;
							local_mec->set_hand_state(hand_state);
						}
					}
					else if (hand_item_name == "BATTERY") {
						int charge_percentage = hand_state.Value_8;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::SliderInt("##HandCharge%", &charge_percentage, 0, 100, "Charge: %d%")) {
							hand_state.Value_8 = charge_percentage;
							local_mec->set_hand_state(hand_state);
						}
					}
					else if (hand_item_name == "REVOLVER" || hand_item_name == "PISTOL" || hand_item_name == "SHORTY" || hand_item_name == "SMG" || hand_item_name == "RIFLE" || hand_item_name == "SHOTGUN") {
						int ammo = hand_state.Value_8;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::SliderInt("##HandAmmo", &ammo, 0, 100, "Ammo: %d%")) {
							hand_state.Value_8 = ammo;
							local_mec->set_hand_state(hand_state);
						}
					}
				}
				else {
					ImGui::Text("Hand Item: AIR");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					if (ImGui::BeginCombo("##ChangeHandItem", selected_item_name.empty() ? "Select Item" : selected_item_name.c_str())) {
						if (ImGui::Selectable("EMPTY", selected_item_name == "EMPTY")) {
							selected_item_name = "EMPTY";
							local_mec->set_hand_item(nullptr); // Clear the hand item
						}

						PopulateUniqueItems();

						// List of items that require AssignToItemData
						std::unordered_set<std::string> special_items = { "SHORTY", "PISTOL", "REVOLVER", "SMG", "RIFLE", "SHOTGUN", "DETONATOR", "C4" };

						// Ensure all special items are added to the list if missing
						for (const auto& special_item : special_items) {
							if (std::find(item_names.begin(), item_names.end(), special_item) == item_names.end()) {
								item_names.push_back(special_item); // Add missing special items
							}
						}

						// Now list all the items
						for (const auto& item_name : item_names) {
							if (ImGui::Selectable(item_name.c_str(), item_name == selected_item_name)) {
								selected_item_name = item_name;

								// Check if the item is one of the special ones
								if (special_items.find(item_name) != special_items.end()) {
									// Use AssignToItemData for these items
									auto item_data = AssignToItemData(item_name);
									if (item_data) {
										local_mec->set_hand_item(item_data);
									}
									else {
										std::cout << "Failed to assign item data for: " << item_name << std::endl;
									}
								}
								else {
									// Use unique_item_data for regular items
									auto item_data = unique_item_data[item_name];
									if (item_data) {
										local_mec->set_hand_item(item_data);
									}
								}

								// Reset combo box selection after changing the item
								selected_item_name = "";
							}
						}

						ImGui::EndCombo();
					}
				}
				ImGui::Separator();
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
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 70);
					if (ImGui::BeginCombo("##ChangeBagItem", selected_item_name.empty() ? "Select Item" : selected_item_name.c_str())) {
						if (ImGui::Selectable("EMPTY", selected_item_name == "EMPTY")) {
							selected_item_name = "EMPTY";
							local_mec->set_bag_item(nullptr); // Clear the bag item
						}

						PopulateUniqueItems();

						// List of items that require AssignToItemData
						std::unordered_set<std::string> special_items = { "SHORTY", "PISTOL", "REVOLVER", "SMG", "RIFLE", "SHOTGUN", "DETONATOR", "C4" };

						// Ensure all special items are added to the list if missing
						for (const auto& special_item : special_items) {
							if (std::find(item_names.begin(), item_names.end(), special_item) == item_names.end()) {
								item_names.push_back(special_item); // Add missing special items
							}
						}

						// Now list all the items
						for (const auto& item_name : item_names) {
							if (ImGui::Selectable(item_name.c_str(), item_name == selected_item_name)) {
								selected_item_name = item_name;

								// Check if the item is one of the special ones
								if (special_items.find(item_name) != special_items.end()) {
									// Use AssignToItemData for these items
									auto item_data = AssignToItemData(item_name);
									if (item_data) {
										local_mec->set_bag_item(item_data);
									}
									else {
										std::cout << "Failed to assign item data for: " << item_name << std::endl;
									}
								}
								else {
									// Use unique_item_data for regular items
									auto item_data = unique_item_data[item_name];
									if (item_data) {
										local_mec->set_bag_item(item_data);
									}
								}

								// Reset combo box selection after changing the item
								selected_item_name = "";
							}
						}

						ImGui::EndCombo();
					}
					// Show the checkbox for locking
					ImGui::SameLine();
					if (ImGui::Checkbox("Lock", &lock_bag_item)) {
						if (lock_bag_item) {
							// Save the current bag item and state
							locked_bag_item = bag_item;
							locked_bag_state = local_mec->get_bag_state();
						}
						else {
							// Clear the locked data when unlocking
							locked_bag_item = nullptr;
						}
					}
					auto bag_state = local_mec->get_bag_state();
					if (bag_item_name == "GAZ BOTTLE") {
						const char* colors[] = { "Yellow", "Red", "Blue" };
						int current_value = bag_state.Value_8;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##Color", &current_value, colors, IM_ARRAYSIZE(colors))) {
							bag_state.Value_8 = current_value;
							local_mec->set_bag_state(bag_state);
						}
					}
					else if (bag_item_name == "PACKAGE") {
						const char* package_types[] = { "Security", "Computers", "Botanic", "Restaurant", "Medical", "Tutorial" , "Machine" };
						int package_value = bag_state.Value_8 - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##PackageTypes", &package_value, package_types, IM_ARRAYSIZE(package_types))) {
							bag_state.Value_8 = package_value + 1;
							local_mec->set_bag_state(bag_state);
						}
					}
					else if (bag_item_name == "FISH") {
						const char* fish_types[] = { "Salmon", "Tuna", "Cod", "Shrimp" };
						int fish_value = bag_state.Value_8 - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##FishType", &fish_value, fish_types, IM_ARRAYSIZE(fish_types))) {
							bag_state.Value_8 = fish_value + 1;
							local_mec->set_bag_state(bag_state);
						}
					}
					else if (bag_item_name == "RICE") {
						const char* rice_types[] = { "White", "Brown", "Black" };
						int rice_value = bag_state.Value_8 - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##RiceType", &rice_value, rice_types, IM_ARRAYSIZE(rice_types))) {
							bag_state.Value_8 = rice_value + 1;
							local_mec->set_bag_state(bag_state);
						}
					}
					else if (bag_item_name == "CONTAINER") {
						const char* colors[] = { "Empty", "Green", "Yellow", "Blue", "White", "Red", "White Rice", "Brown Rice", "Black Rice", "Dirty" };
						int current_value = (bag_state.Value_8 == -1) ? 9 : bag_state.Value_8; // Map -1 (Dirty) to the new index
						int selected_index = current_value;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##Color", &selected_index, colors, IM_ARRAYSIZE(colors))) {
							if (selected_index == 9) {
								// Handle "Dirty" selection
								bag_state.Value_8 = -1;
							}
							else if (selected_index >= 6) {
								bag_state.Value_8 = 6;
								switch (selected_index) {
								case 6: bag_state.Time_15 = 1; break;
								case 7: bag_state.Time_15 = 2; break;
								case 8: bag_state.Time_15 = 3; break;
								}
							}
							else {
								bag_state.Value_8 = selected_index;

								if (selected_index == 5) {
									bag_state.Time_15 = 1; // Set Time_15 to 1 for Red
								}
								else {
									bag_state.Time_15 = 0;
								}
							}
							local_mec->set_bag_state(bag_state);
						}
					}
					else if (bag_item_name == "SAMPLE") {
						const char* colors[] = { "Empty", "Green", "Yellow", "Blue", "White", "Red", "Green Mix", "Yellow Mix", "Blue Mix", "White Mix" };
						int current_index = (bag_state.Value_8 == 0) ? 0 :
							(bag_state.Time_15 == 1 ? bag_state.Value_8 + 5 : bag_state.Value_8);

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##SampleColor", &current_index, colors, IM_ARRAYSIZE(colors))) {
							if (current_index == 0) { // Empty
								bag_state.Value_8 = 0;
								bag_state.Time_15 = 0;
							}
							else if (current_index == 5) { // Red
								bag_state.Value_8 = 5;
								bag_state.Time_15 = 1;
							}
							else if (current_index >= 6) { // Mixes
								bag_state.Value_8 = current_index - 5; // Map mix index back to unmixed Value_8
								bag_state.Time_15 = 1; // Mixes have Time_15 of 1
							}
							else { // Standard colors
								bag_state.Value_8 = current_index;
								bag_state.Time_15 = 0; // Standard colors have Time_15 of 0
							}
							local_mec->set_bag_state(bag_state);
						}
					}
					else if (bag_item_name == "FUSE") {
						const char* fuse_colors[] = { "Red", "Yellow", "Blue" };
						int time_color = bag_state.Time_15 - 1;
						int value_color = bag_state.Value_8 - 1;

						float half_width = (ImGui::GetContentRegionAvail().x) / 2;

						ImGui::SetNextItemWidth(half_width);
						if (ImGui::Combo("##ValueColor", &value_color, fuse_colors, IM_ARRAYSIZE(fuse_colors))) {
							bag_state.Value_8 = value_color + 1;
							local_mec->set_bag_state(bag_state);
						}

						ImGui::SameLine();
						ImGui::SetNextItemWidth(half_width);
						if (ImGui::Combo("##TimeColor", &time_color, fuse_colors, IM_ARRAYSIZE(fuse_colors))) {
							bag_state.Time_15 = time_color + 1;
							local_mec->set_bag_state(bag_state);
						}
					}
					else if (bag_item_name == "NAME") { // Pizzushi
						const char* rice_options[] = { "White Rice", "Brown Rice", "Black Rice" };
						const char* fish_options[] = { "Salmon", "Tuna", "Cod", "Shrimp" };
						const char* container_colors[] = { "Green", "Yellow", "Blue", "White", "Red" };

						// Default value for Value_8 (1st option for rice, fish, and container)
						if (bag_state.Value_8 <= 0) {
							bag_state.Value_8 = 111; // Default to "White Rice", "Salmon", "Green"
							local_mec->set_bag_state(bag_state); // Ensure it's set in-game
						}

						int value = bag_state.Value_8;
						int rice_value = value / 100;
						int fish_value = (value / 10) % 10;
						int container_value = value % 10;

						// Initialize indices with default values (0 for the first option)
						int rice_index = (rice_value > 0) ? rice_value - 1 : 0;
						int fish_index = (fish_value > 0) ? fish_value - 1 : 0;
						int container_index = (container_value > 0) ? container_value - 1 : 0;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##Rice", &rice_index, rice_options, IM_ARRAYSIZE(rice_options))) {
							rice_value = rice_index + 1;
						}
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##Fish", &fish_index, fish_options, IM_ARRAYSIZE(fish_options))) {
							fish_value = fish_index + 1;
						}
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##Container Color", &container_index, container_colors, IM_ARRAYSIZE(container_colors))) {
							container_value = container_index + 1;
						}

						bag_state.Value_8 = (rice_value * 100) + (fish_value * 10) + container_value;
						local_mec->set_bag_state(bag_state);
					}
					else if (bag_item_name == "CASSETTE") {
						const char* cassette_titles[] = {
									"KHARMA",
									"Who Am I",
									"Burning Wish",
									"Cake",
									"Puzzle",
									"Sun",
									"Worship",
									"Royalty (Instrumental)",
									"Grave",
						}; // new list credit to sinusbot

						int current_value = bag_state.Value_8;
						int selected_index = current_value;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##Cassette", &selected_index, cassette_titles, IM_ARRAYSIZE(cassette_titles))) {
							bag_state.Value_8 = selected_index;
							local_mec->set_bag_state(bag_state);
						}
					}
					else if (bag_item_name == "VENT FILTER") {
						int clean_percentage = bag_state.Value_8;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::SliderInt("##%", &clean_percentage, 0, 100, "Clean: %d%")) {
							bag_state.Value_8 = clean_percentage;
							local_mec->set_bag_state(bag_state);
						}
						if (!ImGui::IsItemDeactivatedAfterEdit());
					}
					else if (bag_item_name == "BATTERY") {
						int charge_percentage = bag_state.Value_8;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::SliderInt("##%", &charge_percentage, 0, 100, "Charge: %d%")) {
							bag_state.Value_8 = charge_percentage;
							local_mec->set_bag_state(bag_state);
						}
					}
					else if (bag_item_name == "REVOLVER" || bag_item_name == "PISTOL" || bag_item_name == "SHORTY" || bag_item_name == "SMG" || bag_item_name == "RIFLE" || bag_item_name == "SHOTGUN") {
						int ammo = bag_state.Value_8;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::SliderInt("##BagAmmo", &ammo, 0, 100, "Ammo: %d%")) {
							bag_state.Value_8 = ammo;
							local_mec->set_bag_state(bag_state);
						}
					}
				}
				else {
					ImGui::Text("Bag Item: EMPTY");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					if (ImGui::BeginCombo("##ChangeBagItem", selected_item_name.empty() ? "Select Item" : selected_item_name.c_str())) {
						if (ImGui::Selectable("EMPTY", selected_item_name == "EMPTY")) {
							selected_item_name = "EMPTY";
							local_mec->set_bag_item(nullptr); // Clear the bag item
						}

						PopulateUniqueItems();

						// List of items that require AssignToItemData
						std::unordered_set<std::string> special_items = { "SHORTY", "PISTOL", "REVOLVER", "SMG", "RIFLE", "SHOTGUN", "DETONATOR", "C4" };

						// Ensure all special items are added to the list if missing
						for (const auto& special_item : special_items) {
							if (std::find(item_names.begin(), item_names.end(), special_item) == item_names.end()) {
								item_names.push_back(special_item); // Add missing special items
							}
						}

						// Now list all the items
						for (const auto& item_name : item_names) {
							if (ImGui::Selectable(item_name.c_str(), item_name == selected_item_name)) {
								selected_item_name = item_name;

								// Check if the item is one of the special ones
								if (special_items.find(item_name) != special_items.end()) {
									// Use AssignToItemData for these items
									auto item_data = AssignToItemData(item_name);
									if (item_data) {
										local_mec->set_bag_item(item_data);
									}
									else {
										std::cout << "Failed to assign item data for: " << item_name << std::endl;
									}
								}
								else {
									// Use unique_item_data for regular items
									auto item_data = unique_item_data[item_name];
									if (item_data) {
										local_mec->set_bag_item(item_data);
									}
								}

								// Reset combo box selection after changing the item
								selected_item_name = "";
							}
						}

						ImGui::EndCombo();
					}
				}
			}

			if (hand_item) {
				if (!hand_inventory) {
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					if (ImGui::Button("Can Inventory", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
						if (hand_item) {
							hand_item->set_can_inventory(!hand_inventory);
						}
					}
				}
				else {
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					if (ImGui::Button("Can't Inventory", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
						if (hand_item) {
							hand_item->set_can_inventory(!hand_inventory);
						}
					}
				}
			}

			if (ImGui::CollapsingHeader("ITEM THROW DATA")) {
				auto itemThrow = hand_item->get_throw_type();
				auto itemThrowForce = itemThrow->get_throw_force();
				auto itemVerticalForce = itemThrow->get_vertical_force();
				auto itemVerticalOffset = itemThrow->get_vertical_offset();
				auto itemRestitution = itemThrow->get_restitution();
				auto itemGravity = itemThrow->get_gravity();
				auto itemDrag = itemThrow->get_drag();
				auto itemRadius = itemThrow->get_radius();

				// First column (left side)
				ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
				if (ImGui::SliderInt("##ThrowForce", &itemThrowForce, 0, 10000, "Throw Force: %d%")) {
					itemThrow->set_throw_force(itemThrowForce);
				}
				ImGui::SameLine();
				int intRestitution = static_cast<int>(itemRestitution * 100); // Scale double to int representation
				if (ImGui::SliderInt("##Restitution", &intRestitution, 0, 1000, "Restitution: %d")) {
					itemRestitution = intRestitution / 100.0; // Convert back to double
					itemThrow->set_restitution(itemRestitution);
				}

				if (ImGui::SliderInt("##VerticalForce", &itemVerticalForce, 0, 10000, "Vertical Force: %d%")) {
					itemThrow->set_vertical_force(itemVerticalForce);
				}
				ImGui::SameLine();
				int intGravity = static_cast<int>(itemGravity * 100); // Scale double to int representation
				if (ImGui::SliderInt("##Gravity", &intGravity, -100000, 100000, "Gravity: %d")) {
					itemGravity = intGravity / 100.0; // Convert back to double
					itemThrow->set_gravity(itemGravity);
				}

				if (ImGui::SliderInt("##VerticalOffset", &itemVerticalOffset, -100, 100, "Vertical Offset: %d%")) {
					itemThrow->set_vertical_offset(itemVerticalOffset);
				}
				ImGui::SameLine();
				int intDrag = static_cast<int>(itemDrag * 100); // Scale double to int representation
				if (ImGui::SliderInt("##Drag", &intDrag, 0, 1000, "Drag: %d")) {
					itemDrag = intDrag / 100.0; // Convert back to double
					itemThrow->set_drag(itemDrag);
				}

				int intRadius = static_cast<int>(itemRadius * 100); // Scale double to int representation
				if (ImGui::SliderInt("##Radius", &intRadius, -1000, 1000, "Radius: %d")) {
					itemRadius = intRadius / 100.0; // Convert back to double
					itemThrow->set_radius(itemRadius);
				}
				// Restore original width
				ImGui::PopItemWidth();
			} ImGui::SameLine(); MenuTooltip("Only works in self hosted lobbies.");

			calculatedHeight += itemHeight * 16.75;
		}

		ImGui::EndChild();

		//ImGui::SetWindowSize(ImVec2(-1, calculatedHeight));
		if (calculatedHeight <= 200.0f) {
			ImGui::SetWindowSize(ImVec2(500.f, 200.0f));
		}
		else {
			ImGui::SetWindowSize(ImVec2(500.f, calculatedHeight));
		}

		ImGui::PopStyleColor(3);

		ImGui::End();
	}

	if (player_list) {
		ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.1059f, 0.3765f, 0.6510f, 1.0f));

		// Temporary vectors to separate employees and dissidents
		std::vector<std::pair<std::string, ImVec4>> employees;
		std::vector<std::pair<std::string, ImVec4>> dissidents;

		// Variables to calculate dynamic size
		float max_text_width = 0.0f;
		int total_entries = 0;

		for (auto mec : player_cache) {
			if (mec != local_mec) {
				auto state = mec->player_state();
				if (!state) continue;

				auto name = state->get_player_name_private().read_string();
				auto role = mec->get_player_role();

				auto mec_root = mec->get_root_component();
				if (!mec_root) continue;

				auto position = mec_root->get_relative_location();
				auto distance = MenuCalculateDistance(local_mec->get_net_location(), position);

				auto ghost_root = mec->get_ghost_root();
				bool is_ghost = false;
				double ghost_distance = 0.0;

				if (ghost_root) {
					auto ghostPosition = ghost_root->get_relative_location();
					ghost_distance = MenuCalculateDistanceMeters(position, ghostPosition);

					if (ghost_distance > 2) {
						is_ghost = true; // User is a ghost
					}
				}

				// Determine the display name with [D] for dissident and [G] for ghost
				std::string display_name =
					(role == 4 ? std::string("[D]") : std::string("")) +
					(is_ghost ? std::string("[G]") : std::string("")) +
					name +
					(player_distance ? " [" + distance + "m]" : "");

				// Fetch the player's color from the configuration
				ImVec4 color = (role == 4) ? dissident_color : employee_color;

				// Add to the respective list
				if (role == 4) {
					dissidents.emplace_back(display_name, color);
				}
				else {
					employees.emplace_back(display_name, color);
				}

				// Calculate text width for dynamic sizing
				float text_width = ImGui::CalcTextSize(display_name.c_str()).x;
				max_text_width = (std::max)(max_text_width, text_width);
				total_entries++;
			}
		}

		// Add widths for headers and location info
		float header_width = ImGui::CalcTextSize("Dissidents").x;
		max_text_width = (std::max)(max_text_width, header_width);
		max_text_width = (std::max)(max_text_width, ImGui::CalcTextSize("Location: X: 9999.9 Y: 9999.9").x);

		// Calculate the total height
		int header_entries = 6.75; // For "Dissidents" and "Employees" headers
		int total_rows = total_entries + header_entries;
		float row_height = ImGui::GetTextLineHeightWithSpacing();
		float window_height = total_rows * row_height + 20.0f; // Add some extra padding

		// Apply padding for width and height
		float padding = 20.0f; // Padding around content
		float window_width = max_text_width + padding;

		// Set dynamic size for the window
		ImGui::SetNextWindowSize(ImVec2(window_width, window_height), ImGuiCond_Always);

		// Define window flags
		ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;
		if (player_list_locked) {
			flags |= ImGuiWindowFlags_NoMove;
		}

		// Lock window position if the list is locked, otherwise allow moving
		if (player_list_locked) {
			ImGui::SetNextWindowPos(ImVec2(player_list_x, player_list_y), ImGuiCond_Always);
		}
		else {
			ImGui::SetNextWindowPos(ImVec2(player_list_x, player_list_y), ImGuiCond_FirstUseEver);
		}

		ImGui::Begin("Players", &player_list, ImGuiWindowFlags_AlwaysAutoResize);

		ImVec2 menu_position = ImGui::GetWindowPos(); // Get the current menu position (X, Y)

		ImGui::Text("Location:");
		ImGui::Separator();
		ImGui::Text("X: %.1f", menu_position.x); // Display the X coordinate
		ImGui::SameLine();
		ImGui::Text("Y: %.1f", menu_position.y); // Display the Y coordinate

		if (ImGui::Checkbox("Lock List##LockList", &player_list_locked)) {
			player_list_x = menu_position.x;
			player_list_y = menu_position.y;
		}

		// Render Dissidents
		if (ImGui::CollapsingHeader("Dissidents", ImGuiTreeNodeFlags_DefaultOpen)) {
			for (const auto& [display_name, color] : dissidents) {
				ImGui::TextColored(color, "%s", display_name.c_str());
			}
		}

		// Render Employees
		if (ImGui::CollapsingHeader("Employees", ImGuiTreeNodeFlags_DefaultOpen)) {
			for (const auto& [display_name, color] : employees) {
				ImGui::TextColored(color, "%s", display_name.c_str());
			}
		}

		ImGui::PopStyleColor();
		ImGui::End();
	}
}
