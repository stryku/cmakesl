#pragma once

#include "common/int_alias.hpp"

#include <deque>
#include <memory>

namespace cmsl::exec::inst {
class instance;

class list_value
{
private:
  using element_t = std::unique_ptr<instance>;
  using container_t = std::deque<element_t>;

  static constexpr int_t k_special_value{ -1 };

public:
  list_value() = default;
  explicit list_value(container_t values);

  list_value(list_value&&);
  list_value& operator=(list_value&&);

  list_value(const list_value& other);
  list_value& operator=(const list_value& other);

  ~list_value();

  bool operator==(const list_value& other) const;
  bool operator!=(const list_value& other) const;
  bool operator<(const list_value& other) const;
  bool operator<=(const list_value& other) const;
  bool operator>(const list_value& other) const;
  bool operator>=(const list_value& other) const;

  void push_back(element_t element);
  void push_back(const list_value& other);
  void push_front(element_t element);
  void push_front(const list_value& other);
  void pop_back();
  void pop_front();

  instance& at(int_t index);
  const instance& at(int_t index) const;
  instance& front();
  instance& back();

  void insert(int_t pos, element_t element);
  void insert(int_t pos, const list_value& other);

  void erase(int_t pos, int_t count = k_special_value);
  int_t remove(const instance& value, int_t count = k_special_value);
  int_t remove_last(const instance& value, int_t count);
  void clear();

  void resize(int_t new_size, const instance* fill);

  void sort();
  void reverse();

  int_t min() const;
  int_t max() const;

  list_value sublist(int_t pos, int_t count = k_special_value) const;

  int_t size() const;
  bool empty() const;

  int_t find(const instance& value, int_t pos = k_special_value) const;
  // Todo: implement when instance factory is available here.
  // list_value find_all(const instance& value, int_t pos = k_special_value);

private:
  void append_with_copy(const list_value& other);
  void append_with_copy_to(const list_value& other,
                           container_t& container) const;

  container_t copy(const list_value& other) const;
  int_t interpret_special_value(int_t value, int_t special_value) const;
  container_t::const_iterator place(int_t pos) const;

  template <typename IndexCalculator>
  int_t remove_impl(const instance& value, int_t count,
                    IndexCalculator&& indexCalculator);

  container_t::iterator begin();
  container_t::iterator end();
  container_t::const_iterator cbegin() const;
  container_t::const_iterator cend() const;

private:
  container_t m_list;
};
}
