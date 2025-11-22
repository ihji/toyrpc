#include <mstch_gen.h>

mstch::map toyrpc::convertServiceToMstch(const Service &service) {
  {
    mstch::map service_map;
    service_map["service_name"] = service.service_name;

    mstch::array methods_array;
    for (const auto &method : service.methods) {
      mstch::map method_map;
      method_map["method_name"] = method.method_name;

      mstch::array params_array;
      for (const auto &param : method.params) {
        bool is_i64 = (param.type == Type::I64);
        bool is_string = (param.type == Type::STRING);
        mstch::map param_map;
        param_map["type"] =
            (is_i64) ? std::string("int64_t") : std::string("std::string");
        param_map["name"] = param.name;
        param_map["is_i64"] = is_i64;
        param_map["is_string"] = is_string;
        params_array.push_back(param_map);
      }
      method_map.emplace("params", params_array);

      bool return_is_i64 = (method.return_type == Type::I64);
      bool return_is_string = (method.return_type == Type::STRING);
      method_map["return_type"] =
          (return_is_i64) ? std::string("int64_t") : std::string("std::string");
      method_map["return_is_i64"] = return_is_i64;
      method_map["return_is_string"] = return_is_string;

      methods_array.push_back(method_map);
    }
    service_map.emplace("methods", methods_array);

    return service_map;
  }
}