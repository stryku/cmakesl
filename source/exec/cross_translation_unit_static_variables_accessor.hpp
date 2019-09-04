#pragma once

namespace cmsl::exec {

namespace inst {
class instance;
}

class cross_translation_unit_static_variables_accessor
{
public:
  virtual ~cross_translation_unit_static_variables_accessor() = default;
  virtual inst::instance* access_variable(unsigned index) = 0;
};
}
