CXX = g++
CXXFLAGS = -std=c++17 -Wall -g
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system
TARGET = A*wheels
SRC = A*_wheels.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(SFML_LIBS)

# Clean target
clean:
	rm -f $(TARGET)

# Run target
run: all
	./$(TARGET)
