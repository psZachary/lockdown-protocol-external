#include "radar.h"
#include "globals.h"
#include "config.h"
#include "game_math.hpp"
#include "overlay/imgui/imgui.h"
#include <vector>
#include <cmath>

#define M_PI 3.1415926535

using namespace globals;
using namespace config;

// Helper function for converting vector2 to ImVec2
inline ImVec2 toImVec2(const vector2& vec) {
	return ImVec2(static_cast<float>(vec.x), static_cast<float>(vec.y));
}

// Helper function to compute triangle points
std::vector<ImVec2> compute_triangle(const vector2& center, double size, double angle) {
	std::vector<ImVec2> points;

	// Define the vertices relative to the center
	vector2 front = center + vector2(size * cos(angle), size * sin(angle));       // Front vertex
	vector2 left = center + vector2(size * cos(angle + M_PI * 0.75), size * sin(angle + M_PI * 0.75));  // Left vertex
	vector2 right = center + vector2(size * cos(angle - M_PI * 0.75), size * sin(angle - M_PI * 0.75)); // Right vertex

	points.push_back(toImVec2(front));
	points.push_back(toImVec2(left));
	points.push_back(toImVec2(right));

	return points;
}

void radar::draw() {
	if (esp_radar) {
		// Define scale
		const double scaleFactor = 0.05; // Scale to convert world units to radar units

		// Calculate radar position based on the config setting
		ImVec2 screenSize = ImGui::GetIO().DisplaySize;

		// Calculate radar size using esp_radar_scale
		double radarSize = screenSize.x * esp_radar_scale; // Scale dynamically based on setting

		// Adjust position offsets relative to screen size
		double padding = screenSize.x * 0.02; // 2% of screen width for padding

		// Calculate radar center based on the user-defined position
		vector2 radarCenter;
		if (esp_radar_position == "Top Left") {
			radarCenter = { radarSize / 2 + padding, radarSize / 2 + padding };
		}
		else if (esp_radar_position == "Top Middle") {
			radarCenter = { screenSize.x / 2.0, radarSize / 2 + padding };
		}
		else if (esp_radar_position == "Top Right") {
			radarCenter = { screenSize.x - radarSize / 2 - padding, radarSize / 2 + padding };
		}
		else if (esp_radar_position == "Middle Left") {
			radarCenter = { radarSize / 2 + padding, screenSize.y / 2.0 };
		}
		else if (esp_radar_position == "Middle Right") {
			radarCenter = { screenSize.x - radarSize / 2 - padding, screenSize.y / 2.0 };
		}
		else if (esp_radar_position == "Bottom Left") {
			radarCenter = { radarSize / 2 + padding, screenSize.y - radarSize / 2 - padding };
		}
		else if (esp_radar_position == "Bottom Middle") {
			radarCenter = { screenSize.x / 2.0, screenSize.y - radarSize / 2 - padding };
		}
		else if (esp_radar_position == "Bottom Right") {
			radarCenter = { screenSize.x - radarSize / 2 - padding, screenSize.y - radarSize / 2 - padding };
		}

		// Get camera's rotation (from f_minimal_view_info)
		f_camera_cache last_frame_cached = local_camera_manager->get_cached_frame_private();
		f_minimal_view_info camera_cache = last_frame_cached.pov;
		vector3 camera_rotation = camera_cache.rotation; // Get camera rotation (yaw, pitch, roll)

		// Normalize yaw (roll)
		double camera_yaw = camera_rotation.y; // Roll represents yaw
		if (camera_yaw < 0) {
			camera_yaw += 360.0; // Normalize to [0, 360)
		}
		camera_yaw = camera_yaw * (M_PI / 180.0); // Convert degrees to radians

		// Get local player's position and rotation
		auto local_root = local_mec->get_root_component();
		vector3 local_pos = local_root->get_relative_location();
		vector3 local_rot = local_root->get_relative_rotation();

		// Determine the facing angle dynamically (in radians)
		double player_facing_angle = local_rot.z * (M_PI / 180.0); // Convert degrees to radians

		// Draw radar background
		ImGui::GetForegroundDrawList()->AddCircleFilled(
			toImVec2(radarCenter), radarSize / 2, IM_COL32(0, 0, 0, 150)); // Semi-transparent black
		ImGui::GetForegroundDrawList()->AddCircle(
			toImVec2(radarCenter), radarSize / 2, IM_COL32(255, 255, 255, 255), 64, 1.5f); // White outline

		// Calculate triangle vertices for the local player
		double triangleSize = 8.0; // Triangle size for the local player
		vector2 top = radarCenter + vector2(triangleSize * cos(camera_yaw), triangleSize * sin(camera_yaw));
		vector2 left = radarCenter + vector2(triangleSize * cos(camera_yaw + 2.0 * M_PI / 3.0), triangleSize * sin(camera_yaw + 2.0 * M_PI / 3.0));
		vector2 right = radarCenter + vector2(triangleSize * cos(camera_yaw - 2.0 * M_PI / 3.0), triangleSize * sin(camera_yaw - 2.0 * M_PI / 3.0));

		// Player dot
		float playerDotSize = 5.0f; // Size of the player's dot
		ImGui::GetForegroundDrawList()->AddCircleFilled(
			toImVec2(radarCenter), playerDotSize, IM_COL32(255, 255, 0, 255)); // Bright yellow dot

		// Cone to represent the player's POV
		double fovAngle = 90.0 * (M_PI / 180.0); // FOV angle (90 degrees converted to radians)

		// Calculate curved edge of the FOV cone
		double coneRadius = radarSize / 2.0; // Radius of the radar
		const int numSegments = 32; // Number of segments for smooth curvature
		std::vector<ImVec2> coneVertices;

		// Add the cone tip (player position)
		coneVertices.push_back(toImVec2(radarCenter));

		// Generate points along the curved edge
		for (int i = 0; i <= numSegments; ++i) {
			double angle = camera_yaw - (fovAngle / 2.0) + (fovAngle * i / numSegments);
			vector2 edgePoint = radarCenter + vector2(
				coneRadius * cos(angle),
				coneRadius * sin(angle)
			);
			coneVertices.push_back(toImVec2(edgePoint));
		}

		// Draw the FOV cone as a filled polygon
		ImGui::GetForegroundDrawList()->AddConvexPolyFilled(
			coneVertices.data(), static_cast<int>(coneVertices.size()), IM_COL32(255, 255, 255, 64)); // White with 25% opacity

		// Convert colors from config.h to ImU32
		ImU32 employeeColor = ImGui::ColorConvertFloat4ToU32(config::employee_color);
		ImU32 ghostEmployeeColor = ImGui::ColorConvertFloat4ToU32(config::ghost_employee_color);
		ImU32 dissidentColor = ImGui::ColorConvertFloat4ToU32(config::dissident_color);
		ImU32 ghostDissidentColor = ImGui::ColorConvertFloat4ToU32(config::ghost_dissident_color);
		ImU32 weaponColor = ImGui::ColorConvertFloat4ToU32(config::weapon_color);
		ImU32 gazColor = ImGui::ColorConvertFloat4ToU32(config::gaz_bottle_color);
		ImU32 ventColor = ImGui::ColorConvertFloat4ToU32(config::vent_filter_color);
		ImU32 riceColor = ImGui::ColorConvertFloat4ToU32(config::rice_color);
		ImU32 packageColor = ImGui::ColorConvertFloat4ToU32(config::package_color);
		ImU32 sampleColor = ImGui::ColorConvertFloat4ToU32(config::sample_color);
		ImU32 containerColor = ImGui::ColorConvertFloat4ToU32(config::container_color);
		ImU32 screwColor = ImGui::ColorConvertFloat4ToU32(config::screw_driver_color);
		ImU32 batteryColor = ImGui::ColorConvertFloat4ToU32(config::battery_color);
		ImU32 fuseColor = ImGui::ColorConvertFloat4ToU32(config::fuse_color);

		// Helper function for rotation
		auto rotate_point = [](const vector2& point, double angle) {
			double cos_a = cos(angle);
			double sin_a = sin(angle);
			return vector2{
				point.x * cos_a - point.y * sin_a,
				point.x * sin_a + point.y * cos_a
			};
			};

		// Plot players
		if (player_radar) {
			for (auto mec : player_cache) {

				if (mec != local_mec) {
					auto role = mec->get_player_role(); // Get player role
					ImU32 playercolor = (role == 4) ? dissidentColor : employeeColor; // Use config colors

					auto root_component = mec->get_root_component();
					if (!root_component) continue;

					auto relative_location = root_component->get_relative_location();
					vector3 position = relative_location;
					vector3 relativePos = position - local_pos; // Calculate relative position
					vector2 radarPos = { relativePos.x * scaleFactor, relativePos.y * scaleFactor }; // Map to radar space

					// Rotate radar points based on player facing angle
					radarPos = rotate_point(radarPos, player_facing_angle);

					// Only draw players within radar bounds
					if (radarPos.magnitude() < radarSize / 2) {
						vector2 pointPos = radarCenter + radarPos; // Map to radar space
						ImGui::GetForegroundDrawList()->AddCircleFilled(
							toImVec2(pointPos), 3.0f, playercolor); // Use role color
					}
				}
			}
		}

		// Plot ghosts
		if (ghost_radar) {
			for (auto mec : player_cache) {
				if (mec != local_mec) {
					auto role = mec->get_player_role(); // Get player role
					ImU32 ghostcolor = (role == 4) ? ghostDissidentColor : ghostEmployeeColor; // Use config colors

					auto ghost_root = mec->get_ghost_root();
					if (!ghost_root) continue;

					auto relative_location = ghost_root->get_relative_location();
					vector3 position = relative_location;
					vector3 relativePos = position - local_pos; // Calculate relative position
					vector2 radarPos = { relativePos.x * scaleFactor, relativePos.y * scaleFactor }; // Map to radar space

					// Rotate radar points based on player facing angle
					radarPos = rotate_point(radarPos, player_facing_angle);

					// Only draw players within radar bounds
					if (radarPos.magnitude() < radarSize / 2) {
						vector2 pointPos = radarCenter + radarPos; // Map to radar space
						ImGui::GetForegroundDrawList()->AddCircleFilled(
							toImVec2(pointPos), 3.0f, ghostcolor); // Use role color
					}
				}
			}
		}


		// Plot weapons
		if (weapon_radar) {
			for (auto item : world_item_cache) {
				if (!item) continue;

				auto data = item->get_data();
				auto item_name = data->get_name().read_string();

				// Check if the item is a weapon
				if (item_name == "SHORTY" || item_name == "PISTOL" || item_name == "REVOLVER" ||
					item_name == "C4" || item_name == "DETONATOR" || item_name == "KNIFE") {
					auto root_component = item->get_root_component();
					if (!root_component) continue;

					auto relative_location = root_component->get_relative_location();
					vector3 position = relative_location;
					vector3 relativePos = position - local_pos; // Calculate relative position
					vector2 radarPos = { relativePos.x * scaleFactor, relativePos.y * scaleFactor }; // Map to radar space

					// Rotate radar points based on player facing angle
					radarPos = rotate_point(radarPos, player_facing_angle);

					// Only draw items within radar bounds
					if (radarPos.magnitude() < radarSize / 2) {
						vector2 pointPos = radarCenter + radarPos; // Map to radar space
						ImGui::GetForegroundDrawList()->AddCircleFilled(
							toImVec2(pointPos), 3.0f, weaponColor); // Use weaponColor for weapons
					}
				}
			}
		}

		// Plot Primary Objects
		if (primary_radar) {
			for (auto item : world_item_cache) {
				if (!item) continue;

				auto data = item->get_data();
				auto item_name = data->get_name().read_string();

				// Check if the item is a weapon
				if (item_name == "GAZ BOTTLE" || item_name == "VENT FILTER" || item_name == "RICE" ||
					item_name == "PACKAGE" || item_name == "SAMPLE") {
					auto root_component = item->get_root_component();
					if (!root_component) continue;

					auto relative_location = root_component->get_relative_location();
					vector3 position = relative_location;
					vector3 relativePos = position - local_pos; // Calculate relative position
					vector2 radarPos = { relativePos.x * scaleFactor, relativePos.y * scaleFactor }; // Map to radar space

					// Rotate radar points based on player facing angle
					radarPos = rotate_point(radarPos, player_facing_angle);

					ImU32 dotColor = IM_COL32(255, 255, 255, 255);
					if (item_name == "GAZ BOTTLE") {
						dotColor = gazColor;
					}
					else if (item_name == "VENT FILTER") {
						dotColor = ventColor;
					}
					else if (item_name == "RICE") {
						dotColor = riceColor;
					}
					else if (item_name == "PACKAGE") {
						dotColor = packageColor;
					}
					else if (item_name == "SAMPLE") {
						dotColor = sampleColor;
					}

					// Only draw items within radar bounds
					if (radarPos.magnitude() < radarSize / 2) {
						vector2 pointPos = radarCenter + radarPos; // Map to radar space
						ImGui::GetForegroundDrawList()->AddCircleFilled(
							toImVec2(pointPos), 3.0f, dotColor);
					}
				}
			}
		}


		// Plot Secondary Objects
		if (secondary_radar) {
			for (auto item : world_item_cache) {
				if (!item) continue;

				auto data = item->get_data();
				auto item_name = data->get_name().read_string();

				// Check if the item is a weapon
				if (item_name == "BATTERY" || item_name == "FUSE" || item_name == "CONTAINER" ||
					item_name == "SCREW DRIVER") {
					auto root_component = item->get_root_component();
					if (!root_component) continue;

					auto relative_location = root_component->get_relative_location();
					vector3 position = relative_location;
					vector3 relativePos = position - local_pos; // Calculate relative position
					vector2 radarPos = { relativePos.x * scaleFactor, relativePos.y * scaleFactor }; // Map to radar space

					// Rotate radar points based on player facing angle
					radarPos = rotate_point(radarPos, player_facing_angle);

					ImU32 dotColor = IM_COL32(255, 255, 255, 255);
					if (item_name == "BATTERY") {
						dotColor = batteryColor;
					}
					else if (item_name == "FUSE") {
						dotColor = fuseColor;
					}
					else if (item_name == "CONTAINER") {
						dotColor = containerColor;
					}
					else if (item_name == "SCREW DRIVER") {
						dotColor = screwColor;
					}

					// Only draw items within radar bounds
					if (radarPos.magnitude() < radarSize / 2) {
						vector2 pointPos = radarCenter + radarPos; // Map to radar space
						ImGui::GetForegroundDrawList()->AddCircleFilled(
							toImVec2(pointPos), 3.0f, dotColor);
					}
				}
			}
		}
	}
}

