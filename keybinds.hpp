#include <string>
#include "overlay/imgui/imgui.h"
#include <WinUser.h>
#include <unordered_map>

static void ImHotkey(const char* label, int* k, const ImVec2& size_arg = ImVec2(0, 0))
{

	static const char* key_names[] = {
		"OFF",
		"Left Mouse Button",
		"Right Mouse Button",
		"Cancel",
		"Middle Mouse Button",
		"Back Mouse Button",
		"Front Mouse Button",
		"Unknown",
		"Back",
		"Tab",
		"Unknown",
		"Unknown",
		"Clear",
		"Return",
		"Unknown",
		"Unknown",
		"Shift",
		"Control",
		"Menu",
		"Pause",
		"Capital",
		"VK_KANA",
		"Unknown",
		"VK_JUNJA",
		"VK_FINAL",
		"VK_KANJI",
		"Unknown",
		"Escape",
		"VK_CONVERT",
		"VK_NONCONVERT",
		"VK_ACCEPT",
		"VK_MODECHANGE",
		"Space",
		"Prior",
		"Next",
		"End",
		"Home",
		"Left",
		"Up",
		"Right",
		"Down",
		"Select",
		"Print",
		"Execute",
		"Snapshot",
		"Insert",
		"Delete",
		"Help",
		"0",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"A",
		"B",
		"C",
		"D",
		"E",
		"F",
		"G",
		"H",
		"I",
		"J",
		"K",
		"L",
		"M",
		"N",
		"O",
		"P",
		"Q",
		"R",
		"S",
		"T",
		"U",
		"V",
		"W",
		"X",
		"Y",
		"Z",
		"VK_LWIN",
		"VK_RWIN",
		"VK_APPS",
		"Unknown",
		"VK_SLEEP",
		"Numpad 0",
		"Numpad 1",
		"Numpad 2",
		"Numpad 3",
		"Numpad 4",
		"Numpad 5",
		"Numpad 6",
		"Numpad 7",
		"Numpad 8",
		"Numpad 9",
		"Multiply",
		"Add",
		"Seperator",
		"Subtract",
		"Decimal",
		"Divide",
		"F1",
		"F2",
		"F3",
		"F4",
		"F5",
		"F6",
		"F7",
		"F8",
		"F9",
		"F10",
		"F11",
		"F12",
		"F13",
		"F14",
		"F15",
		"F16",
		"F17",
		"F18",
		"F19",
		"F20",
		"F21",
		"F22",
		"F23",
		"F24",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Numlock",
		"Scroll",
		"VK_OEM_NEC_EQUAL",
		"VK_OEM_FJ_MASSHOU",
		"VK_OEM_FJ_TOUROKU",
		"VK_OEM_FJ_LOYA",
		"VK_OEM_FJ_ROYA",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Left Shift",
		"Right Shift",
		"Left Control",
		"Right Control",
		"Left Menu",
		"Right Menu"
	};

	static const int key_codes[] = {
		0x0,  //Undefined
		0x01,
		0x02,
		0x03,
		0x04,
		0x05,
		0x06,
		0x07, //Undefined
		0x08,
		0x09,
		0x0A, //Reserved
		0x0B, //Reserved
		0x0C,
		0x0D,
		0x0E, //Undefined
		0x0F, //Undefined
		0x10,
		0x11,
		0x12,
		0x13,
		0x14,
		0x15,
		0x16, //IME On
		0x17,
		0x18,
		0x19,
		0x1A, //IME Off
		0x1B,
		0x1C,
		0x1D,
		0x1E,
		0x1F,
		0x20,
		0x21,
		0x22,
		0x23,
		0x24,
		0x25,
		0x26,
		0x27,
		0x28,
		0x29,
		0x2A,
		0x2B,
		0x2C,
		0x2D,
		0x2E,
		0x2F,
		0x30,
		0x31,
		0x32,
		0x33,
		0x34,
		0x35,
		0x36,
		0x37,
		0x38,
		0x39,
		0x3A, //Undefined
		0x3B, //Undefined
		0x3C, //Undefined
		0x3D, //Undefined
		0x3E, //Undefined
		0x3F, //Undefined
		0x40, //Undefined
		0x41,
		0x42,
		0x43,
		0x44,
		0x45,
		0x46,
		0x47,
		0x48,
		0x49,
		0x4A,
		0x4B,
		0x4C,
		0x4B,
		0x4E,
		0x4F,
		0x50,
		0x51,
		0x52,
		0x53,
		0x54,
		0x55,
		0x56,
		0x57,
		0x58,
		0x59,
		0x5A,
		0x5B,
		0x5C,
		0x5D,
		0x5E, //Rservered
		0x5F,
		0x60, //Numpad1
		0x61, //Numpad2
		0x62, //Numpad3
		0x63, //Numpad4
		0x64, //Numpad5
		0x65, //Numpad6
		0x66, //Numpad7
		0x67, //Numpad8
		0x68, //Numpad8
		0x69, //Numpad9
		0x6A,
		0x6B,
		0x6C,
		0x6D,
		0x6E,
		0x6F,
		0x70, //F1
		0x71, //F2
		0x72, //F3
		0x73, //F4
		0x74, //F5
		0x75, //F6
		0x76, //F7
		0x77, //F8
		0x78, //F9
		0x79, //F10
		0x7A, //F11
		0x7B, //F12
		0x7C, //F13
		0x7D, //F14
		0x7E, //F15
		0x7F, //F16
		0x80, //F17
		0x81, //F18
		0x82, //F19
		0x83, //F20
		0x84, //F21
		0x85, //F22
		0x86, //F23
		0x87, //F24
		0x88, //Unkown
		0x89, //Unkown
		0x8A, //Unkown
		0x8B, //Unkown
		0x8C, //Unkown
		0x8D, //Unkown
		0x8E, //Unkown
		0x8F, //Unkown
		0x90,
		0x91,
		0x92, //OEM Specific
		0x93, //OEM Specific
		0x94, //OEM Specific
		0x95, //OEM Specific
		0x96, //OEM Specific
		0x97, //Unkown
		0x98, //Unkown
		0x99, //Unkown
		0x9A, //Unkown
		0x9B, //Unkown
		0x9C, //Unkown
		0x9D, //Unkown
		0x9E, //Unkown 
		0x9F, //Unkown
		0xA0,
		0xA1,
		0xA2,
		0xA3,
		0xA4,
		0xA5
	};

	static std::unordered_map<std::string, bool> waiting_for_key_map{};
	if (waiting_for_key_map.find(label) == waiting_for_key_map.end())
		waiting_for_key_map.insert({ label, false });


	if (waiting_for_key_map[label] == false) {
		if (ImGui::Button((std::string(key_names[*(int*)k]) + "##" + label).c_str(), size_arg))
			waiting_for_key_map[label] = true;
	}
	else if (waiting_for_key_map[label] == true) {
		ImGui::Button("...", size_arg);
		for (auto& Key : key_codes)
		{
			if (GetAsyncKeyState(Key) && Key != VK_LBUTTON) {
				*(int*)k = Key;
				waiting_for_key_map[label] = false;
			}
		}
	}
}