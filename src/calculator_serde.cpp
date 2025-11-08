#include <calculator.h>
#include <folly/io/IOBuf.h>
#include <iostream>
#include <sys/endian.h>
#include <vector>

namespace toyrpc {
folly::IOBuf Add_Request::serialize() const {
  folly::IOBuf buf(folly::IOBuf::CREATE, 17);
  buf.writableData()[0] = ADD_METHOD_ID;
  // Serialize the request fields into the byte array
  uint64_t a_be = htobe64(a);
  uint64_t b_be = htobe64(b);
  memcpy(buf.writableData() + 1, &a_be, sizeof(a_be));
  memcpy(buf.writableData() + 1 + sizeof(a_be), &b_be, sizeof(b_be));
  buf.append(17);
  return buf;
}

Add_Request Add_Request::deserialize(const folly::IOBuf &data) {
  if (data.length() != 17) {
    throw std::runtime_error("Invalid data size for Add_Request");
  }
  Add_Request request;
  memcpy(&request.a, data.data() + 1, sizeof(request.a));
  memcpy(&request.b, data.data() + 1 + sizeof(request.a), sizeof(request.b));
  request.a = be64toh(request.a);
  request.b = be64toh(request.b);
  return request;
}

folly::IOBuf Add_Response::serialize() const {
  folly::IOBuf buf(folly::IOBuf::CREATE, 8);
  // Serialize the response fields into the byte array
  uint64_t result_be = htobe64(result);
  memcpy(buf.writableData(), &result_be, sizeof(result_be));
  buf.append(8);
  return buf;
}

Add_Response Add_Response::deserialize(const folly::IOBuf &data) {
  if (data.length() != 8) {
    throw std::runtime_error("Invalid data size for Add_Response");
  }
  Add_Response response;
  memcpy(&response.result, data.data(), sizeof(response.result));
  response.result = be64toh(response.result);
  return response;
}

folly::IOBuf Concat_Request::serialize() const {
  folly::IOBuf buf(folly::IOBuf::CREATE, 9 + a.size() + b.size());
  buf.writableData()[0] = CONCAT_METHOD_ID;
  uint32_t a_length = htobe32(a.size());
  memcpy(buf.writableData() + 1, &a_length, sizeof(a_length));
  memcpy(buf.writableData() + 1 + sizeof(a_length), a.data(), a.size());
  uint32_t b_length = htobe32(b.size());
  memcpy(buf.writableData() + 1 + sizeof(a_length) + a.size(), &b_length,
         sizeof(b_length));
  memcpy(buf.writableData() + 1 + sizeof(a_length) + a.size() +
             sizeof(b_length),
         b.data(), b.size());
  buf.append(9 + a.size() + b.size());
  return buf;
}

Concat_Request Concat_Request::deserialize(const folly::IOBuf &data) {
  if (data.length() < 9) {
    throw std::runtime_error("Invalid data size for Concat_Request");
  }
  Concat_Request request;
  uint32_t a_length;
  memcpy(&a_length, data.data() + 1, sizeof(a_length));
  a_length = be32toh(a_length);
  if (data.length() < 9 + a_length) {
    throw std::runtime_error("Invalid data size for Concat_Request");
  }
  request.a =
      std::string(reinterpret_cast<const char *>(data.data() + 5), a_length);
  uint32_t b_length;
  memcpy(&b_length, data.data() + 5 + a_length, sizeof(b_length));
  b_length = be32toh(b_length);
  if (data.length() != 9 + a_length + b_length) {
    throw std::runtime_error("Invalid data size for Concat_Request");
  }
  request.b = std::string(
      reinterpret_cast<const char *>(data.data() + 9 + a_length), b_length);
  return request;
}

folly::IOBuf Concat_Response::serialize() const {
  folly::IOBuf buf(folly::IOBuf::CREATE, 4 + result.size());
  uint32_t result_length = htobe32(result.size());
  memcpy(buf.writableData(), &result_length, sizeof(result_length));
  memcpy(buf.writableData() + sizeof(result_length), result.data(),
         result.size());
  buf.append(4 + result.size());
  return buf;
}

Concat_Response Concat_Response::deserialize(const folly::IOBuf &data) {
  if (data.length() < 4) {
    throw std::runtime_error("Invalid data size for Concat_Response");
  }
  Concat_Response response;
  uint32_t result_length;
  memcpy(&result_length, data.data(), sizeof(result_length));
  result_length = be32toh(result_length);
  if (data.length() != 4 + result_length) {
    throw std::runtime_error("Invalid data size for Concat_Response");
  }
  response.result = std::string(reinterpret_cast<const char *>(data.data() + 4),
                                result_length);
  return response;
}
} // namespace toyrpc