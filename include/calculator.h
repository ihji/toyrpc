#pragma once

#include <folly/io/IOBuf.h>
#include <memory>
#include <span>
#include <stdexcept>

namespace toyrpc {
class RpcException : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

class CalculatorService {
public:
  virtual ~CalculatorService() = default;

  virtual int64_t Add(int64_t a, int64_t b) = 0;

  virtual std::string Concat(const std::string &a, const std::string &b) = 0;
};

class CalculatorClient {
public:
  CalculatorClient(const std::string &host, int port);
  ~CalculatorClient();
  int64_t Add(int64_t a, int64_t b);
  std::string Concat(const std::string &a, const std::string &b);

private:
  struct Impl;
  std::unique_ptr<Impl> pimpl_;
};

class CalculatorServer {
public:
  CalculatorServer(CalculatorService &service_impl, int port);
  ~CalculatorServer();

  void serve();
  void stop();

private:
  struct Impl;
  std::unique_ptr<Impl> pimpl_;
};

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