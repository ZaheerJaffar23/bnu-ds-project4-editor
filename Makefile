CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -Iinclude

SRC_DIR  := src
OBJ_DIR  := obj
BIN_DIR  := bin

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

SAMPLE := data/sample/manuscript.txt
FULL   := data/full/manuscript.txt

.PHONY: all clean run run-full manuscript demo demo-full

all: $(BIN_DIR)/editor

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR)/editor: $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN_DIR)/generate_manuscript: tools/generate_manuscript.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

manuscript: $(BIN_DIR)/generate_manuscript
	mkdir -p data
	$(BIN_DIR)/generate_manuscript data/manuscript.txt

run: all
	$(BIN_DIR)/editor $(SAMPLE)

run-full: all
	$(BIN_DIR)/editor $(FULL)

demo: all
	$(BIN_DIR)/editor --demo $(SAMPLE)

demo-full: all
	$(BIN_DIR)/editor --demo $(FULL)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
