# Simple Makefile wrapper for CMake out-of-source builds.
#
# Usage:
#   make           # configure + build (Debug by default)
#   make build     # just build
#   make configure # just run cmake -S . -B build
#   make clean     # clean via CMake
#   make install   # cmake --build build --target install
#
# You can override:
#   BUILD_DIR=...  # default: build
#   CONFIG=...     # default: Debug

BUILD_DIR ?= build
CONFIG    ?= Debug

CMAKE     ?= cmake

# Default target: configure and build
.PHONY: all
all: build

# Run CMake configuration step if needed
$(BUILD_DIR)/CMakeCache.txt:
	$(CMAKE) -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(CONFIG)

.PHONY: configure
configure: $(BUILD_DIR)/CMakeCache.txt

# Build using CMake
.PHONY: build
build: configure
	$(CMAKE) --build $(BUILD_DIR) --config $(CONFIG)

# Clean using CMake's generated clean target
.PHONY: clean
clean: configure
	$(CMAKE) --build $(BUILD_DIR) --target clean --config $(CONFIG)

# Optional: install
.PHONY: install
install: configure
	$(CMAKE) --build $(BUILD_DIR) --target install --config $(CONFIG)

