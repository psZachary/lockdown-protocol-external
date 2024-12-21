#pragma once
#include "mem.hpp"

#define pthis (uintptr_t)this
// Does not add get_##name to the front and uses the direct name
#define GET_OFFSET_NAMED(offset, name, type) type name() { return mem::rpm<type>((uintptr_t)this + offset); }
// Does not add set_##name to the front and uses the direct name
#define SET_OFFSET_NAMED(offset, name, type) void name(type value) { mem::wpm<type>((uintptr_t)this + offset, value); }
// Adds get_##name to the front
#define GET_OFFSET(offset, name, type) GET_OFFSET_NAMED(offset, get_##name, type)
// Adds set_##name to the front
#define SET_OFFSET(offset, name, type) SET_OFFSET_NAMED(offset, set_##name, type)
#define OFFSET(offset, name, type) GET_OFFSET(offset, name, type) SET_OFFSET(offset, name, type)