#pragma once

#include "visibility.hpp"

#include <optional>
#include <string>
#include <tuple>
#include <vector>

namespace cmsl::facade {
class cmake_facade
{
public:
  struct version
  {
    size_t major;
    size_t minor;
    size_t patch;
    size_t tweak;

    bool operator==(const version& other) const
    {
      return std::tie(major, minor, patch, tweak) ==
        std::tie(other.major, other.minor, other.patch, other.tweak);
    }

    bool operator<(const version& other) const
    {
      const auto lhs = { major, minor, patch, tweak };
      const auto rhs = { other.major, other.minor, other.patch, other.tweak };

      return std::lexicographical_compare(std::cbegin(lhs), std::cend(lhs),
                                          std::cbegin(rhs), std::cend(rhs));
    }

    bool operator<=(const version& other) const
    {
      return *this < other || *this == other;
    }
  };

  virtual ~cmake_facade() = default;

  virtual version get_cmake_version() const = 0;

  virtual void message(const std::string& what) const = 0;
  virtual void warning(const std::string& what) const = 0;
  virtual void error(const std::string& what) const = 0;
  virtual void fatal_error(const std::string& what) = 0;
  virtual bool did_fatal_error_occure() const = 0;

  virtual void register_project(const std::string& name) = 0;

  virtual void install(const std::string& target_name) = 0;

  virtual std::string get_current_binary_dir() const = 0;
  virtual std::string get_current_source_dir() const = 0;
  virtual std::string get_root_source_dir() const = 0;

  virtual void add_custom_command(const std::vector<std::string>& command,
                                  const std::string& output) const = 0;

  virtual void make_directory(const std::string& dir) const = 0;

  virtual void add_executable(
    const std::string& name,
    const std::vector<std::string>&
      sources) = 0; // Todo: Change sources to vector of string_views
  virtual void add_library(
    const std::string& name,
    const std::vector<std::string>&
      sources) = 0; // Todo: Change sources to vector of string_views

  virtual void target_link_library(const std::string& target_name,
                                   visibility v,
                                   const std::string& library_name) = 0;

  virtual void target_include_directories(
    const std::string& target_name, visibility v,
    const std::vector<std::string>&
      dirs) = 0; // Todo: Change dirs to vector of string_views

  virtual std::string current_directory() const = 0;

  virtual void go_into_subdirectory(const std::string& dir) = 0;

  virtual void go_directory_up() = 0;

  virtual void enable_ctest() const = 0;

  virtual void add_test(const std::string& test_executable_name) = 0;

  enum class cxx_compiler_id
  {
    clang
  };

  struct cxx_compiler_info
  {
    cxx_compiler_id id;
  };

  virtual cxx_compiler_info get_cxx_compiler_info() const = 0;

  virtual std::optional<std::string> try_get_extern_define(
    const std::string& name) const = 0;

  virtual void set_property(const std::string& property_name,
                            const std::string& property_value) const = 0;

  virtual std::optional<bool> get_option_value(
    const std::string& name) const = 0;
  virtual void register_option(const std::string& name,
                               const std::string& description,
                               bool value) const = 0;
};
}
