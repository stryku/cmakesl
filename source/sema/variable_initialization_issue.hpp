#pragma once

#include "lexer/token.hpp"

#include <variant>

namespace cmsl::sema {
class sema_type;

namespace variable_initialization_issues {
struct different_types
{
  const sema_type& expected;
  const sema_type& got;
};

struct reference_init_from_temporary_value
{
};

struct designated_different_types
{
  lexer::token member_name;
  const sema_type& member_type;
  const sema_type& init_type;
};

struct designated_reference_init_from_temporary_value
{
  lexer::token member_name;
  const sema_type& member_type;
};

struct designated_member_not_found
{
  lexer::token member_name;
  const sema_type& type;
};

struct designated_member_initialized_multiple_times
{
  lexer::token member_name;
  lexer::token first_initialization;
};
}

using variable_initialization_issue_t = std::variant<
  variable_initialization_issues::different_types,
  variable_initialization_issues::reference_init_from_temporary_value,
  variable_initialization_issues::designated_different_types,
  variable_initialization_issues::
    designated_reference_init_from_temporary_value,
  variable_initialization_issues::designated_member_not_found,
  variable_initialization_issues::
    designated_member_initialized_multiple_times>;
}
