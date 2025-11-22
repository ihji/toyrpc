#pragma once

#include <folly/io/IOBuf.h>
#include <string>

namespace toyrpc {
const uint8_t ADD_METHOD_ID = 1;
const uint8_t CONCAT_METHOD_ID = 2;

struct Add_Request {
  int64_t a;
  int64_t b;
  folly::IOBuf serialize() const;
  static Add_Request deserialize(const folly::IOBuf &data);
};

struct Add_Response {
  int64_t result;
  folly::IOBuf serialize() const;
  static Add_Response deserialize(const folly::IOBuf &data);
};

struct Concat_Request {
  std::string a;
  std::string b;
  folly::IOBuf serialize() const;
  static Concat_Request deserialize(const folly::IOBuf &data);
};

struct Concat_Response {
  std::string result;
  folly::IOBuf serialize() const;
  static Concat_Response deserialize(const folly::IOBuf &data);
};
} // namespace toyrpc