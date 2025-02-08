CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude
LDFLAGS = -lSDL3
SRC = $(wildcard src/*.cpp)
HEADERS = $(wildcard include/*.h)
SIM_SRC = $(wildcard src/sim/*.cpp)
OBJ = $(patsubst src/%.cpp, $(TARGET_DIR)/%.o, $(SRC))
SIM_OBJ = $(patsubst src/sim/%.cpp, $(TARGET_DIR)/%.o, $(SIM_SRC))
DEP = $(OBJ:.o=.d) $(SIM_OBJ:.o=.d)
TARGET_DIR = bin
TARGETS = $(patsubst src/sim/%.cpp, $(TARGET_DIR)/%, $(SIM_SRC))

all: check_dir $(TARGETS)

check_dir:
	@mkdir -p $(TARGET_DIR)

$(TARGET_DIR)/%: $(TARGET_DIR)/%.o $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(TARGET_DIR)/%.o: src/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

$(TARGET_DIR)/%.o: src/sim/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

-include $(DEP)

clean:
	rm -rf $(TARGET_DIR)

.PHONY: all clean check_dir
