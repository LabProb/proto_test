#include <iostream>
#include <fstream>
#include "system.pb.h"

int main() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // --- 1. Створюємо об'єкт ---
    telemetry::Metrics metrics;
    metrics.set_cpu_usage(42.0f);
    metrics.set_memory_usage(65.0f);
    metrics.set_temperature(55.5f);
    metrics.set_mode("eco");

    // --- 2. Серіалізація в файл ---
    {
        std::ofstream out("build/metrics.bin", std::ios::binary);
        if (!metrics.SerializeToOstream(&out)) {
            std::cerr << "Serialize failed\n";
            return 1;
        }
    }

    std::cout << "Serialized to metrics.bin\n";

    // --- 3. Десеріалізація ---
    telemetry::Metrics loaded;
    {
        std::ifstream in("build/metrics.bin", std::ios::binary);
        if (!loaded.ParseFromIstream(&in)) {
            std::cerr << "Parse failed\n";
            return 1;
        }
    }

    // --- 4. Вивід ---
    std::cout << "CPU: " << loaded.cpu_usage() << "\n";
    std::cout << "MEM: " << loaded.memory_usage() << "\n";
    std::cout << "TEMP: " << loaded.temperature() << "\n";
    std::cout << "MODE: " << loaded.mode() << "\n";

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}