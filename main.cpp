#include <iostream>
#include <thread>
#include <string>
#include "overlay/overlay.h"
#include "mem.hpp"
#include "game_structures.hpp"
#include "util.hpp"
#include "config.h"
#include "menu.h"
#include "globals.h"
#include "ItemProperties.h"

using namespace menu;
using namespace config;
using namespace globals;
using namespace protocol::engine::sdk;
using namespace protocol::game::sdk;

std::unordered_map<std::string, ItemProperties> itemData;

void InitializeItems() {
    itemData["KNIFE"] = ItemProperties(0.1, 0.4, 0.4, 130, 20);
    itemData["PACKAGE"] = ItemProperties(0.3, 1, -1, 180, 40);
    itemData["GAZ BOTTLE"] = ItemProperties(0.3, 1, -1, 180, 40);
    itemData["FUSE"] = ItemProperties(0.2, 0.7, -0.8, 130, 20);
    itemData["SCREW DRIVER"] = ItemProperties(0.1, 0.4, 0.1, 130, 15);
    itemData["CASSETTE"] = ItemProperties(0.2, 0.7, -0.8, 130, 20);
    itemData["BATTERY"] = ItemProperties(0.2, 0.7, -0.8, 130, 20);
    itemData["VENT FILTER"] = ItemProperties(0.3, 1, -1, 180, 40);
    itemData["FISH"] = ItemProperties(0.2, 0.7, 1, 130, 20);
    itemData["RICE"] = ItemProperties(0.3, 1, -1, 180, 40);
    itemData["CONTAINER"] = ItemProperties(0.2, 0.7, -0.8, 130, 20);
    itemData["C4"] = ItemProperties(0.2, 0.7, -0.8, 130, 20);
    itemData["NAME"] = ItemProperties(0.3, 1, -1, 180, 40); // Pizzushi
    itemData["SAMPLE"] = ItemProperties(0.2, 0.7, -0.8, 130, 20);
}

ItemProperties GetItemProperties(const std::string& itemName) {
    if (itemData.find(itemName) != itemData.end()) {
        return itemData[itemName];
    }
    else {
        std::cerr << "Item not found: " << itemName << std::endl;
        return ItemProperties(0.1, 0.4, 10.0, 130, 15);  // Default properties if not found
    }
}

static void cache_useful() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        gworld = u_world::get_world(mem::module_base);
        if (!gworld) continue;
        game_state = gworld->get_game_state();
        if (!game_state) continue;
        owning_instance = gworld->get_owning_game_instance();
        if (!owning_instance) continue;
        local_player = owning_instance->get_localplayer();
        if (!local_player) continue;
        local_controller = local_player->get_player_controller();
        if (!local_controller) continue;
        local_camera_manager = local_controller->get_camera_manager();
        if (!local_camera_manager) continue;
        local_mec = (mec_pawn*)local_controller->get_pawn();
        if (!local_mec) continue;

        std::vector < mec_pawn* > temp_player_cache{};
        std::vector < world_item* > temp_world_item_cache{};

        InitializeItems();

        auto levels = gworld->get_levels();
        for (auto level : levels.list()) {
            auto actors = level->get_actors();
            for (auto actor : actors.list()) {
                auto class_name = util::get_name_from_fname(actor->class_private()->fname_index());
                auto name = util::get_name_from_fname(actor->outer()->fname_index());

                if (class_name.find("WorldItem_C") != std::string::npos) {
                    temp_world_item_cache.push_back((world_item*)actor);
                }
                if (class_name.find("Mec_C") != std::string::npos) {
                    temp_player_cache.push_back((mec_pawn*)actor);
                }
            }
        }

        player_cache = temp_player_cache;
        world_item_cache = temp_world_item_cache;
    }
}

static void render_callback() {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    menu::draw();

    f_camera_cache last_frame_cached = local_camera_manager->get_cached_frame_private();
    auto players = game_state->player_array();

    if (GetAsyncKeyState(VK_F2) & 1) {
        player_esp = !player_esp;
    }
    if (GetAsyncKeyState(VK_F3) & 1) {
        weapon_esp = !weapon_esp;
    }
    if (GetAsyncKeyState(VK_F4) & 1) {
        primary_object_esp = !primary_object_esp;
    }
    if (GetAsyncKeyState(VK_F5) & 1) {
        secondary_object_esp = !secondary_object_esp;
    }
    if (GetAsyncKeyState(VK_F6) & 1) {
        speedhack = !speedhack;
    }
    if (GetAsyncKeyState(VK_F7) & 1) {
        god_mode = !god_mode;
    }
    if (GetAsyncKeyState(VK_F8) & 1) {
        infinite_stamina = !infinite_stamina;
    }
    if (GetAsyncKeyState(VK_F9) & 1) {
        living_state = !living_state;
    }

    auto hand_item = local_mec->get_hand_item();
    auto melee_item_data = (u_data_melee*)local_mec->get_hand_item();

    //if (hand_item) {
    //    auto mtype = melee_item_data->get_melee_type();
    //    // Retrieve the properties
    //    double castTime = mtype->get_cast_time();
    //    double recoverTime = mtype->get_recover_time();
    //    double stun = mtype->get_stun();
    //    int cost = mtype->get_cost();
    //    int range = mtype->get_range();

    //    // Format and output the string to the console
    //    std::cout << "itemData[\"" << hand_item->get_name().read_string() << "\"] = "
    //    << "ItemProperties(" << castTime << ", "
    //    << recoverTime << ", "
    //    << stun << ", "
    //    << range << ", "
    //    << cost << ");" << std::endl;
    //}

    if (infinite_ammo) {
        if (hand_item) {
            auto item_name = hand_item->get_name().read_string();
            if (item_name == "SHORTY" || item_name == "PISTOL" || item_name == "REVOLVER" || item_name == "SHOTGUN" || item_name == "RIFLE" || item_name == "SMG") {
                auto hand_state = local_mec->get_hand_state();
                hand_state.Value_8 = 99;
                local_mec->set_hand_state(hand_state);
            }
            else {
                std::cout << "Error: " << item_name << " is not a valid gun." << std::endl;
                infinite_ammo = !infinite_ammo;
            }
        }
    }

    if (living_state) {
        local_mec->set_alive(true);
        local_mec->set_health(100);
        living_state = !living_state;
    }

    if (infinite_stamina) {
        local_mec->set_stamina(1.);
    }

    if (god_mode) {
        local_mec->set_health(100);
    }

    //static double fric = local_mec->get_friction();
    //std::cout << *reinterpret_cast<std::uint64_t*>(&fric);
    if (speedhack) {
        local_mec->set_acceleration(vector2(9999.0, 9999.0));
        local_mec->set_max_speed(2000.0);
        local_mec->set_friction(100000);
    }
    else {
        local_mec->set_acceleration(vector2(100.0, 100.0));
        local_mec->set_max_speed(800.0);
        local_mec->set_friction(0);
    }

    if (fast_melee || infinite_melee_range) {
        if (util::get_name_from_fname(hand_item->class_private()->fname_index()).find("Data_Melee_C") != std::string::npos) {
            auto mtype = melee_item_data->get_melee_type();

            if (fast_melee) {
                mtype->set_cast_time(0.05);
                mtype->set_recover_time(0.05);
                mtype->set_stun(0.0);
                mtype->set_cost(0);
            }
            if (infinite_melee_range) {
                mtype->set_range(10000);
            }
        }
    }

    if (hand_item) {
        if (!fast_melee) {
            auto mtype = melee_item_data->get_melee_type();
            std::string item_name = hand_item->get_name().read_string();
            ItemProperties itemprops = GetItemProperties(item_name);

            if (!fast_melee) {
                mtype->set_cast_time(itemprops.melee_cast_time);
                mtype->set_recover_time(itemprops.melee_recover_time);
                mtype->set_stun(itemprops.melee_stun);
                mtype->set_cost(itemprops.melee_cost);
            }
        }

        if (!infinite_melee_range) {
            auto mtype = melee_item_data->get_melee_type();
            std::string item_name = hand_item->get_name().read_string();
            ItemProperties itemprops = GetItemProperties(item_name);

            if (!infinite_melee_range) {
                mtype->set_range(itemprops.melee_range);
            }
        }
    }

    if (player_esp) {
        for (auto mec : player_cache) {
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
                auto name_norm = "[" + name + "]" + (role == 4 ? " [D]" : "");

                overlay->draw_text(screen_position, color, name_norm.c_str(), true);
            }
        }
    }

    for (auto item : world_item_cache) {
        if (!item) continue;

        auto data = item->get_data();
        auto item_name = data->get_name().read_string();
        auto item_state = item->get_item_state();
        int item_value = item_state.Value_8;
        int item_time = item_state.Time_15;
        auto distance = item->get_distance();

        if (item_name == "PISTOL" || item_name == "REVOLVER" || item_name == "SHORTY" || item_name == "SMG" || item_name == "RIFLE" || item_name == "SHOTGUN") {
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

            if (weapon_esp) {
                auto position = item_root->get_relative_location();
                vector3 screen_position{};
                if (util::w2s(position, last_frame_cached.pov, screen_position)) {
                    if (item_name == "PISTOL") {
                        overlay->draw_text(screen_position, IM_COL32(250, 92, 0, 255), "[PISTOL]", true); // Orange
                    }
                    else if (item_name == "REVOLVER") {
                        overlay->draw_text(screen_position, IM_COL32(250, 92, 0, 255), "[REVOLVER]", true);
                    }
                    else if (item_name == "SHORTY") {
                        overlay->draw_text(screen_position, IM_COL32(250, 92, 0, 255), "[SHORTY]", true);
                    }
                    else if (item_name == "SMG") {
                        overlay->draw_text(screen_position, IM_COL32(250, 92, 0, 255), "[SMG]", true);
                    }
                    else if (item_name == "RIFLE") {
                        overlay->draw_text(screen_position, IM_COL32(250, 92, 0, 255), "[RIFLE]", true);
                    }
                    else if (item_name == "SHOTGUN") {
                        overlay->draw_text(screen_position, IM_COL32(250, 92, 0, 255), "[SHOTGUN]", true);
                    }
                    if (weapon_details) {
                        overlay->draw_text({ static_cast<float>(screen_position.x), static_cast<float>(screen_position.y) + 15 }, IM_COL32(250, 92, 0, 255), ("[Ammo: " + std::to_string(item_value) + "]").c_str(), true);
                    }
                }
            }
        }

        if (item_name == "KNIFE" || item_name == "C4" || item_name == "DETONATOR") {
            auto item_root = item->get_root_component();
            if (!item_root) continue;

            if (weapon_esp) {
                auto position = item_root->get_relative_location();
                vector3 screen_position{};

                if (util::w2s(position, last_frame_cached.pov, screen_position)) {
                    if (item_name == "KNIFE") {
                        overlay->draw_text(screen_position, IM_COL32(250, 92, 0, 255), "[KNIFE]", true); // Orange
                    }
                    else if (item_name == "C4") {
                        overlay->draw_text(screen_position, IM_COL32(250, 92, 0, 255), "[C4]", true);
                    }
                    else if (item_name == "DETONATOR") {
                        overlay->draw_text(screen_position, IM_COL32(250, 92, 0, 255), "[DETONATOR]", true);
                    }
                }
            }
        }

        if (item_name == "GAZ BOTTLE" || item_name == "VENT FILTER" || item_name == "RICE" || item_name == "PACKAGE" || item_name == "SAMPLE") {
            auto item_root = item->get_root_component();
            if (!item_root) continue;

            if (primary_object_esp) {
                auto position = item_root->get_relative_location();
                vector3 screen_position{};

                if (util::w2s(position, last_frame_cached.pov, screen_position)) {
                    if (item_name == "GAZ BOTTLE") {
                        overlay->draw_text(screen_position, IM_COL32(0, 83, 250, 255), "[GAZ BOTTLE]", true); // Blue

                        if (primary_object_details) {
                            if (item_value == 0) {
                                overlay->draw_text({ static_cast<float>(screen_position.x), static_cast<float>(screen_position.y) + 15 }, IM_COL32(0, 83, 250, 255), "[Color: Yellow]", true);
                            }
                            else if (item_value == 1) {
                                overlay->draw_text({ static_cast<float>(screen_position.x), static_cast<float>(screen_position.y) + 15 }, IM_COL32(0, 83, 250, 255), "[Color: Red]", true);
                            }
                            else if (item_value == 2) {
                                overlay->draw_text({ static_cast<float>(screen_position.x), static_cast<float>(screen_position.y) + 15 }, IM_COL32(0, 83, 250, 255), "[Color: Blue]", true);
                            }
                        }
                    }
                    else if (item_name == "VENT FILTER") {
                        overlay->draw_text(screen_position, IM_COL32(65, 115, 217, 255), "[VENT FILTER]", true); // Light Blue

                        if (primary_object_details) {
                            overlay->draw_text({ static_cast<float>(screen_position.x), static_cast<float>(screen_position.y) + 15 }, IM_COL32(65, 115, 217, 255), ("[Clean %: " + std::to_string(item_value) + "]").c_str(), true);
                        }
                    }
                    else if (item_name == "RICE") {
                        if (item_value == 1) {
                            overlay->draw_text(screen_position, IM_COL32(201, 169, 122, 255), "[WHITE RICE]", true);
                        }
                        else if (item_value == 2) {
                            overlay->draw_text(screen_position, IM_COL32(201, 169, 122, 255), "[BROWN RICE]", true);
                        }
                        else if (item_value == 3) {
                            overlay->draw_text(screen_position, IM_COL32(201, 169, 122, 255), "[BLACK RICE]", true);
                        }
                    }
                    else if (item_name == "PACKAGE") {
                        overlay->draw_text(screen_position, IM_COL32(87, 47, 24, 255), "[PACKAGE]", true); // Brown

                        if (primary_object_details) {
                            if (item_value == 1) {
                                overlay->draw_text({ static_cast<float>(screen_position.x), static_cast<float>(screen_position.y) + 15 }, IM_COL32(87, 47, 24, 255), "[Security]", true);
                            }
                            else if (item_value == 2) {
                                overlay->draw_text({ static_cast<float>(screen_position.x), static_cast<float>(screen_position.y) + 15 }, IM_COL32(87, 47, 24, 255), "[Computers]", true);
                            }
                            else if (item_value == 3) {
                                overlay->draw_text({ static_cast<float>(screen_position.x), static_cast<float>(screen_position.y) + 15 }, IM_COL32(87, 47, 24, 255), "[Botanic]", true);
                            }
                            else if (item_value == 4) {
                                overlay->draw_text({ static_cast<float>(screen_position.x), static_cast<float>(screen_position.y) + 15 }, IM_COL32(87, 47, 24, 255), "[Restaurant]", true);
                            }
                            else if (item_value == 5) {
                                overlay->draw_text({ static_cast<float>(screen_position.x), static_cast<float>(screen_position.y) + 15 }, IM_COL32(87, 47, 24, 255), "[Medical]", true);
                            }
                            else if (item_value == 6) {
                                overlay->draw_text({ static_cast<float>(screen_position.x), static_cast<float>(screen_position.y) + 15 }, IM_COL32(87, 47, 24, 255), "[Tutorial]", true);
                            }
                            else if (item_value == 7) {
                                overlay->draw_text({ static_cast<float>(screen_position.x), static_cast<float>(screen_position.y) + 15 }, IM_COL32(87, 47, 24, 255), "[Machine]", true);
                            }
                        }
                    }
                    else if (item_name == "SAMPLE") {
                        if (item_value == 1) {
                            overlay->draw_text(screen_position, IM_COL32(92, 6, 191, 255), "[GREEN SAMPLE]", true);
                        }
                        else if (item_value == 2) {
                            overlay->draw_text(screen_position, IM_COL32(92, 6, 191, 255), "[YELLOW SAMPLE]", true);
                        }
                        else if (item_value == 3) {
                            overlay->draw_text(screen_position, IM_COL32(92, 6, 191, 255), "[BLUE SAMPLE]", true);
                        }
                        else if (item_value == 4) {
                            overlay->draw_text(screen_position, IM_COL32(92, 6, 191, 255), "[WHITE SAMPLE]", true);
                        }
                        else if (item_value == 5) {
                            overlay->draw_text(screen_position, IM_COL32(92, 6, 191, 255), "[RED SAMPLE]", true);
                        }
                    }
                }
            }
        }

        if (item_name == "FUSE" || item_name == "BATTERY" || item_name == "SCREW DRIVER" || item_name == "CONTAINER") {
            auto item_root = item->get_root_component();
            if (!item_root) continue;

            if (secondary_object_esp) {
                auto position = item_root->get_relative_location();
                vector3 screen_position{};

                if (util::w2s(position, last_frame_cached.pov, screen_position)) {
                    if (item_name == "FUSE") {
                        overlay->draw_text(screen_position, IM_COL32(105, 105, 105, 255), "[FUSE]", true); // Grey

                        if (secondary_object_details) {
                            const char* color_names[] = { "INVALID", "RED", "YELLOW", "BLUE" };

                            int color_index_value = (item_value >= 1 && item_value <= 3) ? item_value : 0;
                            int color_index_time = (item_time >= 1 && item_time <= 3) ? item_time : 0;

                            std::string fuse_info = "[" + std::string(color_names[color_index_value]) + " | " + std::string(color_names[color_index_time]) + "]";

                            overlay->draw_text({ static_cast<float>(screen_position.x), static_cast<float>(screen_position.y) + 15 }, IM_COL32(105, 105, 105, 255), fuse_info.c_str(), true);
                        }
                    }
                    else if (item_name == "BATTERY") {
                        overlay->draw_text(screen_position, IM_COL32(189, 189, 189, 255), "[BATTERY]", true); // Light Grey

                        if (secondary_object_details) {
                            overlay->draw_text({ static_cast<float>(screen_position.x), static_cast<float>(screen_position.y) + 15 }, IM_COL32(189, 189, 189, 255), ("[Charge %: " + std::to_string(item_value) + "]").c_str(), true);
                        }
                    }
                    else if (item_name == "SCREW DRIVER") {
                        overlay->draw_text(screen_position, IM_COL32(255, 255, 255, 255), "[SCREW DRIVER]", true); // Tan-ish
                    }
                    else if (item_name == "CONTAINER") {
                        overlay->draw_text(screen_position, IM_COL32(136, 0, 145, 255), "[CONTAINER]", true); // Pink-sih purple

                        if (secondary_object_details) {
                            if (item_value == -1) {
                                overlay->draw_text({ static_cast<float>(screen_position.x), static_cast<float>(screen_position.y) + 15 }, IM_COL32(136, 0, 145, 255), "[Dirty]", true);
                            }
                            if (item_value == 0) {
                                overlay->draw_text({ static_cast<float>(screen_position.x), static_cast<float>(screen_position.y) + 15 }, IM_COL32(136, 0, 145, 255), "[Clean]", true);
                            }
                            else if (item_value == 1) {
                                overlay->draw_text({ static_cast<float>(screen_position.x), static_cast<float>(screen_position.y) + 15 }, IM_COL32(136, 0, 145, 255), "[Green]", true);
                            }
                            else if (item_value == 2) {
                                overlay->draw_text({ static_cast<float>(screen_position.x), static_cast<float>(screen_position.y) + 15 }, IM_COL32(136, 0, 145, 255), "[Yellow]", true);
                            }
                            else if (item_value == 3) {
                                overlay->draw_text({ static_cast<float>(screen_position.x), static_cast<float>(screen_position.y) + 15 }, IM_COL32(136, 0, 145, 255), "[Blue", true);
                            }
                            else if (item_value == 4) {
                                overlay->draw_text({ static_cast<float>(screen_position.x), static_cast<float>(screen_position.y) + 15 }, IM_COL32(136, 0, 145, 255), "[White]", true);
                            }
                            else if (item_value == 5) {
                                overlay->draw_text({ static_cast<float>(screen_position.x), static_cast<float>(screen_position.y) + 15 }, IM_COL32(136, 0, 145, 255), "[Red]", true);
                            }
                            else if (item_value == 6) {
                                if (item_time == 1) {
                                    overlay->draw_text({ static_cast<float>(screen_position.x), static_cast<float>(screen_position.y) + 15 }, IM_COL32(136, 0, 145, 255), "[White Rice]", true);
                                }
                                else if (item_time == 2) {
                                    overlay->draw_text({ static_cast<float>(screen_position.x), static_cast<float>(screen_position.y) + 15 }, IM_COL32(136, 0, 145, 255), "[Brown Rice]", true);
                                }
                                else if (item_time == 3) {
                                    overlay->draw_text({ static_cast<float>(screen_position.x), static_cast<float>(screen_position.y) + 15 }, IM_COL32(136, 0, 145, 255), "[Black Rice]", true);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

int main() {
    InitializeItems();

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

    std::thread cache_thread(cache_useful);
    cache_thread.detach();

    while (true) {
        overlay->msg_loop();
    }
}