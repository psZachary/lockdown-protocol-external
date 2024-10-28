#pragma once

#include "game_math.hpp"
#include "game_structures.hpp"
#include <Windows.h>
#include "mem.hpp"

namespace util {
	using namespace protocol::engine::sdk;
	using namespace protocol::game::sdk;

	static matrix4x4_t matrix(vector3 rot, vector3 origin = vector3(0, 0, 0))
	{
		double rad_pitch = (rot.x * 3.14159 / 180.);
		double rad_yaw = (rot.y * 3.14159 / 180.);
		double rad_roll = (rot.z * 3.14159 / 180.);

		double SP = sin(rad_pitch);
		double CP = cos(rad_pitch);
		double SY = sin(rad_yaw);
		double CY = cos(rad_yaw);
		double SR = sin(rad_roll);
		double CR = cos(rad_roll);

		matrix4x4_t matrix;
		matrix.m[0][0] = CP * CY;
		matrix.m[0][1] = CP * SY;
		matrix.m[0][2] = SP;
		matrix.m[0][3] = 0.f;

		matrix.m[1][0] = SR * SP * CY - CR * SY;
		matrix.m[1][1] = SR * SP * SY + CR * CY;
		matrix.m[1][2] = -SR * CP;
		matrix.m[1][3] = 0.f;

		matrix.m[2][0] = -(CR * SP * CY + SR * SY);
		matrix.m[2][1] = CY * SR - CR * SP * SY;
		matrix.m[2][2] = CR * CP;
		matrix.m[2][3] = 0.f;

		matrix.m[3][0] = origin.x;
		matrix.m[3][1] = origin.y;
		matrix.m[3][2] = origin.z;
		matrix.m[3][3] = 1.f;

		return matrix;
	}

	static vector2 screen_size() {
		static auto screen_size = vector2{};
		if (screen_size != vector2{ 0, 0 }) return screen_size;

		RECT rect;
		GetWindowRect(GetDesktopWindow(), &rect);
		screen_size = vector2{ (float)(rect.right - rect.left), (float)(rect.bottom - rect.top) };

		return screen_size;
	}

	static bool w2s(vector3 location, f_minimal_view_info camera_cache, vector3& screen_loc) {
		auto pov = camera_cache;
		vector3 rotation = pov.rotation;
		matrix4x4_t temp_matrix = matrix(rotation);

		float fov = pov.fov;

		vector3 v_axis_x = vector3(temp_matrix.m[0][0], temp_matrix.m[0][1], temp_matrix.m[0][2]);
		vector3 v_axis_y = vector3(temp_matrix.m[1][0], temp_matrix.m[1][1], temp_matrix.m[1][2]);
		vector3 v_axis_z = vector3(temp_matrix.m[2][0], temp_matrix.m[2][1], temp_matrix.m[2][2]);

		vector3 v_delta = location - pov.location;
		vector3 v_transformed = vector3(v_delta.dot(v_axis_y), v_delta.dot(v_axis_z), v_delta.dot(v_axis_x));

		if (v_transformed.z < 1.f)
			return false;


		screen_loc.x = (screen_size().x / 2) + v_transformed.x * ((screen_size().x / 2) / tanf(fov * (float)3.14159f / 360.f)) / v_transformed.z;
		screen_loc.y = (screen_size().y / 2) - v_transformed.y * ((screen_size().x / 2) / tanf(fov * (float)3.14159f / 360.f)) / v_transformed.z;

		if (screen_loc.x > screen_size().x ||
			screen_loc.y > screen_size().y ||
			screen_loc.y < 0 ||
			screen_loc.x < 0) return false;

		return true;
	}
	static vector2 cursor_position() {
		POINT cursor_point{};
		if (!GetCursorPos(&cursor_point))
			return vector2{ 0.f,0.f };
		return vector2{ static_cast<float>(cursor_point.x), static_cast<float>(cursor_point.y) };
	}
	static std::string get_name_from_fname(int key)
	{
		static std::map<int, std::string> cached_fnames{};

		auto cached_name = cached_fnames.find(key);
		if (cached_name != cached_fnames.end())
			return cached_name->second;

		auto chunkOffset = (UINT)((int)(key) >> 16);
		auto name_offset = (USHORT)key;

		auto pool_chunk = mem::rpm<UINT64>(mem::module_base + protocol::engine::GNAMES + ((chunkOffset + 2) * 8));
		auto entry_offset = pool_chunk + (ULONG)(2 * name_offset);
		auto name_entry = mem::rpm<INT16>(entry_offset);

		auto len = name_entry >> 6;
		char buff[1028];
		if ((DWORD)len && len > 0)
		{
			memset(buff, 0, 1028);
			mem::read_raw(entry_offset + 2, buff, len);
			buff[len] = '\0';
			std::string ret(buff);
			cached_fnames.emplace(key, ret);
			return std::string(ret);
		}
		else return "";
	}
}