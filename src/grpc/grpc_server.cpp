#include <grpcpp/grpcpp.h>
#include "system.grpc.pb.h"

#include <iostream>
#include <memory>
#include <mutex>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

class MetricsServiceImpl final : public telemetry::MetricsService::Service {
public:
    MetricsServiceImpl()
        : mode_("eco") {}

    Status GetMetrics(ServerContext*,
                      const telemetry::Empty*,
                      telemetry::Metrics* response) override
    {
        std::lock_guard<std::mutex> lock(mutex_);

        response->set_cpu_usage(42.0);
        response->set_memory_usage(65.0);
        response->set_temperature(55.5);
        response->set_mode(mode_);

        return Status::OK;
    }

    Status SetMode(ServerContext*,
                   const telemetry::SetModeRequest* request,
                   telemetry::SetModeResponse* response) override
    {
        std::lock_guard<std::mutex> lock(mutex_);

        const std::string& newMode = request->mode();

        if (newMode != "eco" && newMode != "performance") {
            response->set_success(false);
            response->set_message("Invalid mode");
            return Status::OK;
        }

        mode_ = newMode;

        response->set_success(true);
        response->set_message("Mode updated");

        return Status::OK;
    }

private:
    std::string mode_;
    std::mutex mutex_;
};

int main() {
    std::string address("0.0.0.0:50051");

    MetricsServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server started on " << address << std::endl;

    server->Wait();
}