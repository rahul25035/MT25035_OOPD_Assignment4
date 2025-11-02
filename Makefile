# Makefile for OOPD Assignment 4 - Templates and Threads

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g -pthread
TARGET = erp_system
SOURCES = main.cpp
HEADERS = Student.h StudentManager.h Iterator.h SortingThreads.h SearchIndex.h
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

help:
	@echo "Available targets:"
	@echo "  make        - Build the executable"
	@echo "  make run    - Build and run the executable"
	@echo "  make clean  - Remove build artifacts"
	@echo "  make help   - Display this help message"

.PHONY: all clean run help
