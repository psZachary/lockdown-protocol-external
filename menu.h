#pragma once
#include <unordered_set>
#include <string>

namespace menu {
	void PopulateUniqueItems(std::unordered_set<std::string>& inserted_names);
	void draw();
}
