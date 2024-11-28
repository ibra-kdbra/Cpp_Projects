# Sources
SOURCES = src/main.cpp src/TreeNode.cpp src/DiameterBinaryTree.cpp src/EqualTreePartition.cpp src/FlattenBinaryTree.cpp src/InvertBinaryTree.cpp src/LcaBinaryTree.cpp

# Object files (placed in a build directory)
OBJECTS = $(SOURCES:src/%.cpp=build/%.o)
CXX = g++
CXXFLAGS = -std=c++11 -Wall -g -Iinclude


TARGET = bin/binary_tree_algorithms

$(shell mkdir -p build bin)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf build/ bin/
