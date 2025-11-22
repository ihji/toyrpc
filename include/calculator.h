#pragma once

#include <memory>
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
} // namespace toyrpc