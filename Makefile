CC            = g++
COMMON_CFLAGS = -ggdb -std=c++17 -Wno-unknown-pragmas
CFLAGS        = $(COMMON_CFLAGS) -O0 -g -Wall -Wno-unused
PROD_CFLAGS   = $(COMMON_CFLAGS) -O3
DEBUG_DEFINES = -DDEBUG=1
PROD_DEFINES  = -DDEBUG=0
SRC           = main.cpp $(shell find ./src/ -name "*.cpp")
TEST_SRC      = test/test_main.cpp $(shell find ./src/ -name "*.cpp")
OUT_DIR       = build
BIN_NAME      = main
# LIBS         = -lm

.PHONY: help
help:
	@echo Supported targets:
	@cat $(MAKEFILE_LIST) | grep -e "^[\.a-zA-Z0-9_-]*: *.*## *" | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-35s\033[0m %s\n", $$1, $$2}'

.DEFAULT_GOAL := help

.PHONY: build
build: ## Build the project in the build folder. Creates ./build folder if it does not exist.
	mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $(DEBUG_DEFINES) -o $(OUT_DIR)/$(BIN_NAME) $(SRC)

.PHONY: build_prod
build_prod: clean ## Same as build, but optimizes aggresivly and generates no debug information.
	mkdir -p $(OUT_DIR)
	$(CC) $(PROD_CFLAGS) $(PROD_ENV) -o $(OUT_DIR)/$(BIN_NAME) $(SRC)

.PHONY: generate_asm
generate_asm: ## Uses objdump -S to generate asm from the binary. Expects the output binaries to be generated first by other make targets.
	objdump -M intel -S $(OUT_DIR)/$(BIN_NAME) > $(OUT_DIR)/$(BIN_NAME).S

.PHONY: test
test: clean ## Build test binaries and run tests.
	mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $(DEBUG_DEFINES) -o $(OUT_DIR)/$(BIN_NAME)_test $(TEST_SRC)
	$(OUT_DIR)/$(BIN_NAME)_test

.PHONY: clean
clean: ## Deletes the build folder.
	rm -rf $(OUT_DIR)