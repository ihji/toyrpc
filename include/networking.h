#include <string>
#include <vector>

namespace toyrpc {
using ServerCallback =
    std::function<void(const std::vector<uint8_t> &, std::vector<uint8_t> &)>;
struct Server {
private:
  std::string address_;
  int port_;
  int server_socket_;

public:
  Server(const std::string &address, int port);
  void start();
  void handleRequest(ServerCallback callback);
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
  void sendRequest(const std::vector<uint8_t> &request,
                   std::vector<uint8_t> &response);
};
} // namespace toyrpc