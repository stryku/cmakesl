#include "sema/factories.hpp"
#include "common/assert.hpp"
#include "errors/error.hpp"
#include "errors/errors_observer.hpp"
#include "factories.hpp"
#include "sema/builtin_function_kind.hpp"
#include "sema/builtin_sema_function.hpp"
#include "sema/builtin_token_provider.hpp"
#include "sema/builtin_types_finder.hpp"
#include "sema/homogeneous_generic_type.hpp"
#include "sema/sema_context.hpp"
#include "sema/sema_type.hpp"
#include "sema/type_builder.hpp"
#include "sema/user_sema_function.hpp"

#include <generated/builtin_token_providers.hpp>

namespace {
template <unsigned N>
cmsl::lexer::token make_id_token(const char (&tok)[N])
{
  return cmsl::lexer::make_token(cmsl::lexer::token_type ::identifier, tok);
}
}

namespace cmsl::sema {
user_sema_function& sema_function_factory::create_user(
  const sema_context& ctx, const sema_type* return_type, function_signature s)
{
  return create_impl<user_sema_function>(ctx, return_type, std::move(s));
}
builtin_sema_function& sema_function_factory::create_builtin(
  const sema_context& ctx, const sema_type& return_type, function_signature s,
  builtin_function_kind kind)
{
  return create_impl<builtin_sema_function>(ctx, return_type, std::move(s),
                                            kind);
}

sema_function_factory::~sema_function_factory()
{
}

sema_context_impl& sema_context_factory::create(std::string name,
                                                const sema_context* parent)
{
  return create_impl<sema_context_impl>(
    name, parent, sema_context::context_type::namespace_);
}

sema_context_impl& sema_context_factory::create_class(
  std::string name, const sema_context* parent)
{
  return create_impl<sema_context_impl>(name, parent,
                                        sema_context::context_type::class_);
}

sema_context_factory::~sema_context_factory()
{
}

const sema_type& sema_type_factory::create(const sema_context& ctx,
                                           ast::type_representation name,
                                           std::vector<member_info> members)
{
  return create_and_add<sema_type>(name.primary_name_token(), ctx, name,
                                   std::move(members));
}

const sema_type& sema_type_factory::create_builtin(
  const sema_context& ctx, ast::type_representation name,
  std::vector<member_info> members)
{
  return create_and_add<sema_type>(name.primary_name_token(),
                                   sema_type::builtin_tag{}, ctx, name,
                                   std::move(members));
}

const sema_type& sema_type_factory::create_homogeneous_generic(
  const sema_context& ctx, ast::type_representation name,
  const sema_type& value_type)
{
  return create_and_add<homogeneous_generic_type>(
    name.primary_name_token(), ctx, std::move(name), value_type);
}

const sema_type& sema_type_factory::create_reference(
  const sema_type& referenced_type)
{
  const auto primary_name_token = referenced_type.name().primary_name_token();
  if (referenced_type.is_builtin()) {
    return create_and_add<sema_type>(primary_name_token,
                                     sema_type::builtin_tag{},
                                     sema_type_reference{ referenced_type });
  } else {
    return create_and_add<sema_type>(primary_name_token,
                                     sema_type_reference{ referenced_type });
  }
}

sema_type_factory::sema_type_factory(types_context& types_ctx)
  : m_types_ctx{ types_ctx }
{
}

sema_type_factory::~sema_type_factory()
{
}

const sema_type& sema_type_factory::create_designated_initializer(
  const sema_context& ctx, ast::type_representation name)
{
  return create_and_add<sema_type>(name.primary_name_token(),
                                   sema_type::designated_initializer_tag{},
                                   ctx, name);
}

template <typename T, typename... Args>
const sema_type& sema_type_factory::create_and_add(
  const lexer::token& primary_name, Args&&... args)
{
  const auto& created = create_impl<T>(std::forward<Args>(args)...);
  m_types_ctx.register_type(primary_name, created);
  return created;
}

sema_generic_type_factory::sema_generic_type_factory(
  sema_context& generic_types_context, const sema_context& creation_context,
  sema_type_factory& type_factory, sema_function_factory& function_factory,
  sema_context_factory& context_factory,
  errors::errors_observer& errors_observer,
  const builtin_token_provider& builtin_token_provider,
  builtin_types_accessor builtin_types, types_context& types_ctx)
  : sema_type_factory{ types_ctx }
  , m_generic_types_context{ generic_types_context }
  , m_creation_context{ creation_context }
  , m_type_factory{ type_factory }
  , m_function_factory{ function_factory }
  , m_context_factory{ context_factory }
  , m_errors_observer{ errors_observer }
  , m_builtin_token_provider{ builtin_token_provider }
  , m_builtin_types{ builtin_types }
{
}

const sema_type* sema_generic_type_factory::create_generic(
  const ast::type_representation& name)
{
  const auto primary_name = name.generic_name().primary_name();
  if (primary_name.str() == "list") {
    return create_list(name);
  }

  CMSL_UNREACHABLE("Creating unknown generic type");
  return nullptr;
}

const sema_type* sema_generic_type_factory::create_list(
  const ast::type_representation& name)
{
  const auto generic_parameters_error_creator = [&name] {
    errors::error err;
    err.message = "list<> expects one generic parameter, got " +
      std::to_string(name.nested_types().size());
    err.range = name.src_range();
    err.type = errors::error_type::error;
    const auto source = name.generic_name().primary_name().source();
    err.source_path = source.path();
    const auto line_info =
      source.line(name.generic_name().primary_name().src_range().begin.line);
    err.line_start_pos = line_info.start_pos;
    err.line_snippet = line_info.line;
    return err;
  };

  const auto& nested_types = name.nested_types();
  if (nested_types.empty()) {
    const auto err = generic_parameters_error_creator();
    m_errors_observer.nofify_error(err);
    return nullptr;
  }
  constexpr auto list_generic_parameters{ 1u };
  if (nested_types.size() > list_generic_parameters) {
    const auto err = generic_parameters_error_creator();
    m_errors_observer.nofify_error(err);
    return nullptr;
  }

  const auto& value_type_representation = name.nested_types().front();
  const auto value_type =
    try_get_or_create_value_type(value_type_representation);
  if (!value_type) {
    return nullptr;
  }

  const auto list_name_representation = prepare_list_name_representation(name);
  type_builder builder{ m_type_factory, m_function_factory, m_context_factory,
                        m_generic_types_context, list_name_representation };
  builder.build_homogeneous_generic_and_register_in_context(*value_type);

  // At this point we know that list type is created and registered in context.
  // We can safely dereference it.
  const auto& list_type =
    *m_generic_types_context.find_type(list_name_representation);
  const auto& int_type = m_builtin_types.int_;
  const auto& void_type = m_builtin_types.void_;
  const auto& bool_type = m_builtin_types.bool_;

  const auto token_provider = m_builtin_token_provider.list();

  auto functions = {
    type_builder::builtin_function_info{
      // list()
      list_type,
      function_signature{ token_provider.default_constructor(), {} },
      builtin_function_kind::list_ctor },
    // Todo: push_back could return reference
    type_builder::builtin_function_info{
      // void push_back(value_type)
      void_type,
      function_signature{
        token_provider.push_back_value(),
        { parameter_declaration{ *value_type, make_id_token("") } } },
      builtin_function_kind::list_push_back_value },
    // Todo: push_back could return reference
    type_builder::builtin_function_info{
      // void push_back(list)
      void_type,
      function_signature{
        token_provider.push_back_list(),
        { parameter_declaration{ list_type, make_id_token("") } } },
      builtin_function_kind::list_push_back_list },
    // Todo: push_front could return reference
    type_builder::builtin_function_info{
      // void push_front(value_type)
      void_type,
      function_signature{
        token_provider.push_front_value(),
        { parameter_declaration{ *value_type, make_id_token("") } } },
      builtin_function_kind::list_push_front_value },
    // Todo: push_front could return reference
    type_builder::builtin_function_info{
      // void push_front(list)
      void_type,
      function_signature{
        token_provider.push_front_list(),
        { parameter_declaration{ list_type, make_id_token("") } } },
      builtin_function_kind::list_push_front_list },
    // Todo: pop_back could return reference
    type_builder::builtin_function_info{
      // void pop_back()
      void_type, function_signature{ token_provider.pop_back(), {} },
      builtin_function_kind::list_pop_back },
    // Todo: pop_front could return reference
    type_builder::builtin_function_info{
      // void pop_front()
      void_type, function_signature{ token_provider.pop_front(), {} },
      builtin_function_kind::list_pop_front },
    type_builder::builtin_function_info{
      // value_type& at(int)
      *value_type,
      function_signature{
        token_provider.at(),
        { parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::list_at },
    // Todo: front should return a reference
    type_builder::builtin_function_info{
      // value_type front()
      *value_type, function_signature{ token_provider.front(), {} },
      builtin_function_kind::list_front },
    // Todo: back should return a reference
    type_builder::builtin_function_info{
      // value_type back()
      *value_type, function_signature{ token_provider.back(), {} },
      builtin_function_kind::list_back },
    // Todo: insert could return reference
    type_builder::builtin_function_info{
      // void insert(int pos, value_type)
      void_type,
      function_signature{
        token_provider.insert_pos_value(),
        { parameter_declaration{ int_type, make_id_token("") },
          parameter_declaration{ *value_type, make_id_token("") } } },
      builtin_function_kind::list_insert_pos_value },
    // Todo: insert could return reference
    type_builder::builtin_function_info{
      // void insert(int pos, list)
      void_type,
      function_signature{
        token_provider.insert_pos_list(),
        { parameter_declaration{ int_type, make_id_token("") },
          parameter_declaration{ list_type, make_id_token("") } } },
      builtin_function_kind::list_insert_pos_list },
    // Todo: erase could return reference
    type_builder::builtin_function_info{
      // void erase(int pos)
      void_type,
      function_signature{
        token_provider.erase_pos(),
        { parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::list_erase_pos },
    // Todo: erase could return reference
    type_builder::builtin_function_info{
      // void erase(int pos, int count)
      void_type,
      function_signature{
        token_provider.erase_pos_count(),
        { parameter_declaration{ int_type, make_id_token("") },
          parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::list_erase_pos_count },
    type_builder::builtin_function_info{
      // int remove(value_type)
      int_type,
      function_signature{
        token_provider.remove_value(),
        { parameter_declaration{ *value_type, make_id_token("") } } },
      builtin_function_kind::list_remove_value },
    type_builder::builtin_function_info{
      // int remove(value_type, int count)
      int_type,
      function_signature{
        token_provider.remove_value_count(),
        { parameter_declaration{ *value_type, make_id_token("") },
          parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::list_remove_value_count },
    type_builder::builtin_function_info{
      // int remove_last(value_type, int count)
      int_type,
      function_signature{
        token_provider.remove_last(),
        { parameter_declaration{ *value_type, make_id_token("") },
          parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::list_remove_last_value_count },
    type_builder::builtin_function_info{
      // void clear()
      void_type, function_signature{ token_provider.clear(), {} },
      builtin_function_kind::list_clear },
    type_builder::builtin_function_info{
      // void resize(int new_size)
      void_type,
      function_signature{
        token_provider.resize(),
        { parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::list_resize },
    type_builder::builtin_function_info{
      // void sort()
      void_type, function_signature{ token_provider.sort(), {} },
      builtin_function_kind::list_sort },
    type_builder::builtin_function_info{
      // void reverse()
      void_type, function_signature{ token_provider.reverse(), {} },
      builtin_function_kind::list_reverse },
    type_builder::builtin_function_info{
      // int min()
      int_type, function_signature{ token_provider.min(), {} },
      builtin_function_kind::list_min },
    type_builder::builtin_function_info{
      // int max()
      int_type, function_signature{ token_provider.max(), {} },
      builtin_function_kind::list_max },
    type_builder::builtin_function_info{
      // list sublist(int pos)
      list_type,
      function_signature{
        token_provider.sublist_pos(),
        { parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::list_sublist_pos },
    type_builder::builtin_function_info{
      // list sublist(int pos, int count)
      list_type,
      function_signature{
        token_provider.sublist_pos_count(),
        { parameter_declaration{ int_type, make_id_token("") },
          parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::list_sublist_pos_count },
    type_builder::builtin_function_info{
      // int size()
      int_type, function_signature{ token_provider.size(), {} },
      builtin_function_kind::list_size },
    type_builder::builtin_function_info{
      // bool empty()
      bool_type, function_signature{ token_provider.empty(), {} },
      builtin_function_kind::list_empty },
    type_builder::builtin_function_info{
      // int find(value_type)
      int_type,
      function_signature{
        token_provider.find_value(),
        { parameter_declaration{ *value_type, make_id_token("") } } },
      builtin_function_kind::list_find_value },
    type_builder::builtin_function_info{
      // int find(value_type, int pos)
      int_type,
      function_signature{
        token_provider.find_value_pos(),
        { parameter_declaration{ *value_type, make_id_token("") },
          parameter_declaration{ int_type, make_id_token("") } } },
      builtin_function_kind::list_find_value_pos },
    type_builder::builtin_function_info{
      // list operator+(value_type)
      list_type,
      function_signature{
        token_provider.operator_plus_value(),
        { parameter_declaration{ *value_type, make_id_token("") } } },
      builtin_function_kind::list_operator_plus_value },
    type_builder::builtin_function_info{
      // list operator+(list)
      list_type,
      function_signature{
        token_provider.operator_plus_list(),
        { parameter_declaration{ list_type, make_id_token("") } } },
      builtin_function_kind::list_operator_plus_list },
    type_builder::builtin_function_info{
      // list& operator+=(value_type)
      list_type,
      function_signature{
        token_provider.operator_plus_equal_value(),
        { parameter_declaration{ *value_type, make_id_token("") } } },
      builtin_function_kind::list_operator_plus_equal_value },
    type_builder::builtin_function_info{
      // list& operator+=(list)
      list_type,
      function_signature{
        token_provider.operator_plus_equal_list(),
        { parameter_declaration{ list_type, make_id_token("") } } },
      builtin_function_kind::list_operator_plus_equal_list }
  };

  builder.with_builtin_functions(functions);

  return &list_type;
}

const sema_type* sema_generic_type_factory::try_get_or_create_value_type(
  const ast::type_representation& type_name)
{
  const auto found = m_creation_context.find_type(type_name);
  if (!type_name.is_generic()) {
    if (!found) {
      errors::error err;
      err.message = "value type not found";
      err.range = source_range{
        type_name.generic_name().primary_name().src_range().begin,
        type_name.generic_name().tokens.back().src_range().end
      };
      err.type = errors::error_type::error;
      const auto source = type_name.generic_name().primary_name().source();
      err.source_path = source.path();
      const auto line_info = source.line(
        type_name.generic_name().primary_name().src_range().begin.line);
      err.line_start_pos = line_info.start_pos;
      err.line_snippet = line_info.line;
      m_errors_observer.nofify_error(err);
      return nullptr;
    }
    return found;
  }

  if (!found) {
    // If it's a generic type and it's not found, try create it and return
    // result.
    return create_generic(type_name);
  }

  return found;
}

ast::type_representation
sema_generic_type_factory::prepare_list_name_representation(
  const ast::type_representation& name) const
{
  const auto name_token = m_builtin_token_provider.list().name();
  auto tokens = name.tokens();
  tokens.front() = name_token;
  auto nested_types = name.nested_types();

  auto generic_name =
    ast::type_representation::generic_type_name{ std::move(tokens),
                                                 std::move(nested_types) };
  return ast::type_representation{ std::move(generic_name) };
}
}
