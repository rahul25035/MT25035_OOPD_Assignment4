# OOPD Assignment 4 - Enhanced Student Management System

## New Features Added

### 1. **Student Search by Roll Number** 🔍
- **Menu Options 4 & 8**: Search for students by their roll number
- Works for both IIIT system (string roll numbers) and IIT system (integer roll numbers)
- Displays complete student details including:
  - Roll number, name, branch, start year
  - Total courses taken
  - Detailed course information (code, semester/grade)
- Fast O(n) linear search through student database

### 2. **Save Sorted Students to CSV** 💾
- **Automatically triggered after sorting** (Menu Options 5 & 9)
- Creates two output files:
  - `sorted_iiit_students.csv` - For IIIT students sorted data
  - `sorted_iit_students.csv` - For IIT students sorted data
- CSV format includes: RollNumber, Name, Branch, StartYear, Number of Courses
- Students are saved in the sorted order (by year, then by name)

## Complete Menu Structure

```
📁 FILE OPERATIONS
  1. Load Students from CSV File

👥 IIIT-DELHI SYSTEM (String Roll Numbers)
  2. Add IIIT Student
  3. Display IIIT Students
  4. Search IIIT Student by Roll Number ⭐ NEW
  5. Sort IIIT Students (Parallel + Save to CSV) ⭐ ENHANCED

🏛️ IIT-DELHI SYSTEM (Integer Roll Numbers)
  6. Add IIT Student
  7. Display IIT Students
  8. Search IIT Student by Roll Number ⭐ NEW
  9. Sort IIT Students (Parallel + Save to CSV) ⭐ ENHANCED

🔍 ADVANCED FEATURES
 10. Search Index Demo (High Performers)

🚪 EXIT
  0. Exit Program
```

## How to Build and Run

### Compilation
```bash
make
```

### Run the Program
```bash
make run
```

### Clean Build Artifacts
```bash
make clean
```

## Usage Examples

### Example 1: Search for a Student
1. Load students from CSV (Option 1)
2. Choose Option 4 (for IIIT) or Option 8 (for IIT)
3. Enter the roll number when prompted
4. View the complete student details

### Example 2: Sort and Export to CSV
1. Load students from CSV (Option 1)
2. Choose Option 5 (for IIIT) or Option 9 (for IIT)
3. Enter number of threads (2-8)
4. Wait for sorting to complete
5. Check the output files:
   - `sorted_iiit_students.csv`
   - `sorted_iit_students.csv`

## File Structure

```
.
├── main.cpp                  # Main program with enhanced search & CSV export
├── Student.h                 # Student template class
├── StudentManager.h          # Enhanced manager with search support
├── Iterator.h                # Custom iterators
├── SortingThreads.h          # Parallel sorting with threads
├── SearchIndex.h             # Fast search index
├── Makefile                  # Build configuration
├── students.csv              # Input data file
├── sorted_iiit_students.csv  # Output (generated after sorting)
├── sorted_iit_students.csv   # Output (generated after sorting)
└── sorting_thread_log.txt    # Thread performance log
```

## Key Implementation Details

### Search Functionality
- **Location**: `searchStudentIIIT()` and `searchStudentIIT()` in main.cpp
- **Complexity**: O(n) where n = number of students
- **Method**: Linear search through student vector by roll number
- **Display**: Full student details with formatted course information

### CSV Export After Sorting
- **Location**: `saveSortedToCSV()` template function in main.cpp
- **Triggered**: Automatically after parallel sorting completes
- **Format**: Standard CSV with header row
- **Order**: Students saved in sorted order (year → name)
- **Files**: Separate files for IIIT and IIT systems

## Requirements Met

✅ All original assignment requirements  
✅ **NEW**: Search student by roll number functionality  
✅ **NEW**: Save sorted students to CSV file  
✅ Clean code without (1), (2) suffixes in filenames  
✅ Complete, compilable code ready to copy-paste  

## Compilation Requirements

- **Compiler**: g++ with C++17 support
- **Flags**: `-std=c++17 -Wall -Wextra -O2 -g -pthread`
- **Libraries**: Standard C++ library + pthread for threading

## Testing Notes

1. Ensure `students.csv` is in the same directory
2. Default filename is `students.csv` (can be changed at runtime)
3. Search is case-sensitive for roll numbers
4. Sorted CSV files will be overwritten each time you sort
5. Thread log is appended to `sorting_thread_log.txt`

## Author
OOPD Assignment 4 - 2025
Enhanced with search and CSV export functionality
