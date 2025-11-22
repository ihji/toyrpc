#pragma once

#include <string>
#include <vector>

namespace toyrpc {
enum class Type {
  I64,
  STRING,
};

struct Param {
  Type type;
  std::string name;
};

struct Method {
  std::string method_name;
  std::vector<Param> params;
  Type return_type;
};

struct Service {
  std::string service_name;
  std::vector<Method> methods;
};
} // namespace toyrpc