SOURCES = src/main.cpp src/TreeNode.cpp src/GuiApp.cpp src/DiameterBinaryTree.cpp \
          src/EqualTreePartition.cpp src/FlattenBinaryTree.cpp src/InvertBinaryTree.cpp \
          src/LcaBinaryTree.cpp

MOC_HEADERS = include/GuiApp.h

MOC_SOURCES = $(MOC_HEADERS:include/%.h=build/moc_%.cpp)

OBJECTS = $(SOURCES:src/%.cpp=build/%.o) $(MOC_SOURCES:build/%.cpp=build/%.o)

CXX = g++
CXXFLAGS = -std=c++11 -Wall -g -Iinclude `pkg-config --cflags Qt5Widgets`
LDFLAGS = `pkg-config --libs Qt5Widgets`

TARGET = bin/binary_tree_gui

$(shell mkdir -p build bin)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/%.o: build/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/moc_%.cpp: include/%.h
	moc $< -o $@

clean:
	rm -rf build bin

