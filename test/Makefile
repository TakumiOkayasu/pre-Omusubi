# Omusubi テスト用 Makefile

CXX = clang++
CXXFLAGS = -std=c++14 -Wall -Wextra -pedantic -I../include
LDFLAGS =

# ディレクトリ
OBJ_DIR = obj

# テストファイル
TEST_SOURCES = test_span.cpp \
               test_string_view.cpp \
               test_fixed_string.cpp \
               test_fixed_buffer.cpp \
               test_vector3.cpp \
               test_all.cpp

# オブジェクトファイル（obj/ディレクトリ内）
TEST_OBJECTS = $(addprefix $(OBJ_DIR)/, $(TEST_SOURCES:.cpp=.o))

# 実行ファイル
TARGET = test_runner

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(TEST_OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)
