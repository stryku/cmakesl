#pragma once

#include <functional>

namespace cmsl::sema{
    class sema_type;
    struct identifier_info
    {
        std::reference_wrapper<const sema_type> type;
        unsigned index;
    };
}