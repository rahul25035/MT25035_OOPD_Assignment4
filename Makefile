# Makefile for OOPD Assignment 4 - Templates and Threads
# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2 -g -fsanitize=address
LDFLAGS = -pthread

# Target executable
TARGET = erp_system

# Source files
SOURCES = main.cpp
HEADERS = Student.h StudentDatabase.h ParallelSort.h CSVReader.h
OBJECTS = $(SOURCES:.cpp=.o)

# Data file
DATA_FILE = students.csv

# Default target
all: $(TARGET) $(DATA_FILE)

# Build executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^
	@echo "\n=== Build Successful ==="
	@echo "Executable: $(TARGET)"
	@echo "Run with: ./$(TARGET)"

# Compile source files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Generate CSV data file
$(DATA_FILE):
	@echo "Generating CSV data file with 3000 student records..."
	@python3 -c "\
import csv; \
import random; \
first_names = ['Rahul', 'Priya', 'Arjun', 'Neha', 'Vikram', 'Anjali', 'Rohan', 'Divya', 'Aditya', 'Pooja']; \
last_names = ['Kumar', 'Singh', 'Sharma', 'Patel', 'Gupta', 'Verma', 'Reddy', 'Nair', 'Jain', 'Das']; \
branches = ['CSE', 'ECE', 'MNIT', 'MTech']; \
iiit_courses = [('COL100', 'Introduction to Computer Science'), ('COL106', 'Data Structures'), ('COL216', 'OOPD'), ('COL331', 'Algorithms'), ('COL106', 'Advanced Algorithms')]; \
iit_courses = [(101, 'Linear Algebra'), (202, 'Discrete Mathematics'), (303, 'Probability and Statistics'), (404, 'Physics I'), (505, 'Chemistry')]; \
data = []; \
for i in range(3000): \
    roll = f'2025{i+1:04d}'; \
    name = f'{random.choice(first_names)} {random.choice(last_names)}'; \
    branch = random.choice(branches); \
    year = random.randint(2023, 2025); \
    current = ';'.join([f'{code}:{n}' for code, n in random.sample(iiit_courses, random.randint(2, 3))]); \
    prev = ';'.join([f'{code}:Grade-{random.randint(5, 10)}' for code, _ in random.sample(iiit_courses, random.randint(1, 3))]); \
    data.append([roll, name, branch, year, current, prev]); \
with open('students.csv', 'w', newline='') as f: \
    writer = csv.writer(f); \
    writer.writerow(['RollNumber', 'Name', 'Branch', 'StartingYear', 'CurrentCourses', 'PreviousCourses']); \
    writer.writerows(data); \
print('Generated students.csv with 3000 records')"

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "Cleaned build artifacts"

# Full clean (including data)
distclean: clean
	rm -f $(DATA_FILE)
	@echo "Cleaned all generated files"

# Run the program
run: $(TARGET)
	./$(TARGET)

# Debug mode
debug: CXXFLAGS = -Wall -Wextra -std=c++17 -g -fsanitize=address
debug: clean $(TARGET)
	@echo "Debug build complete"

# Help
help:
	@echo "Available targets:"
	@echo "  all       - Build the executable (default)"
	@echo "  run       - Build and run the program"
	@echo "  clean     - Remove object files and executable"
	@echo "  distclean - Remove all generated files"
	@echo "  debug     - Build with debug flags"
	@echo "  help      - Show this help message"

.PHONY: all clean distclean run debug help
