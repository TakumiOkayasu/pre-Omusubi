# Compiler and flags
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
TEST_DIR = test
EXAMPLE_DIR = examples
BIN_DIR = bin

# Target executable
TARGET = main

# Source files
MAIN_SRC = main.cpp
LIB_SRCS = $(shell find $(SRC_DIR) -type f -name '*.cpp')

# Header files
HEADERS = $(shell find $(INC_DIR) -type f \( -name '*.h' -o -name '*.hpp' \))

# Object files (maintain directory structure in obj/)
LIB_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(LIB_SRCS))

# Default target
all: $(TARGET)

# Link main.cpp with library object files to create executable
$(TARGET): $(MAIN_SRC) $(LIB_OBJS) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(MAIN_SRC) $(LIB_OBJS)

# Compile library source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Rebuild everything
rebuild: clean all

# Run the program
run: $(TARGET)
	./$(TARGET)

# Test targets
# Standalone tests (have main() in global scope)
STANDALONE_TESTS = test_auto_capacity test_format test_format_string
STANDALONE_TEST_BINS = $(patsubst %,$(BIN_DIR)/%,$(STANDALONE_TESTS))

# test_all.cpp requires linking multiple test files (not included in standalone builds)
# test_span, test_string_view, test_fixed_buffer, test_fixed_string, test_vector3
# have main() in namespace scope and are designed to be linked with test_all.cpp

# Example targets
# Only build examples with standard suffixes (_demo, _example)
EXAMPLE_DEMO_SRCS = $(wildcard $(EXAMPLE_DIR)/*_demo.cpp)
EXAMPLE_EXAMPLE_SRCS = $(wildcard $(EXAMPLE_DIR)/*_example.cpp)
EXAMPLE_BINS = $(patsubst $(EXAMPLE_DIR)/%.cpp,$(BIN_DIR)/%,$(EXAMPLE_DEMO_SRCS) $(EXAMPLE_EXAMPLE_SRCS))

# Build all standalone tests
tests: $(STANDALONE_TEST_BINS)

# Build all examples
examples: $(EXAMPLE_BINS)

# Build individual test
$(BIN_DIR)/test_%: $(TEST_DIR)/test_%.cpp $(HEADERS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $<

# Build individual example
$(BIN_DIR)/%_demo: $(EXAMPLE_DIR)/%_demo.cpp $(HEADERS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $<

$(BIN_DIR)/%_example: $(EXAMPLE_DIR)/%_example.cpp $(HEADERS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $<

# Run all tests
test: tests
	@for test in $(STANDALONE_TEST_BINS); do \
		echo "Running $$test..."; \
		$$test || exit 1; \
	done

# Clean test and example binaries
clean-tests:
	rm -rf $(BIN_DIR)

# Clean everything including tests
clean-all: clean clean-tests

.PHONY: all clean rebuild run tests examples test clean-tests clean-all
