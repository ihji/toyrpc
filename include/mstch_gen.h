#pragma once

#include <ast.h>
#include <mstch/mstch.hpp>

namespace toyrpc {
mstch::map convertServiceToMstch(const Service &service);
} // namespace toyrpc