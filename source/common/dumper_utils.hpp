#pragma once

#include <ostream>
#include <stack>
#include <string>

namespace cmsl {
class dumper_utils
{
protected:
  explicit dumper_utils(std::ostream& os);
  ~dumper_utils();

private:
  class ident_guard
  {
  public:
    explicit ident_guard(std::string& ident);
    ~ident_guard();

    ident_guard(const ident_guard&) = delete;
    ident_guard& operator=(const ident_guard&) = delete;

    ident_guard(ident_guard&& other);
    ident_guard& operator=(ident_guard&& other);

  private:
    std::string& m_ident;
    bool m_valid{ true };
  };

  class line_out
  {
  public:
    explicit line_out(std::ostream& os)
      : m_out{ os }
    {
      m_out << m_ident;
    }
    ~line_out() { m_out << '\n'; }

    template <typename T>
    std::ostream& operator<<(T&& value)
    {
      return m_out << std::forward<T>(value);
    }

  private:
    std::ostream& m_out;
  };

protected:
  ident_guard ident();
  void push_ident();
  void pop_ident();
  line_out out();

private:
  std::ostream& m_out;
  std::stack<ident_guard> m_ident_stack;
  static std::string m_ident;
};
}
