CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
TARGET = region_based_grid_tree
SRCS = main.cpp VariableGridMap.cpp

all: build run

build:
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

run:
	./$(TARGET)

clean:
	rm -f $(TARGET)
