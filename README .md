# ERP System - Student Management
## OOPD Assignment 4: Templates and Threads

---

## Project Overview

This is a comprehensive ERP (Enterprise Resource Planning) system for university student management, demonstrating advanced C++ concepts including:

- **Templates** for flexible data types
- **Multi-threaded Programming** for parallel processing
- **Data Abstraction and Hiding** throughout the system
- **Custom Iterators** for different traversal patterns
- **Search Optimization** for fast data retrieval
- **CSV File Handling** for bulk data import

The system is designed to handle students from different universities with different data type conventions (string vs. integer roll numbers, different course coding systems).

---

## Assignment Requirements Fulfillment

### Requirement 1: Flexible Template-Based Student Class ✓
- **File**: `Student.h`
- **Implementation**: 
  - Template class `Student<RollType, CourseType>` handles variable field types
  - Works with any roll number type (string, unsigned int, custom types)
  - Works with any course type (IIITCourse, IITCourse, or custom)
  - Encapsulation using private members and public getters/setters
  - Data hiding ensures proper access control

### Requirement 2: IIIT-Delhi & IIT-Delhi Course Systems ✓
- **File**: `main.cpp`, `Student.h`
- **Implementation**:
  - IIIT-Delhi System: String roll numbers, string course codes with semester and grade
  - IIT-Delhi System: Unsigned int roll numbers, integer course codes with grades
  - Type aliases: `IIITStudent`, `IIITStudentManager`, `IITStudent`, `IITStudentManager`
  - Both systems work in parallel without interference
  - Automatic type handling through templates

### Requirement 3: Parallel Sorting with Threads ✓
- **File**: `SortingThreads.h`, `main.cpp`
- **Implementation**:
  - Multi-threaded merge sort algorithm
  - Minimum 2 threads supported (configurable)
  - Uses `std::thread` for parallel execution
  - `std::mutex` prevents race conditions
  - Logs timing for each thread in `sorting_thread_log.txt`
  - Time complexity: O(n log n)
  - Space complexity: O(n)

### Requirement 4: Multiple Iterator Types ✓
- **File**: `Iterator.h`, `StudentManager.h`
- **Implementation**:
  - `InsertionOrderIterator`: Forward iterator for insertion order traversal
  - `SortedOrderIterator`: Random access iterator for sorted order traversal
  - Without data copying using index mapping
  - Compliant with STL iterator standards
  - Support for standard iterator operations (++, --, [], etc.)

### Requirement 5: Fast High-Grade Student Search ✓
- **File**: `SearchIndex.h`
- **Implementation**:
  - `SearchIndex` template for course-based indexing
  - O(k) lookup where k = number of high-grade students in course
  - Avoids linear search through all students
  - Multi-map based efficient data structure
  - Used in placement company queries (grade >= 9)

---

## Project Structure

```
.
├── main.cpp                    # Main program with menu-driven interface
├── Student.h                   # Template-based Student class
├── StudentManager.h            # Student collection management
├── Iterator.h                  # Custom iterator implementations
├── SortingThreads.h            # Multi-threaded sorting manager
├── SearchIndex.h               # Fast search index for grades
├── Makefile                    # Build configuration
├── README.md                   # This file
├── students_sample_3000.csv    # Sample data (3000 students)
└── sorting_thread_log.txt      # Generated sorting logs
```

---

## Building the Project

### Prerequisites
- GCC or Clang C++ compiler (C++17 or later)
- Standard Unix utilities (make, mkdir, etc.)
- pthread library (for threading)

### Build Instructions

#### Option 1: Using Make (Recommended)
```bash
# Build the executable
make

# Build and run immediately
make run

# Clean build artifacts
make clean
```

#### Option 2: Manual Compilation
```bash
# Compile with C++17 standard and threading support
g++ -std=c++17 -Wall -Wextra -O2 -g -pthread -o erp_system main.cpp

# Run the program
./erp_system
```

### Compilation Flags Explanation
- `-std=c++17`: Enable C++17 standard features (templates, auto)
- `-Wall -Wextra`: Show all compiler warnings
- `-O2`: Optimization level 2 for better performance
- `-g`: Include debugging symbols
- `-pthread`: Link pthread library for multi-threading

---

## Running the Application

### Starting the Program
```bash
./erp_system
```

### Main Menu Options

```
╔════════════════════════════════════════════════════════════╗
║           ERP System - Student Management                  ║
║           Assignment 4: Templates and Threads              ║
╚════════════════════════════════════════════════════════════╝

1. Load Students from CSV (3000 students)
2. IIIT-Delhi System (String Roll Numbers)
3. IIT-Delhi System (Integer Roll Numbers)
4. Perform Parallel Sorting (Multi-threaded)
5. Search High Grade Students (Grade >= 9)
6. Display Statistics
7. Exit
```

### Usage Walkthrough

#### Step 1: Load Sample Data
- Select option **1** from main menu
- When prompted, press Enter to load the default `students_sample_3000.csv`
- Wait for confirmation message

#### Step 2: Explore IIIT-Delhi System
- Select option **2** to access IIIT-Delhi system
- Submenu options:
  - **1**: Add a new student manually
  - **2**: Display all students (choose insertion or sorted order)
  - **3**: Return to main menu

#### Step 3: Explore IIT-Delhi System
- Select option **3** to access IIT-Delhi system
- Submenu options similar to IIIT system
- Demonstrates different data types (integer roll numbers)

#### Step 4: Perform Parallel Sorting
- Select option **4** from main menu
- Enter number of threads (minimum 2, recommended 4-8)
- System will sort all loaded students using specified threads
- Thread timing statistics displayed on screen
- Detailed logs written to `sorting_thread_log.txt`

#### Step 5: Search High-Grade Students
- Select option **5** from main menu
- Enter minimum grade points (1-10, where 10=A+)
- System returns students with at least that grade
- Uses optimized search (not linear search)
- Shows first 10 results and count of remaining

#### Step 6: View Statistics
- Select option **6** to see system statistics
- Displays total student count in each system

### Sample CSV Format

The `students_sample_3000.csv` file uses the following format:

```
RollNumber,Name,Branch,StartYear,IIITCourses(Code:Sem:Grade),IITCourses(Code:Grade)
MT23001,Student_1,CSE,2023,OOP:9:B+;DSA:5:A,101:A+
1513,Student_2,ECE,2021,ML:7:A-;OOP:9:B+,
```

**Column Descriptions:**
- `RollNumber`: Can be string (MT23001) or numeric (1513)
- `Name`: Student name
- `Branch`: Department (CSE, ECE, MTech-CSE, MTech-AI)
- `StartYear`: Year of enrollment (2020-2024)
- `IIITCourses`: IIIT-Delhi courses (format: Code:Semester:Grade)
  - Courses separated by semicolon
  - Grades: A(10pts), B(9pts), C(8pts), D(7pts)
- `IITCourses`: IIT-Delhi courses (format: CourseCode:Grade)
  - Integer course codes
  - Grades: A(10pts), B(9pts), C(8pts), D(7pts)

---

## Key Features Demonstrated

### 1. Template Programming
```cpp
template <typename RollType, typename CourseType>
class Student { ... }
```
- Flexible for different university systems
- Compile-time type safety
- No runtime overhead

### 2. Multi-threading
```cpp
threads.emplace_back(&SortingThreadsManager::mergeSort<T>, 
                     this, std::ref(data), start, end, i, data.size());
```
- Parallel merge sort algorithm
- Mutex-based synchronization
- Thread-safe statistics collection

### 3. Custom Iterators
```cpp
InsertionOrderIterator<Student<RollType, CourseType>> iter = 
    manager.getInsertionOrderIterator();
```
- Forward and random-access iterators
- Non-destructive traversal
- Support for standard algorithms

### 4. Data Abstraction
- Private data members
- Public accessor methods
- Encapsulated algorithms
- Clear interface definitions

### 5. CSV Processing
- Efficient file I/O
- Robust parsing with error handling
- Support for flexible data formats
- Bulk data loading capability

---

## Performance Characteristics

### Sorting Performance
- **Algorithm**: Multi-threaded Merge Sort
- **Time Complexity**: O(n log n)
- **Space Complexity**: O(n)
- **Threading Overhead**: Minimal with proper thread count selection
- **Recommended Threads**: 2-4 for datasets up to 3000 records

### Search Performance
- **Algorithm**: Index-based lookup
- **Time Complexity**: O(k) where k = matching records
- **vs Linear Search**: O(n) → O(k) improvement
- **Index Building**: O(n log n) once during initialization

### Memory Usage
- **Student Object**: ~200-500 bytes each
- **3000 Students**: ~1-2 MB
- **Total System**: ~5-10 MB

---

## Code Quality Metrics

- **Template Safety**: Compile-time type checking
- **Thread Safety**: Mutex-protected critical sections
- **Error Handling**: Exception handling and validation
- **Memory Management**: RAII principles, no memory leaks
- **Code Organization**: Header-based for template instantiation
- **Compilation Warnings**: None with -Wall -Wextra

---

## Testing Guide

### Test Case 1: Load and Display
1. Run program
2. Load CSV (option 1)
3. Display IIIT students in both orders (option 2 → 2)
4. Display IIT students in both orders (option 3 → 2)
5. Verify data consistency across systems

### Test Case 2: Parallel Sorting
1. Load CSV
2. Run parallel sort with 2 threads
3. Check sorting_thread_log.txt for timing
4. Run with 4 threads and compare performance
5. Verify sorted order correctness

### Test Case 3: Search Functionality
1. Load CSV
2. Search for grade >= 9
3. Verify results are correct
4. Compare search speed with manual verification

### Test Case 4: Manual Addition
1. Add student to IIIT system
2. Add student to IIT system
3. Display both systems
4. Verify data types are handled correctly

---

## Known Limitations & Future Improvements

### Current Limitations
- CSV parser expects specific format
- Maximum 3000 students demonstrated
- No persistent database storage

### Future Enhancements
- Database backend (SQLite/PostgreSQL)
- More sophisticated query language
- RESTful API for remote access
- GUI interface
- Advanced analytics and reporting

---

## Troubleshooting

### Issue: Compilation Error "cannot find -lpthread"
**Solution**: Ensure -pthread flag is used during compilation
```bash
make clean
make
```

### Issue: Segmentation Fault on Startup
**Solution**: Ensure all header files are in the same directory as main.cpp

### Issue: CSV File Not Found
**Solution**: Verify students_sample_3000.csv is in the same directory as the executable
```bash
ls -la students_sample_3000.csv
./erp_system
```

### Issue: Threads Not Running in Parallel
**Solution**: Ensure you have multiple CPU cores. Check system:
```bash
nproc  # Shows number of processors
```

---

## Author Notes

This assignment demonstrates:
1. Advanced C++ template programming for flexible data type handling
2. Multi-threaded programming with proper synchronization
3. Custom iterator implementation following STL standards
4. Efficient data structures for fast searches
5. Professional code organization and documentation

The modular design allows easy extension for additional university systems and course types.

---

## Submission Checklist

- ✓ C++ source code and headers
- ✓ Makefile with compilation instructions
- ✓ README with project description
- ✓ Sample CSV data (3000 students)
- ✓ No binary or library files
- ✓ Private GitHub repository
- ✓ All assignment requirements met
- ✓ Menu-driven interface for user input
- ✓ Thread safety and race condition prevention
- ✓ Multiple iterator types
- ✓ Fast search implementation

---

## Contact & Support

For issues or questions regarding this implementation, please refer to the inline code documentation and function comments throughout the header files.

Last Updated: 2025-11-02
Assignment: OOPD Assignment 4 - Templates and Threads
