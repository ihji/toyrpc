#include <arpa/inet.h>
#include <folly/io/IOBuf.h>
#include <iostream>
#include <netinet/in.h>
#include <networking.h>
#include <sys/endian.h>
#include <sys/socket.h>
#include <unistd.h>
#include <wangle/bootstrap/ClientBootstrap.h>
#include <wangle/channel/AsyncSocketHandler.h>
#include <wangle/channel/EventBaseHandler.h>
#include <wangle/codec/LengthFieldBasedFrameDecoder.h>
#include <wangle/codec/LengthFieldPrepender.h>
#include <wangle/service/ServerDispatcher.h>

namespace toyrpc {
RpcService::RpcService(ServerCallback callback)
    : callback_(std::move(callback)) {}

Future<std::unique_ptr<IOBuf>>
RpcService::operator()(std::unique_ptr<IOBuf> request) {
  folly::IOBuf response(folly::IOBuf::CREATE, 1024);
  callback_(*request.get(), response);
  return std::make_unique<IOBuf>(std::move(response));
}

ServerPipelineFactory::ServerPipelineFactory(ServerCallback callback)
    : service_{std::make_shared<folly::CPUThreadPoolExecutor>(4),
               std::make_shared<RpcService>(std::move(callback))} {}

ServerPipeline::Ptr
ServerPipelineFactory::newPipeline(std::shared_ptr<AsyncTransport> socket) {
  auto pipeline = ServerPipeline::create();
  pipeline->addBack(AsyncSocketHandler(socket));
  pipeline->addBack(EventBaseHandler());
  pipeline->addBack(LengthFieldBasedFrameDecoder());
  pipeline->addBack(LengthFieldPrepender());
  pipeline->addBack(MultiplexServerDispatcher(&service_));
  pipeline->finalize();
  return pipeline;
}

Server::Server(int port, ServerCallback callback) : port_(port), server_() {
  server_.childPipeline(
      std::make_shared<ServerPipelineFactory>(std::move(callback)));
  server_.bind(port);
  std::cout << "Networking Server: initialized at " << port_ << std::endl;
}

void Server::start() {
  std::cout << "Networking Server: starting at port " << port_ << std::endl;
  server_.waitForStop();
}

void Server::stop() {
  std::cout << "Networking Server: stopping at port " << port_ << std::endl;
  server_.stop();
  server_.join();
}

ClientPipeline::Ptr
ClientPipelineFactory::newPipeline(std::shared_ptr<AsyncTransport> socket) {
  auto pipeline = ClientPipeline::create();
  pipeline->addBack(AsyncSocketHandler(socket));
  pipeline->addBack(EventBaseHandler());
  pipeline->addBack(LengthFieldBasedFrameDecoder());
  pipeline->addBack(LengthFieldPrepender());
  pipeline->finalize();
  return pipeline;
}

Client::Client(const std::string &address, int port)
    : server_address_(address), server_port_(port),
      io_group_(std::make_shared<folly::IOThreadPoolExecutor>(1)),
      dispatcher_(std::make_shared<PipelinedClientDispatcher<
                      ClientPipeline, std::unique_ptr<IOBuf>>>()) {
  std::cout << "Networking Client: initialized for " << server_address_ << ":"
            << server_port_ << std::endl;
  client_.group(io_group_);
  client_.pipelineFactory(std::make_shared<ClientPipelineFactory>());
}
Client::~Client() { std::cout << "Networking Client: destroyed" << std::endl; }

void Client::connect() {
  auto pipeline =
      client_.connect(folly::SocketAddress(server_address_, server_port_))
          .get();
  dispatcher_->setPipeline(pipeline);
  service_ = std::make_unique<ExpiringFilter<std::unique_ptr<IOBuf>>>(
      dispatcher_, std::chrono::seconds(5));
  std::cout << "Networking Client: connected to " << server_address_ << ":"
            << server_port_ << std::endl;
}

Future<IOBuf> Client::sendRequest(const folly::IOBuf &request) {
  std::cout << "Networking Client: sending request of size " << request.length()
            << std::endl;
  return (*service_)(std::make_unique<IOBuf>(request))
      .thenValue([](std::unique_ptr<IOBuf> response) {
        std::cout << "Networking Client: received response of size "
                  << response->length() << std::endl;
        return *response;
      });
}
} // namespace toyrpc
