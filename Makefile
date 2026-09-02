.PHONY: all configure build run run-server run-client clean rebuild

BUILD_DIR=build
SERVER_TARGET=system_server
CLIENT_TARGET=system_client

all: build

configure:
	cmake -S . -B $(BUILD_DIR)

build: configure
	cmake --build $(BUILD_DIR)

run: run-server

run-server: build
	./$(BUILD_DIR)/$(SERVER_TARGET)

run-client: build
	./$(BUILD_DIR)/$(CLIENT_TARGET)

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean all
