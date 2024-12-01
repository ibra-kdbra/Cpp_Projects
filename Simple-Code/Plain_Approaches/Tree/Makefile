# Define the source directories
SRC_DIR = src
ALGORITHMS_DIR = $(SRC_DIR)/algorithms

# Collect all .cpp files from the source directory
SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/TreeNode.cpp $(SRC_DIR)/GuiApp.cpp \
          $(wildcard $(ALGORITHMS_DIR)/*.cpp)

# MOC headers and sources
MOC_HEADERS = include/GuiApp.h
MOC_SOURCES = $(MOC_HEADERS:include/%.h=build/moc_%.cpp)

# Object files: .cpp to .o for both source and MOC sources
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=build/%.o) $(MOC_SOURCES:build/%.cpp=build/%.o)

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -g -Iinclude `pkg-config --cflags Qt5Widgets`
LDFLAGS = `pkg-config --libs Qt5Widgets`

# Target executable
TARGET = bin/binary_tree_gui

# Create build and bin directories if they don't exist
$(shell mkdir -p build bin build/algorithms)

# Linking the object files to create the final executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Rule for compiling .cpp to .o
build/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule for compiling MOC-generated .cpp to .o
build/%.o: build/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule for generating MOC .cpp files from header files
build/moc_%.cpp: include/%.h
	moc $< -o $@

# Clean rule to remove generated files
clean:
	rm -rf build bin
