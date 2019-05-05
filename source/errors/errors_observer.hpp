#pragma once

namespace cmsl::errors {
struct error;

class errors_observer
{
public:
  void nofify_error(const error& error);
};
}
