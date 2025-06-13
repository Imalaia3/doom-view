CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -g

SRC_DIR := src
OBJ_DIR := build

SOURCES := $(shell find $(SRC_DIR) -name '*.cpp')
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

all: viewer

viewer: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o:
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $(patsubst $(OBJ_DIR)/%.o,$(SRC_DIR)/%.cpp,$@) -o $@

clean:
	rm -rf $(OBJ_DIR) viewer

.PHONY: all clean