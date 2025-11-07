#include <calculator.h>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>

constexpr int TEST_PORT = 9091;

class MyTestCalculator : public toyrpc::CalculatorService {
public:
  int64_t Add(int64_t a, int64_t b) override {
    std::cout << "Server: Add(" << a << ", " << b << ") called." << std::endl;
    return a + b;
  };

  std::string Concat(const std::string &a, const std::string &b) override {
    std::cout << "Server: Concat(" << a << ", " << b << ") called."
              << std::endl;
    return a + b;
  };
};

class RpcTest : public ::testing::Test {
protected:
  std::unique_ptr<MyTestCalculator> service_impl_;
  std::unique_ptr<toyrpc::CalculatorServer> server_;
  std::thread server_thread_;

  void SetUp() override {
    std::cout << "Test SetUp: starting server..." << std::endl;
    service_impl_ = std::make_unique<MyTestCalculator>();

    server_ =
        std::make_unique<toyrpc::CalculatorServer>(*service_impl_, TEST_PORT);

    server_thread_ = std::thread([this]() {
      try {
        server_->serve();
        std::cout << "Test SetUp: server stopped." << std::endl;
      } catch (const std::exception &e) {
        std::cerr << "Server thread exception: " << e.what() << std::endl;
      }
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "Test SetUp: server is running" << std::endl;
  }

  void TearDown() override {
    std::cout << "Test TearDown: stopping server..." << std::endl;
    server_->stop();
    server_thread_.join();
    std::cout << "Test TearDown: server stopped." << std::endl;
  }
};

TEST_F(RpcTest, BasicAddAndConcat) {
  toyrpc::CalculatorClient client("127.0.0.1", TEST_PORT);

  int64_t add_result = client.Add(10, 32);
  std::string concat_result = client.Concat("hello ", "world");

  EXPECT_EQ(add_result, 42);
  EXPECT_EQ(concat_result, "hello world");
}

TEST_F(RpcTest, MultipleClient) {
  toyrpc::CalculatorClient client1("127.0.0.1", TEST_PORT);
  toyrpc::CalculatorClient client2("127.0.0.1", TEST_PORT);

  EXPECT_EQ(client1.Add(1, 2), 3);
  EXPECT_EQ(client2.Concat("a", "b"), "ab");
  EXPECT_EQ(client1.Add(100, 200), 300);
}