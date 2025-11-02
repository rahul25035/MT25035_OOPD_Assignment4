# University ERP System - Assignment 4

## Overview

This is a comprehensive Object-Oriented Programming and Design (OOPD) assignment implementation that creates a University ERP (Enterprise Resource Planning) system in C++. The system is designed to manage student records with support for different universities and course coding systems, parallel data processing, and role-based access control.

## Features

### Core Assignment Requirements (All Implemented)

**1. Template-Based Student Management (Question 1)**
- Generic `Student<RollNoType, CourseCodeType>` template class
- Supports different data types for roll numbers (string, unsigned int, etc.)
- Supports different course identifier types (strings like "OOPD", integers like 101)
- Implements data abstraction and data hiding using private members and public accessors
- Maintains complete student information including names, branches, courses, and grades

**2. Multi-University System (Question 2)**
- `IIITStudentDatabase` class for IIIT-Delhi using string-based course codes (e.g., "OOPD", "DSA")
- `IITStudentDatabase` class for IIT-Delhi using integer-based course codes (e.g., 101, 102)
- Both databases use the same template-based Student class with different template parameters
- Handles both universities' course naming conventions seamlessly

**3. Parallel Sorting with 3000 Records (Question 3)**
- Loads 3000 student records from CSV file (`students_data.csv` pre-generated)
- Implements multi-threaded parallel sorting using `std::thread`
- Uses 2 separate threads: one sorts first half, one sorts second half
- Thread-safe operations using `std::mutex` and `std::lock_guard` (NO race conditions)
- Merges two sorted halves after thread completion
- Logs execution time for each thread and total sorting time in milliseconds
- Thread-safe database access prevents concurrent modification issues

**4. Multiple Iterator Types (Question 4)**
- Insertion order iterator: Uses `std::vector` directly with forward iteration
- Sorted order iterator: Maintains separate sorted vector accessed via public methods
- Provides const accessors to prevent unintended data modifications
- Efficient O(1) random access using vector iterators
- Different traversal patterns without copying data

**5. Fast Grade-Based Lookup (Question 5)**
- Multi-map index: `courseGradeIndex[courseCode] -> vector<Student*>`
- Stores only students with grades >= 9 in specific courses
- Enables O(1) course lookup and quick iteration through qualified students
- `buildCourseGradeIndex()` method to refresh the index
- `getStudentsByGradeAndCourse()` for O(1) access without linear search
- Placement companies can query students with grades >= 9 in specific courses instantly

### Additional Features

**6. Role-Based Access Control (BONUS)**
- Admin Role:
  - Load student data from CSV
  - Add new students to the system
  - Update existing student information
  - Perform parallel sorting operations
  - Query students by grades in specific courses
  - View all students
  - Credentials: `admin` / `admin123`

- Student Role:
  - View students in insertion order (read-only)
  - View students in sorted order (read-only)
  - Search specific students by roll number
  - NO modification capabilities
  - Credentials: `student` / `student123`

**7. Thread-Safe Database Operations**
- All database operations protected with `std::mutex`
- `std::lock_guard` ensures automatic lock release (RAII principle)
- Prevents data corruption during concurrent access
- Multiple threads can safely query student data

**8. CSV File Parsing**
- Custom CSV parser for reading 3000 student records
- Handles comma-separated values with proper field extraction
- Parses complex fields (current courses, previous courses with grades)
- Supports flexible data formats

## Project Structure

```
.
├── Student.h                 # Template class for student records
├── StudentDatabase.h         # Database classes (IIIT & IIT)
├── AuthManager.h            # Authentication and authorization
├── main.cpp                 # Application entry point and UI
├── Makefile                 # Build configuration
├── students_data.csv        # Pre-generated 3000 student records
└── README.md               # This file
```

## File Descriptions

### Student.h
**Template class**: `Student<RollNoType, CourseCodeType>`
- Private members: rollNumber, name, branch, startingYear, currentCourses, previousCourses
- Public methods: Getters/Setters, course management, grade retrieval
- Generic to handle any data type for roll numbers and course codes

### StudentDatabase.h
**Two Database Classes**:
1. `IIITStudentDatabase`: Manages `Student<string, string>` objects
2. `IITStudentDatabase`: Manages `Student<string, int>` objects

**Key Methods**:
- `addStudent()`: Thread-safe student addition
- `updateStudent()`: Admin function to update student info
- `findStudent()`: Search by roll number
- `readFromCSV()`: Load 3000 records from CSV
- `parallelSort()`: Multi-threaded sorting with timing
- `buildCourseGradeIndex()`: Build fast lookup index
- `getStudentsByGradeAndCourse()`: O(1) lookup by grade and course
- `getStudentsInsertionOrder()`: Access in insertion order
- `getSortedStudents()`: Access in sorted order

### AuthManager.h
**Authentication Manager**:
- `UserRole` enum: ADMIN or STUDENT
- `login()`: Authenticate user with role-based login
- `logout()`: End user session
- Permission checking methods: `isAdmin()`, `isStudent()`, `isAuthenticated()`

### main.cpp
**Application UI**:
- Main menu for login selection
- Admin menu with 7 options
- Student menu with 4 view-only options
- Interactive CLI interface
- Input validation and error handling
- User-friendly prompts and feedback

## Compilation and Execution

### Prerequisites
- GCC compiler with C++17 support
- Make utility
- Linux/macOS or Windows with WSL/MinGW

### Build Instructions

**Step 1: Compile the project**
```bash
make
```
This will compile all source files and create the executable `erp_system`.

**Step 2: Run the application**
```bash
./erp_system
```
Or use the convenient target:
```bash
make run
```

**Step 3: Clean build artifacts**
```bash
make clean
```

## Usage Instructions

### Initial Login
1. Choose option 1 for Admin login or option 2 for Student login
2. Default credentials:
   - **Admin**: Username: `admin`, Password: `admin123`
   - **Student**: Username: `student`, Password: `student123`

### Admin Workflow (Example)
1. **Load Data**: Select option 1 in Admin menu
   - Enter filename: `students_data.csv`
   - System loads 3000 pre-generated student records
   
2. **Perform Parallel Sort**: Select option 4
   - System launches 2 threads to sort halves simultaneously
   - Displays timing information for each thread
   - Merges sorted halves efficiently

3. **Query by Grade**: Select option 5
   - Enter course name (e.g., "OOPD")
   - System instantly retrieves students with grade >= 9
   - Uses indexed lookup (NO linear search)

4. **View Students**: Select option 6
   - Displays first 10 students from database
   - Shows roll number, name, branch, year, courses, and grades

### Student Workflow (Example)
1. **View Insertion Order**: Select option 1
   - Displays students in the order they were added

2. **View Sorted Data**: Select option 2
   - Shows students sorted by roll number
   - (Admin must perform sort first)

3. **Search Student**: Select option 3
   - Enter specific roll number
   - System finds and displays matching student

## Compilation Flags Explanation

```
-Wall           : Enable all standard warnings
-Wextra         : Enable extra compiler warnings
-std=c++17      : Use C++17 standard (for std::thread, std::mutex, etc.)
-O2             : Optimize for performance
-g              : Include debugging symbols
-pthread        : Link with pthread library for thread support
```

## Key Design Patterns Used

1. **Template Metaprogramming**: Generic Student class for type flexibility
2. **RAII (Resource Acquisition Is Initialization)**: Mutex locking with lock_guard
3. **Singleton-like Pattern**: Global database instances
4. **Factory Pattern**: CSV file parsing and student creation
5. **Enum-based State Management**: Role-based access control
6. **Thread-Safe Lazy Initialization**: Course grade index built on demand

## Thread Safety Guarantees

- All database operations are protected by mutex locks
- No race conditions in parallel sorting
- Each thread sorts its own partition independently
- Main thread waits for both sorting threads to complete before merging
- Lock guards automatically release locks (exception-safe)

## Scalability Features

- Efficient O(1) course-grade lookup using map-based indexing
- Supports 3000+ student records with minimal memory overhead
- Parallel sorting reduces sorting time for large datasets
- Iterator-based access avoids unnecessary data copying

## Limitations and Future Enhancements

- Credentials are hardcoded (production would use hashed storage)
- CSV parsing assumes well-formed input (production would have robust error handling)
- In-memory database (production would use persistent storage like SQL databases)
- Display limited to first 10 records in UI (pagination not implemented)

## Assignment Compliance Checklist

- ✅ Template class accommodates different data types
- ✅ IIIT-Delhi and IIT-Delhi systems implemented
- ✅ 3000 student records loaded from CSV
- ✅ Parallel sorting with 2+ threads implemented
- ✅ NO race conditions (mutex protected)
- ✅ Thread execution time logged
- ✅ Insertion order iterator implemented
- ✅ Sorted order iterator implemented
- ✅ Fast grade-based lookup without linear search
- ✅ Data abstraction and hiding principles applied
- ✅ README documentation provided
- ✅ Makefile for compilation included
- ✅ Additional admin/student authentication feature

## Contact & Support

For questions about this implementation, refer to the code comments and inline documentation throughout the source files.

---
**Submission Format**: All files included in structured directory with Makefile and README as per assignment requirements.
