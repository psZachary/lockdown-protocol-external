#pragma once
#include "macros.hpp"
#include "uobject.hpp"

namespace protocol::engine::sdk {
	class u_object; // Forward declaration

	class u_field : public u_object {
	public:
		GET_OFFSET(0x28, next, u_field*);
	};
}

