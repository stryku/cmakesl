#pragma once

#include "common/string.hpp"

#include <memory>

// Todo: is it needed?
namespace cmsl::exec::inst
{
            class instance;
            using instance_members_t = string_view_map<std::unique_ptr<instance>>;
}
