#include <arpa/inet.h>
#include <folly/io/IOBuf.h>
#include <iostream>
#include <netinet/in.h>
#include <networking.h>
#include <sys/socket.h>
#include <unistd.h>

namespace toyrpc {
Server::Server(const std::string &address, int port)
    : address_(address), port_(port), stop_requested_(false) {
  std::cout << "Networking Server: initialized at " << address_ << ":" << port_
            << std::endl;
}

void Server::start(ServerCallback callback) {
  callback_ = callback;

  server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket_ < 0) {
    throw std::runtime_error("Failed to create socket");
  }

  int opt = 1;
  if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) <
      0) {
    throw std::runtime_error("Failed to set socket options");
  }

  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port_);
  if (inet_pton(AF_INET, address_.c_str(), &server_addr.sin_addr) <= 0) {
    throw std::runtime_error("Invalid address");
  }

  if (bind(server_socket_, (sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    throw std::runtime_error("Failed to bind socket");
  }

  if (listen(server_socket_, SOMAXCONN) < 0) {
    throw std::runtime_error("Failed to listen on socket");
  }

  std::cout << "Networking Server: started at " << address_ << ":" << port_
            << std::endl;
  while (!stop_requested_.load()) {
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    int client_socket =
        accept(server_socket_, (sockaddr *)&client_addr, &client_len);
    if (client_socket < 0) {
      throw std::runtime_error("Failed to accept connection");
    }
    std::thread([this, client_socket]() {
      try {
        handleRequest(client_socket);
      } catch (const std::exception &e) {
        std::cerr << "Error handling request: " << e.what() << std::endl;
      }
      close(client_socket);
    }).detach();
  }
}

void Server::handleRequest(int client_socket) {
  while (!stop_requested_.load()) {
    folly::IOBuf request(folly::IOBuf::CREATE, 1024);
    size_t bytes_received = recv(
        client_socket, (void *)(request.writableData()), request.capacity(), 0);
    if (bytes_received < 0) {
      close(client_socket);
      throw std::runtime_error("Failed to receive data");
    }
    std::cout << "Networking Server: received request of size "
              << bytes_received << std::endl;
    if (bytes_received == 0) {
      close(client_socket);
      return; // Connection closed by client
    }
    request.append(bytes_received);

    folly::IOBuf response(folly::IOBuf::CREATE, 1024);
    callback_(request, response);

    std::cout << "Networking Server: sending response of size "
              << response.length() << std::endl;
    ssize_t bytes_sent =
        send(client_socket, response.data(), response.length(), 0);
    if (bytes_sent < 0) {
      close(client_socket);
      throw std::runtime_error("Failed to send data");
    }
  }
}

void Server::stop() {
  stop_requested_.store(true);
  close(server_socket_);
  std::cout << "Networking Server: stopped at " << address_ << ":" << port_
            << std::endl;
}

Client::Client(const std::string &address, int port)
    : server_address_(address), server_port_(port) {
  std::cout << "Networking Client: initialized for " << server_address_ << ":"
            << server_port_ << std::endl;
}

Client::~Client() {
  if (sock_ >= 0) {
    close(sock_);
  }
}

void Client::connect() {
  sock_ = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_ < 0) {
    throw std::runtime_error("Failed to create socket");
  }

  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(server_port_);
  if (inet_pton(AF_INET, server_address_.c_str(), &server_addr.sin_addr) <= 0) {
    close(sock_);
    throw std::runtime_error("Invalid address");
  }

  if (::connect(sock_, (sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    close(sock_);
    throw std::runtime_error("Connection failed");
  }
}

void Client::sendRequest(const folly::IOBuf &request, folly::IOBuf &response) {
  if (sock_ < 0) {
    throw std::runtime_error("Socket is not connected");
  }
  ssize_t bytes_sent = send(sock_, request.data(), request.length(), 0);
  if (bytes_sent < 0) {
    close(sock_);
    throw std::runtime_error("Failed to send data");
  }
  ssize_t bytes_received =
      recv(sock_, (void *)response.writableData(), response.capacity(), 0);
  if (bytes_received < 0) {
    close(sock_);
    throw std::runtime_error("Failed to receive data");
  }
  response.append(bytes_received);
  std::cout << "Networking Client: received response of size "
            << response.length() << std::endl;
}
} // namespace toyrpc
