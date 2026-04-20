SHELL := /bin/bash

.PHONY: clean config format build build_debug run 

MAKEFLAGS += --no-print-directory
NUM_CORES := $(shell nproc)
BUILD_TYPE ?= Release
MAKEFILE_DIR := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))

all: run

clean:
	@echo "#######################################################################"
	@echo "######################### Cleaning workspace ##########################"
	@echo "#######################################################################"
	@rm -rf build dist .Debug .Release cpp-ps1 pkg src/cpp-ps1 compile_commands.json *tar.zst

config: clean
	@echo "#######################################################################"
	@echo "######################## Configuring compiler #########################"
	@echo "#######################################################################"

	@cmake -B build -G Ninja \
		-DCMAKE_CXX_COMPILER=clang++ \
		-S . \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON 

	@if [ ! -f "compile_commands.json" ]; then \
		ln -s $(MAKEFILE_DIR)/build/compile_commands.json .; \
	fi

	@touch .$(BUILD_TYPE);
    
format:
	@clang-format -i $$(find include src -name '*.cpp' -o -name '*.hpp')
    
build:
	@if [ ! -f ".$(BUILD_TYPE)" ]; then \
		make config; \
	fi

	@echo "#######################################################################"
	@echo "####################### Compiling application #########################"
	@echo "#######################################################################"

	@$(MAKE) format
    
	@cmake --build build -- -j$(NUM_CORES)

build_debug:
	$(MAKE) build BUILD_TYPE=Debug

run: build_debug
	@echo "#######################################################################"
	@echo "######################## Running application ##########################"
	@echo "#######################################################################"
	@$(MAKEFILE_DIR)/build/cpp-ps1