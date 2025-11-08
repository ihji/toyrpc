#include <folly/io/IOBuf.h>
#include <string>

namespace toyrpc {
using ServerCallback =
    std::function<void(const folly::IOBuf &, folly::IOBuf &)>;
struct Server {
private:
  std::string address_;
  int port_;
  int server_socket_;
  std::atomic<bool> stop_requested_;
  ServerCallback callback_;

public:
  Server(const std::string &address, int port);
  void start(ServerCallback callback);
  void handleRequest(int client_socket);
  void stop();
};

struct Client {
private:
  std::string server_address_;
  int server_port_;
  int sock_;

public:
  Client(const std::string &address, int port);
  ~Client();
  void connect();
  void sendRequest(const folly::IOBuf &request, folly::IOBuf &response);
};
} // namespace toyrpc