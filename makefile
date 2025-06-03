PROJECT = mylab

CXX = g++
A = ar

CXXFLAGS = -Iinclude -std=c++17 -Wall -g -fPIC -fopenmp

SRC_DIR = src
OBJ_DIR = obj

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Create object directory if it doesn't exist
$(shell mkdir -p $(OBJ_DIR))

.PHONY: all clean cleanall

all: $(PROJECT)

# Link the final executable
$(PROJECT): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS)

# Compile .cpp files into .o object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f output_clockwise.bmp output_counterclockwise.bmp output_filtered.bmp output_filtered_sequential.bmp

cleanall: clean
	rm -f $(PROJECT)
	rm -rf $(OBJ_DIR)