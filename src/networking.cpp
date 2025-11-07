#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <networking.h>
#include <sys/socket.h>
#include <unistd.h>

namespace toyrpc {
Server::Server(const std::string &address, int port)
    : address_(address), port_(port) {
  std::cout << "Networking Server: initialized at " << address_ << ":" << port_
            << std::endl;
}

void Server::start() {
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
}

void Server::handleRequest(ServerCallback callback) {
  sockaddr_in client_addr{};
  socklen_t client_len = sizeof(client_addr);
  int client_socket =
      accept(server_socket_, (sockaddr *)&client_addr, &client_len);
  if (client_socket < 0) {
    throw std::runtime_error("Failed to accept connection");
  }

  std::vector<uint8_t> request(1024);
  ssize_t bytes_received =
      recv(client_socket, request.data(), request.size(), 0);
  if (bytes_received < 0) {
    close(client_socket);
    throw std::runtime_error("Failed to receive data");
  }
  request.resize(bytes_received);

  std::vector<uint8_t> response;
  callback(request, response);

  ssize_t bytes_sent = send(client_socket, response.data(), response.size(), 0);
  if (bytes_sent < 0) {
    close(client_socket);
    throw std::runtime_error("Failed to send data");
  }

  close(client_socket);
}

void Server::stop() {
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

void Client::sendRequest(const std::vector<uint8_t> &request,
                         std::vector<uint8_t> &response) {
  if (sock_ < 0) {
    throw std::runtime_error("Socket is not connected");
  }
  ssize_t bytes_sent = send(sock_, request.data(), request.size(), 0);
  if (bytes_sent < 0) {
    close(sock_);
    throw std::runtime_error("Failed to send data");
  }

  response.resize(1024);
  ssize_t bytes_received = recv(sock_, response.data(), response.size(), 0);
  if (bytes_received < 0) {
    close(sock_);
    throw std::runtime_error("Failed to receive data");
  }
  response.resize(bytes_received);
}
} // namespace toyrpc
