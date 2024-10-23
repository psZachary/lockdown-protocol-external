#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <comdef.h>
#include <iostream>


namespace mem {
    inline DWORD process_id = NULL;
    inline HWND hwnd = NULL;
    inline uintptr_t module_base = NULL;
    inline HANDLE process = NULL;

    inline std::uint64_t get_module_base(const char* name, ULONG pid)
    {
        HANDLE Module = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
        MODULEENTRY32 Entry;
        Entry.dwSize = sizeof(Entry);

        WCHAR* ModuleNameChar;
        int Chars = MultiByteToWideChar(CP_ACP, 0, name, -1, NULL, 0);
        ModuleNameChar = new WCHAR[Chars];
        MultiByteToWideChar(CP_ACP, 0, name, -1, (LPWSTR)ModuleNameChar, Chars);

        while (Module32Next(Module, &Entry)) {
            if (!wcscmp((wchar_t*)Entry.szModule, ModuleNameChar)) {
                CloseHandle(Module);
                return std::uint64_t(Entry.modBaseAddr);
            }
        }

        CloseHandle(Module);
        Entry.modBaseAddr = 0x0;
        return std::uint64_t(Entry.modBaseAddr);
    }

    inline std::uint64_t get_module_size(const char* name, ULONG pid)
    {
        HANDLE Module = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
        MODULEENTRY32 Entry;
        Entry.dwSize = sizeof(Entry);

        WCHAR* ModuleNameChar;
        int Chars = MultiByteToWideChar(CP_ACP, 0, name, -1, NULL, 0);
        ModuleNameChar = new WCHAR[Chars];
        MultiByteToWideChar(CP_ACP, 0, name, -1, (LPWSTR)ModuleNameChar, Chars);

        while (Module32Next(Module, &Entry)) {
            if (!wcscmp((wchar_t*)Entry.szModule, ModuleNameChar)) {
                CloseHandle(Module);
                return std::uint64_t(Entry.modBaseSize);
            }
        }

        CloseHandle(Module);
        return std::uint64_t(NULL);
    }

    template <typename T>
    inline void wpm(uintptr_t address, T buffer) {
        if (address < 0x10000) return;

        WriteProcessMemory(process, (LPVOID)address, &buffer, sizeof(buffer), NULL);
    }

    inline void read_raw(uintptr_t address, void* buffer, size_t size) {
        ReadProcessMemory(process, (LPCVOID)address, buffer, size, NULL);
    }

    inline void* virtallocex(LPVOID address, SIZE_T dwSize, DWORD allocation_type, DWORD protection)
    {
        return VirtualAllocEx(process, address, dwSize, allocation_type, protection);
    }

    inline int attach(const char* window_name) {

        hwnd = FindWindowA(NULL, window_name);
        if (!hwnd) return 1;

        GetWindowThreadProcessId(hwnd, &process_id);
        if (!process_id) return 2;

        process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
        if (!process) return 3;

        return 0;
    }

    template <typename T>
    inline T rpm(uintptr_t address) {
        T data = T();

        mem::read_raw(address, (void*)(&data), sizeof(data));

        return data;
    }
}