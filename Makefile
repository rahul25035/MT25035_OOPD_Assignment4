CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2 -g -pthread
TARGET = erp_system
SOURCES = main.cpp
HEADERS = Student.h StudentDatabase.h AuthManager.h
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
