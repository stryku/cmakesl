#pragma once

namespace cmsl::errors {
struct error;

class errors_observer
{
public:
  void notify_error(const error& error);
};
}
