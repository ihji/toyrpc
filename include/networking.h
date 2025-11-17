#include <folly/executors/CPUThreadPoolExecutor.h>
#include <folly/io/IOBuf.h>
#include <string>
#include <wangle/channel/Pipeline.h>
#include <wangle/service/ClientDispatcher.h>
#include <wangle/service/ExecutorFilter.h>
#include <wangle/service/ExpiringFilter.h>

using namespace folly;
using namespace wangle;

namespace toyrpc {

using ServerCallback =
    std::function<void(const folly::IOBuf &, folly::IOBuf &)>;

using ServerPipeline = Pipeline<IOBufQueue &, IOBuf>;

class ServerPipelineFactory : public PipelineFactory<ServerPipeline> {
public:
  ServerPipelineFactory(ServerCallback callback);
  ServerPipeline::Ptr
  newPipeline(std::shared_ptr<AsyncTransport> socket) override;

private:
  ExecutorFilter<std::unique_ptr<IOBuf>> service_;
};

class RpcService : public Service<std::unique_ptr<IOBuf>> {
public:
  RpcService(ServerCallback callback);

  Future<std::unique_ptr<IOBuf>>
  operator()(std::unique_ptr<IOBuf> request) override;

private:
  ServerCallback callback_;
};

struct Server {
private:
  int port_;
  ServerBootstrap<ServerPipeline> server_;

public:
  Server(int port, ServerCallback callback);
  void start();
  void stop();
};

using ClientPipeline = Pipeline<IOBufQueue &, std::unique_ptr<IOBuf>>;

class ClientPipelineFactory : public PipelineFactory<ClientPipeline> {
public:
  ClientPipeline::Ptr
  newPipeline(std::shared_ptr<AsyncTransport> socket) override;
};

struct Client {
private:
  std::string server_address_;
  int server_port_;
  std::shared_ptr<IOThreadPoolExecutor> io_group_;
  ClientBootstrap<ClientPipeline> client_;
  std::shared_ptr<ClientDispatcherBase<ClientPipeline, std::unique_ptr<IOBuf>>>
      dispatcher_;
  std::unique_ptr<ExpiringFilter<std::unique_ptr<IOBuf>>> service_;

public:
  Client(const std::string &address, int port);
  ~Client();
  void connect();
  Future<IOBuf> sendRequest(const folly::IOBuf &request);
};
} // namespace toyrpc