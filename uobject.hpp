#pragma once
#include "macros.hpp"
#include "common.hpp"

namespace protocol::engine::sdk {
    class u_object {
    public:
        virtual ~u_object() {} // Virtual destructor for polymorphism

        int fname_index() {
            return mem::rpm<int>(reinterpret_cast<uintptr_t>(this) + 0x18); // FName index offset
        }

        u_object* super() {
            return mem::rpm<u_object*>(reinterpret_cast<uintptr_t>(this) + 0x40); // Super offset
        }

        u_object* class_private() {
            return mem::rpm<u_object*>(reinterpret_cast<uintptr_t>(this) + 0x10); // ClassPrivate offset
        }

        u_object* outer() {
            return mem::rpm<u_object*>(reinterpret_cast<uintptr_t>(this) + 0x20); // Outer offset
        }

        bool is_a(const std::string& class_name) {
            u_object* obj_class = this->class_private();
            while (obj_class) {
                int fname_idx = obj_class->fname_index();
                std::string current_class_name = util::get_name_from_fname(fname_idx);

                if (current_class_name == class_name) {
                    return true;
                }

                obj_class = obj_class->super(); // Move to the superclass
            }
            return false;
        }
    };
}
