#include <calculator.h>
#include <iostream>
#include <networking.h>
#include <thread>

namespace toyrpc {
struct CalculatorClient::Impl {
  std::string host;
  int port;
  Client client;

  Impl(const std::string &h, int p) : host(h), port(p), client(h, p) {
    std::cout << "Client: connecting to " << host << ":" << port << std::endl;
    client.connect();
  }

  ~Impl() { std::cout << "Client: disconnecting" << std::endl; }

  int64_t Add(int64_t a, int64_t b) {
    std::cout << "Client: call Add(" << a << ", " << b << ")" << std::endl;
    std::vector<uint8_t> request = Add_Request{a, b}.serialize();
    std::vector<uint8_t> response;
    client.sendRequest(request, response);
    return Add_Response::deserialize(response).result;
  }

  std::string Concat(const std::string &a, const std::string &b) {
    std::cout << "Client: call Concat(" << a << ", " << b << ")" << std::endl;
    std::vector<uint8_t> request = Concat_Request{a, b}.serialize();
    std::vector<uint8_t> response;
    client.sendRequest(request, response);
    return Concat_Response::deserialize(response).result;
  }
};

CalculatorClient::CalculatorClient(const std::string &host, int p)
    : pimpl_(std::make_unique<Impl>(host, p)) {};

CalculatorClient::~CalculatorClient() = default;

int64_t CalculatorClient::Add(int64_t a, int64_t b) {
  return pimpl_->Add(a, b);
}

std::string CalculatorClient::Concat(const std::string &a,
                                     const std::string &b) {
  return pimpl_->Concat(a, b);
}

struct CalculatorServer::Impl {
  CalculatorService &service;
  int port;
  toyrpc::Server server;

  std::atomic<bool> stop_requested_{false};

  Impl(CalculatorService &s, int p)
      : service(s), port(p), server("127.0.0.1", port) {
    std::cout << "Server: Init port " << port << std::endl;
  };

  void serve() {
    std::cout << "Server: Serving on port " << port << std::endl;
    std::cout << "Server: Waiting for requests..." << std::endl;

    server.start();

    auto callback = [this](const std::vector<uint8_t> &request,
                           std::vector<uint8_t> &response) {
      std::cout << "Server: Received request of size " << request.size()
                << std::endl;
      uint8_t method_id = request[0];
      if (method_id == ADD_METHOD_ID) {
        Add_Request add_req = Add_Request::deserialize(request);
        int64_t result = service.Add(add_req.a, add_req.b);
        Add_Response add_resp{result};
        response = add_resp.serialize();
        std::cout << "Server: Processed Add request. Result: " << result
                  << std::endl;
      } else if (method_id == CONCAT_METHOD_ID) {
        Concat_Request concat_req = Concat_Request::deserialize(request);
        std::string result = service.Concat(concat_req.a, concat_req.b);
        Concat_Response concat_resp{result};
        response = concat_resp.serialize();
        std::cout << "Server: Processed Concat request. Result: " << result
                  << std::endl;
      } else {
        std::cerr << "Server: Unknown method ID: " << method_id << std::endl;
        response.push_back(0); // Indicate error
      }
    };

    while (!stop_requested_.load()) {
      server.handleRequest(callback);
    }

    std::cout << "Server: Stopped serving." << std::endl;
  }

  void stop() {
    std::cout << "Server: Stop requested." << std::endl;
    stop_requested_.store(true);
    server.stop();
  }
};

CalculatorServer::CalculatorServer(CalculatorService &service_impl, int p)
    : pimpl_(std::make_unique<Impl>(service_impl, p)) {};

CalculatorServer::~CalculatorServer() = default;

void CalculatorServer::serve() { pimpl_->serve(); }
void CalculatorServer::stop() { pimpl_->stop(); }

} // namespace toyrpc
