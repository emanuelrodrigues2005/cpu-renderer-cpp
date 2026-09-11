BUILD_DIR ?= build
BIN       := $(BUILD_DIR)/cpu-renderer
CMAKE     ?= cmake
ARGS      ?=

.PHONY: all dev configure build run smoke test clean

all: build

configure:
	$(CMAKE) -S . -B $(BUILD_DIR)

build: configure
	$(CMAKE) --build $(BUILD_DIR)

dev: build
	./$(BIN) $(ARGS)

run: build
	./$(BIN) $(ARGS)

smoke: build
	SDL_VIDEODRIVER=dummy ./$(BIN) --smoke

test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

clean:
	rm -rf $(BUILD_DIR)
