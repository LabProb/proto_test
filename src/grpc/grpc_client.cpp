#include <grpcpp/grpcpp.h>
#include "system.grpc.pb.h"

#include <iostream>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class MetricsClient {
public:
    MetricsClient(std::shared_ptr<Channel> channel)
        : stub_(telemetry::MetricsService::NewStub(channel)) {}

    void GetMetrics() {
        telemetry::Empty request;
        telemetry::Metrics response;
        ClientContext context;

        Status status = stub_->GetMetrics(&context, request, &response);

        if (status.ok()) {
            std::cout << "CPU: " << response.cpu_usage() << "%\n";
            std::cout << "MEM: " << response.memory_usage() << "%\n";
            std::cout << "TEMP: " << response.temperature() << "\n";
            std::cout << "MODE: " << response.mode() << "\n";
        } else {
            std::cout << "RPC failed\n";
        }
    }

    void SetMode(const std::string& mode) {
        telemetry::SetModeRequest request;
        request.set_mode(mode);

        telemetry::SetModeResponse response;
        ClientContext context;

        Status status = stub_->SetMode(&context, request, &response);

        if (status.ok()) {
            std::cout << "Success: " << response.success() << "\n";
            std::cout << "Msg: " << response.message() << "\n";
        }
    }

private:
    std::unique_ptr<telemetry::MetricsService::Stub> stub_;
};

int main() {
    MetricsClient client(
        grpc::CreateChannel("localhost:50051",
                            grpc::InsecureChannelCredentials()));

    client.GetMetrics();
    client.SetMode("performance");
    client.GetMetrics();
}