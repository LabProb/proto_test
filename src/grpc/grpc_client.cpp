#include <grpcpp/grpcpp.h>
#include "system.grpc.pb.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class MetricsClient {
public:
    MetricsClient(std::shared_ptr<Channel> channel)
        : stub_(telemetry::MetricsService::NewStub(channel)) {}

    bool GetMetrics() {
        telemetry::Empty request;
        telemetry::Metrics response;
        ClientContext context;

        const Status status = stub_->GetMetrics(&context, request, &response);

        if (status.ok()) {
            std::cout << "CPU: " << response.cpu_usage() << "%\n";
            std::cout << "MEM: " << response.memory_usage() << "%\n";
            std::cout << "TEMP: " << response.temperature() << "\n";
            std::cout << "MODE: " << response.mode() << "\n";
            return true;
        }

        std::cerr << "GetMetrics RPC failed (" << status.error_code()
                  << "): " << status.error_message() << '\n';
        return false;
    }

    bool SetMode(const std::string& mode) {
        telemetry::SetModeRequest request;
        request.set_mode(mode);

        telemetry::SetModeResponse response;
        ClientContext context;

        const Status status = stub_->SetMode(&context, request, &response);

        if (!status.ok()) {
            std::cerr << "SetMode RPC failed (" << status.error_code()
                      << "): " << status.error_message() << '\n';
            return false;
        }

        std::cout << "Success: " << response.success() << "\n";
        std::cout << "Msg: " << response.message() << "\n";
        return response.success();
    }

private:
    std::unique_ptr<telemetry::MetricsService::Stub> stub_;
};

int main() {
    MetricsClient client(
        grpc::CreateChannel("localhost:50051",
                            grpc::InsecureChannelCredentials()));

    const bool initial_metrics_received = client.GetMetrics();
    const bool mode_updated = client.SetMode("performance");
    const bool updated_metrics_received = client.GetMetrics();

    return initial_metrics_received && mode_updated && updated_metrics_received
               ? EXIT_SUCCESS
               : EXIT_FAILURE;
}
