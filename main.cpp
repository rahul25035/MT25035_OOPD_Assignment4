#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <limits>
#include <iomanip>
#include <ctime>
#include "Student.h"
#include "StudentManager.h"
#include "Iterator.h"
#include "SortingThreads.h"
#include "SearchIndex.h"

using namespace std;

// Type aliases for IIIT-Delhi system (string roll numbers, string course codes)
using IIITStudent = Student<string, IIITCourse>;
using IIITStudentManager = StudentManager<string, IIITCourse>;

// Type aliases for IIT-Delhi system (integer roll numbers, integer course codes)
using IITStudent = Student<unsigned int, IITCourse>;
using IITStudentManager = StudentManager<unsigned int, IITCourse>;

// Global managers
IIITStudentManager iiitManager;
IITStudentManager iitManager;
SortingThreadsManager sortingManager;

// ============================================================================
// UTILITY FUNCTIONS FOR INPUT VALIDATION AND ERROR HANDLING
// ============================================================================

/**
 * Clear the input stream and ignore invalid characters
 * Prevents infinite loops from invalid input
 */
void clearInputStream() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

/**
 * Get a validated integer input from user
 * Throws exception if input is invalid
 * @param minValue: Minimum allowed value
 * @param maxValue: Maximum allowed value
 * @return Valid integer within range
 */
int getValidatedInteger(int minValue, int maxValue) {
    int value;
    bool validInput = false;
    
    while (!validInput) {
        try {
            if (!(cin >> value)) {
                clearInputStream();
                cout << "\n❌ ERROR: Invalid input! Please enter a number." << endl;
                cout << "Enter a number between " << minValue << " and " << maxValue << ": ";
                continue;
            }
            
            if (value < minValue || value > maxValue) {
                cout << "\n❌ ERROR: Input out of range!" << endl;
                cout << "Please enter a number between " << minValue << " and " << maxValue << ": ";
                continue;
            }
            
            validInput = true;
        } catch (const exception& e) {
            clearInputStream();
            cout << "\n❌ ERROR: " << e.what() << endl;
            cout << "Enter a number between " << minValue << " and " << maxValue << ": ";
        }
    }
    
    clearInputStream();
    return value;
}

/**
 * Get a validated unsigned integer input
 * Used for IIT roll numbers
 */
unsigned int getValidatedUnsignedInt(unsigned int minValue, unsigned int maxValue) {
    unsigned int value;
    bool validInput = false;
    
    while (!validInput) {
        try {
            if (!(cin >> value)) {
                clearInputStream();
                cout << "\n❌ ERROR: Invalid input! Please enter a positive number." << endl;
                cout << "Enter a number between " << minValue << " and " << maxValue << ": ";
                continue;
            }
            
            if (value < minValue || value > maxValue) {
                cout << "\n❌ ERROR: Input out of range!" << endl;
                cout << "Please enter a number between " << minValue << " and " << maxValue << ": ";
                continue;
            }
            
            validInput = true;
        } catch (const exception& e) {
            clearInputStream();
            cout << "\n❌ ERROR: " << e.what() << endl;
            cout << "Enter a number between " << minValue << " and " << maxValue << ": ";
        }
    }
    
    clearInputStream();
    return value;
}

/**
 * Get a validated string input (non-empty)
 * Throws exception if input is empty
 */
string getValidatedString(const string& prompt) {
    string value;
    bool validInput = false;
    
    while (!validInput) {
        try {
            cout << prompt;
            if (!getline(cin, value)) {
                clearInputStream();
                cout << "\n❌ ERROR: Failed to read input!" << endl;
                cout << "Please try again." << endl;
                continue;
            }
            
            if (value.empty()) {
                cout << "\n❌ ERROR: Input cannot be empty!" << endl;
                cout << "Please enter a valid value." << endl;
                continue;
            }
            
            // Trim leading/trailing whitespace
            size_t start = value.find_first_not_of(" \t\r\n");
            size_t end = value.find_last_not_of(" \t\r\n");
            
            if (start == string::npos) {
                cout << "\n❌ ERROR: Input cannot be only whitespace!" << endl;
                cout << "Please enter a valid value." << endl;
                continue;
            }
            
            value = value.substr(start, end - start + 1);
            validInput = true;
        } catch (const exception& e) {
            cout << "\n❌ ERROR: " << e.what() << endl;
            cout << "Please try again." << endl;
        }
    }
    
    return value;
}

/**
 * Validate a single character (for grades: A/B/C/D)
 */
char getValidatedGrade() {
    char grade;
    bool validInput = false;
    
    while (!validInput) {
        try {
            cout << "Grade (A/B/C/D): ";
            if (!(cin >> grade)) {
                clearInputStream();
                cout << "❌ ERROR: Invalid input! Please enter a single letter." << endl;
                continue;
            }
            
            grade = toupper(grade);
            
            if (grade != 'A' && grade != 'B' && grade != 'C' && grade != 'D') {
                clearInputStream();
                cout << "❌ ERROR: Grade must be A, B, C, or D!" << endl;
                continue;
            }
            
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            validInput = true;
        } catch (const exception& e) {
            clearInputStream();
            cout << "❌ ERROR: " << e.what() << endl;
        }
    }
    
    return grade;
}

// ============================================================================
// CSV PARSING WITH ERROR HANDLING
// ============================================================================

/**
 * Parse IIIT courses from CSV string with error handling
 * Format: Code:Semester:Grade;Code:Semester:Grade
 */
void parseIIITCourses(const string& coursesStr, vector<IIITCourse>& courses) {
    if (coursesStr.empty()) return;
    
    try {
        stringstream ss(coursesStr);
        string courseStr;
        int lineCount = 0;
        
        while (getline(ss, courseStr, ';')) {
            lineCount++;
            if (courseStr.empty()) continue;
            
            try {
                size_t pos1 = courseStr.find(':');
                size_t pos2 = courseStr.rfind(':');
                
                if (pos1 == string::npos || pos2 == string::npos || pos1 == pos2) {
                    cerr << "⚠️  WARNING: Malformed course entry #" << lineCount << ": " 
                         << courseStr << " (skipping)" << endl;
                    continue;
                }
                
                string code = courseStr.substr(0, pos1);
                
                if (code.empty()) {
                    cerr << "⚠️  WARNING: Empty course code in entry #" << lineCount << " (skipping)" << endl;
                    continue;
                }
                
                int sem = stoi(courseStr.substr(pos1 + 1, pos2 - pos1 - 1));
                char grade = courseStr[pos2 + 1];
                
                if (grade != 'A' && grade != 'B' && grade != 'C' && grade != 'D') {
                    cerr << "⚠️  WARNING: Invalid grade '" << grade << "' in entry #" << lineCount 
                         << " (skipping)" << endl;
                    continue;
                }
                
                courses.push_back(IIITCourse(code, sem, grade));
            } catch (const invalid_argument& e) {
                cerr << "⚠️  WARNING: Invalid course data in entry #" << lineCount 
                     << " - " << e.what() << " (skipping)" << endl;
                continue;
            } catch (const exception& e) {
                cerr << "⚠️  WARNING: Error parsing course entry #" << lineCount 
                     << " - " << e.what() << " (skipping)" << endl;
                continue;
            }
        }
    } catch (const exception& e) {
        cerr << "⚠️  WARNING: Error parsing IIIT courses: " << e.what() << endl;
    }
}

/**
 * Parse IIT courses from CSV string with error handling
 * Format: Code:Grade;Code:Grade
 */
void parseIITCourses(const string& coursesStr, vector<IITCourse>& courses) {
    if (coursesStr.empty()) return;
    
    try {
        stringstream ss(coursesStr);
        string courseStr;
        int lineCount = 0;
        
        while (getline(ss, courseStr, ';')) {
            lineCount++;
            if (courseStr.empty()) continue;
            
            try {
                size_t pos = courseStr.find(':');
                
                if (pos == string::npos) {
                    cerr << "⚠️  WARNING: Malformed IIT course entry #" << lineCount 
                         << ": " << courseStr << " (skipping)" << endl;
                    continue;
                }
                
                int code = stoi(courseStr.substr(0, pos));
                char grade = courseStr[pos + 1];
                
                if (grade != 'A' && grade != 'B' && grade != 'C' && grade != 'D') {
                    cerr << "⚠️  WARNING: Invalid grade '" << grade << "' in IIT entry #" << lineCount 
                         << " (skipping)" << endl;
                    continue;
                }
                
                courses.push_back(IITCourse(code, grade));
            } catch (const invalid_argument& e) {
                cerr << "⚠️  WARNING: Invalid IIT course data in entry #" << lineCount 
                     << " - " << e.what() << " (skipping)" << endl;
                continue;
            } catch (const exception& e) {
                cerr << "⚠️  WARNING: Error parsing IIT course entry #" << lineCount 
                     << " - " << e.what() << " (skipping)" << endl;
                continue;
            }
        }
    } catch (const exception& e) {
        cerr << "⚠️  WARNING: Error parsing IIT courses: " << e.what() << endl;
    }
}

// ============================================================================
// FILE LOADING WITH ERROR HANDLING
// ============================================================================

/**
 * Load students from CSV file with comprehensive error handling
 * Handles missing files, corrupted data, format errors
 */
void loadStudentsFromCSV() {
    string filename;
    
    try {
        cout << "\nEnter CSV filename (default: students.csv): ";
        getline(cin, filename);
        
        if (filename.empty()) {
            filename = "students.csv";
        }
        
        ifstream file(filename);
        
        if (!file.is_open()) {
            throw runtime_error("Could not open file: " + filename);
        }
        
        string line;
        
        if (!getline(file, line)) {
            throw runtime_error("File is empty or cannot be read");
        }
        
        int successCount = 0;
        int errorCount = 0;
        int lineNumber = 1;
        
        while (getline(file, line) && successCount < 3000) {
            lineNumber++;
            
            try {
                if (line.empty()) continue;
                
                stringstream ss(line);
                string rollStr, name, branch, yearStr, iiitCoursesStr, iitCoursesStr;
                
                if (!getline(ss, rollStr, ',')) {
                    throw runtime_error("Missing roll number");
                }
                
                if (!getline(ss, name, ',')) {
                    throw runtime_error("Missing name");
                }
                
                if (!getline(ss, branch, ',')) {
                    throw runtime_error("Missing branch");
                }
                
                if (!getline(ss, yearStr, ',')) {
                    throw runtime_error("Missing start year");
                }
                
                if (!getline(ss, iiitCoursesStr, ',')) {
                    iiitCoursesStr = "";
                }
                
                if (!getline(ss, iitCoursesStr, ',')) {
                    iitCoursesStr = "";
                }
                
                if (rollStr.empty() || name.empty()) {
                    cerr << "⚠️  WARNING: Line " << lineNumber << " - Empty roll or name (skipping)" << endl;
                    errorCount++;
                    continue;
                }
                
                int year;
                try {
                    year = stoi(yearStr);
                    if (year < 1900 || year > 2100) {
                        throw out_of_range("Year out of reasonable range");
                    }
                } catch (const exception& e) {
                    cerr << "⚠️  WARNING: Line " << lineNumber << " - Invalid year '" << yearStr 
                         << "' (skipping)" << endl;
                    errorCount++;
                    continue;
                }
                
                // Add to IIIT system
                try {
                    IIITStudent iiitStudent(rollStr, name, branch, year);
                    vector<IIITCourse> iiitCourses;
                    parseIIITCourses(iiitCoursesStr, iiitCourses);
                    
                    for (const auto& course : iiitCourses) {
                        iiitStudent.addCourse(course);
                    }
                    
                    iiitManager.addStudent(iiitStudent);
                } catch (const exception& e) {
                    cerr << "⚠️  WARNING: Line " << lineNumber << " - Failed to add IIIT student: " 
                         << e.what() << endl;
                    errorCount++;
                }
                
                // Add to IIT system (only if roll number is numeric)
                try {
                    unsigned int rollNum = stoul(rollStr);
                    IITStudent iitStudent(rollNum, name, branch, year);
                    vector<IITCourse> iitCourses;
                    parseIITCourses(iitCoursesStr, iitCourses);
                    
                    for (const auto& course : iitCourses) {
                        iitStudent.addCourse(course);
                    }
                    
                    iitManager.addStudent(iitStudent);
                } catch (const invalid_argument&) {
                    // Skip for non-numeric roll numbers (this is expected)
                } catch (const exception& e) {
                    cerr << "⚠️  WARNING: Line " << lineNumber << " - Failed to add IIT student: " 
                         << e.what() << endl;
                }
                
                successCount++;
            } catch (const exception& e) {
                cerr << "⚠️  WARNING: Line " << lineNumber << " - " << e.what() << " (skipping)" << endl;
                errorCount++;
                continue;
            }
        }
        
        file.close();
        
        cout << "\n" << string(70, '=') << endl;
        cout << "✓ CSV Loading Complete" << endl;
        cout << "  Successfully loaded: " << successCount << " students" << endl;
        cout << "  Errors encountered: " << errorCount << " records skipped" << endl;
        cout << string(70, '=') << endl;
        
        if (successCount == 0) {
            cerr << "\n❌ ERROR: No students were loaded from the CSV file!" << endl;
        }
        
    } catch (const ifstream::failure& e) {
        cerr << "\n❌ FILE ERROR: " << e.what() << endl;
    } catch (const runtime_error& e) {
        cerr << "\n❌ ERROR: " << e.what() << endl;
    } catch (const exception& e) {
        cerr << "\n❌ UNEXPECTED ERROR: " << e.what() << endl;
    }
}

// ============================================================================
// SEARCH FUNCTIONALITY - NEW ADDITION
// ============================================================================

/**
 * Search for a student by roll number in IIIT system
 */
void searchStudentIIIT() {
    try {
        if (iiitManager.getTotalStudents() == 0) {
            cout << "\n❌ ERROR: No IIIT students loaded yet!" << endl;
            cout << "Please load students from CSV first (Option 1)" << endl;
            return;
        }
        
        string rollNumber = getValidatedString("\nEnter roll number to search: ");
        
        bool found = false;
        auto& students = iiitManager.getStudents();
        
        cout << "\n" << string(70, '=') << endl;
        cout << "Search Results for Roll Number: " << rollNumber << endl;
        cout << string(70, '=') << endl;
        
        for (size_t i = 0; i < students.size(); i++) {
            if (students[i].getRollNumber() == rollNumber) {
                cout << "\n✓ Student Found!" << endl;
                cout << "\nStudent Details:" << endl;
                cout << "  Roll Number: " << students[i].getRollNumber() << endl;
                cout << "  Name: " << students[i].getName() << endl;
                cout << "  Branch: " << students[i].getBranch() << endl;
                cout << "  Start Year: " << students[i].getStartYear() << endl;
                cout << "  Total Courses: " << students[i].getCourses().size() << endl;
                
                if (!students[i].getCourses().empty()) {
                    cout << "\n  Courses Taken:" << endl;
                    int courseNum = 1;
                    for (const auto& course : students[i].getCourses()) {
                        cout << "    " << courseNum++ << ". Code: " << course.code 
                             << " | Semester: " << course.semester 
                             << " | Grade: " << course.grade << endl;
                    }
                }
                
                found = true;
                break;
            }
        }
        
        if (!found) {
            cout << "\n❌ No student found with roll number: " << rollNumber << endl;
        }
        
        cout << string(70, '=') << endl;
        
    } catch (const exception& e) {
        cerr << "\n❌ ERROR: " << e.what() << endl;
    }
}

/**
 * Search for a student by roll number in IIT system
 */
void searchStudentIIT() {
    try {
        if (iitManager.getTotalStudents() == 0) {
            cout << "\n❌ ERROR: No IIT students loaded yet!" << endl;
            cout << "Please load students from CSV first (Option 1)" << endl;
            return;
        }
        
        cout << "\nEnter roll number to search (positive integer): ";
        unsigned int rollNumber = getValidatedUnsignedInt(1, 999999);
        
        bool found = false;
        auto& students = iitManager.getStudents();
        
        cout << "\n" << string(70, '=') << endl;
        cout << "Search Results for Roll Number: " << rollNumber << endl;
        cout << string(70, '=') << endl;
        
        for (size_t i = 0; i < students.size(); i++) {
            if (students[i].getRollNumber() == rollNumber) {
                cout << "\n✓ Student Found!" << endl;
                cout << "\nStudent Details:" << endl;
                cout << "  Roll Number: " << students[i].getRollNumber() << endl;
                cout << "  Name: " << students[i].getName() << endl;
                cout << "  Branch: " << students[i].getBranch() << endl;
                cout << "  Start Year: " << students[i].getStartYear() << endl;
                cout << "  Total Courses: " << students[i].getCourses().size() << endl;
                
                if (!students[i].getCourses().empty()) {
                    cout << "\n  Courses Taken:" << endl;
                    int courseNum = 1;
                    for (const auto& course : students[i].getCourses()) {
                        cout << "    " << courseNum++ << ". Code: " << course.code 
                             << " | Grade: " << course.grade << endl;
                    }
                }
                
                found = true;
                break;
            }
        }
        
        if (!found) {
            cout << "\n❌ No student found with roll number: " << rollNumber << endl;
        }
        
        cout << string(70, '=') << endl;
        
    } catch (const exception& e) {
        cerr << "\n❌ ERROR: " << e.what() << endl;
    }
}

// ============================================================================
// IIIT SYSTEM FUNCTIONS WITH ERROR HANDLING
// ============================================================================

/**
 * Add a new student to IIIT system with full validation
 */
void addStudentIIIT() {
    try {
        cout << "\n" << string(70, '=') << endl;
        cout << "Add New IIIT Student (String Roll Numbers)" << endl;
        cout << string(70, '=') << endl;
        
        string roll = getValidatedString("Roll Number (string): ");
        string name = getValidatedString("Name: ");
        string branch = getValidatedString("Branch (CSE/ECE/MTech-CSE/MTech-AI): ");
        
        cout << "Start Year: ";
        int year = getValidatedInteger(1900, 2100);
        
        IIITStudent student(roll, name, branch, year);
        
        cout << "\nNumber of courses: ";
        int numCourses = getValidatedInteger(0, 20);
        
        for (int i = 0; i < numCourses; i++) {
            try {
                cout << "\nCourse " << i+1 << ":" << endl;
                string code = getValidatedString("  Course Code: ");
                cout << "  Semester: ";
                int sem = getValidatedInteger(1, 8);
                char grade = getValidatedGrade();
                
                student.addCourse(IIITCourse(code, sem, grade));
            } catch (const exception& e) {
                cerr << "⚠️  WARNING: Error adding course " << i+1 << " - " << e.what() << endl;
                cout << "Skipping this course..." << endl;
                continue;
            }
        }
        
        iiitManager.addStudent(student);
        cout << "\n✓ Student added successfully!" << endl;
        
    } catch (const exception& e) {
        cerr << "\n❌ ERROR: Failed to add student - " << e.what() << endl;
    }
}

/**
 * Display IIIT students with error handling
 */
void displayStudentsIIIT() {
    try {
        if (iiitManager.getTotalStudents() == 0) {
            cout << "\n❌ ERROR: No IIIT students loaded yet!" << endl;
            cout << "Please load students from CSV first (Option 1)" << endl;
            return;
        }
        
        cout << "\n" << string(70, '=') << endl;
        cout << "Display IIIT Students" << endl;
        cout << "1. Display in Insertion Order" << endl;
        cout << "2. Display in Sorted Order" << endl;
        cout << string(70, '=') << endl;
        cout << "\nYour choice: ";
        
        int choice = getValidatedInteger(1, 2);
        
        if (choice == 1) {
            iiitManager.displayInsertionOrder();
        } else if (choice == 2) {
            iiitManager.displaySortedOrder();
        }
        
    } catch (const exception& e) {
        cerr << "\n❌ ERROR: " << e.what() << endl;
    }
}

// ============================================================================
// IIT SYSTEM FUNCTIONS WITH ERROR HANDLING
// ============================================================================

/**
 * Add a new student to IIT system with full validation
 */
void addStudentIIT() {
    try {
        cout << "\n" << string(70, '=') << endl;
        cout << "Add New IIT Student (Integer Roll Numbers)" << endl;
        cout << string(70, '=') << endl;
        
        cout << "Roll Number (positive integer): ";
        unsigned int roll = getValidatedUnsignedInt(1, 999999);
        
        string name = getValidatedString("Name: ");
        string branch = getValidatedString("Branch (CSE/ECE/MTech-CSE/MTech-AI): ");
        
        cout << "Start Year: ";
        int year = getValidatedInteger(1900, 2100);
        
        IITStudent student(roll, name, branch, year);
        
        cout << "\nNumber of courses: ";
        int numCourses = getValidatedInteger(0, 20);
        
        for (int i = 0; i < numCourses; i++) {
            try {
                cout << "\nCourse " << i+1 << ":" << endl;
                cout << "  Course Code (integer): ";
                int code = getValidatedInteger(1, 9999);
                char grade = getValidatedGrade();
                
                student.addCourse(IITCourse(code, grade));
            } catch (const exception& e) {
                cerr << "⚠️  WARNING: Error adding course " << i+1 << " - " << e.what() << endl;
                cout << "Skipping this course..." << endl;
                continue;
            }
        }
        
        iitManager.addStudent(student);
        cout << "\n✓ Student added successfully!" << endl;
        
    } catch (const exception& e) {
        cerr << "\n❌ ERROR: Failed to add student - " << e.what() << endl;
    }
}

/**
 * Display IIT students with error handling
 */
void displayStudentsIIT() {
    try {
        if (iitManager.getTotalStudents() == 0) {
            cout << "\n❌ ERROR: No IIT students loaded yet!" << endl;
            cout << "Please load students from CSV first (Option 1)" << endl;
            return;
        }
        
        cout << "\n" << string(70, '=') << endl;
        cout << "Display IIT Students" << endl;
        cout << "1. Display in Insertion Order" << endl;
        cout << "2. Display in Sorted Order" << endl;
        cout << string(70, '=') << endl;
        cout << "\nYour choice: ";
        
        int choice = getValidatedInteger(1, 2);
        
        if (choice == 1) {
            iitManager.displayInsertionOrder();
        } else if (choice == 2) {
            iitManager.displaySortedOrder();
        }
        
    } catch (const exception& e) {
        cerr << "\n❌ ERROR: " << e.what() << endl;
    }
}

// ============================================================================
// SAVE SORTED STUDENTS TO CSV - NEW ADDITION
// ============================================================================

/**
 * Save sorted students to CSV file
 */
template<typename RollType, typename CourseType>
void saveSortedToCSV(StudentManager<RollType, CourseType>& manager, const string& filename) {
    try {
        ofstream outFile(filename);
        
        if (!outFile.is_open()) {
            throw runtime_error("Could not create file: " + filename);
        }
        
        // Write header
        outFile << "RollNumber,Name,Branch,StartYear,Courses" << endl;
        
        // Get students in sorted order
        manager.sortStudents();
        auto& students = manager.getStudents();
        auto sortedIterator = manager.getSortedOrderIterator();
        
        size_t count = sortedIterator.getSize();
        for (size_t i = 0; i < count; i++) {
            auto& student = sortedIterator[i];
            
            // Write basic info
            outFile << student.getRollNumber() << ","
                   << student.getName() << ","
                   << student.getBranch() << ","
                   << student.getStartYear() << ",";
            
            // Write courses (simplified format)
            const auto& courses = student.getCourses();
            outFile << courses.size();
            
            outFile << endl;
        }
        
        outFile.close();
        
        cout << "\n✓ Sorted students saved to: " << filename << endl;
        cout << "  Total students saved: " << count << endl;
        
    } catch (const exception& e) {
        cerr << "\n❌ ERROR: Failed to save sorted students - " << e.what() << endl;
    }
}

// ============================================================================
// PARALLEL SORTING WITH ERROR HANDLING AND CSV EXPORT
// ============================================================================

/**
 * Perform parallel sorting on IIIT students with export to CSV
 */
void sortIIITStudents() {
    try {
        if (iiitManager.getTotalStudents() == 0) {
            cout << "\n❌ ERROR: No IIIT students loaded yet!" << endl;
            cout << "Please load students from CSV first" << endl;
            return;
        }
        
        cout << "\n" << string(70, '=') << endl;
        cout << "Parallel Sort - IIIT Students" << endl;
        cout << string(70, '=') << endl;
        
        cout << "Number of threads (2-8): ";
        int numThreads = getValidatedInteger(2, 8);
        
        auto& students = iiitManager.getStudents();
        
        cout << "\nSorting " << students.size() << " IIIT students using " 
             << numThreads << " threads..." << endl;
        
        sortingManager.parallelSort(students, numThreads);
        
        cout << "\n✓ Sorting completed successfully!" << endl;
        
        // Save sorted students to CSV
        string outputFilename = "sorted_iiit_students.csv";
        cout << "\nSaving sorted students to CSV..." << endl;
        saveSortedToCSV(iiitManager, outputFilename);
        
    } catch (const exception& e) {
        cerr << "\n❌ ERROR: Sorting failed - " << e.what() << endl;
    }
}

/**
 * Perform parallel sorting on IIT students with export to CSV
 */
void sortIITStudents() {
    try {
        if (iitManager.getTotalStudents() == 0) {
            cout << "\n❌ ERROR: No IIT students loaded yet!" << endl;
            cout << "Please load students from CSV first" << endl;
            return;
        }
        
        cout << "\n" << string(70, '=') << endl;
        cout << "Parallel Sort - IIT Students" << endl;
        cout << string(70, '=') << endl;
        
        cout << "Number of threads (2-8): ";
        int numThreads = getValidatedInteger(2, 8);
        
        auto& students = iitManager.getStudents();
        
        cout << "\nSorting " << students.size() << " IIT students using " 
             << numThreads << " threads..." << endl;
        
        sortingManager.parallelSort(students, numThreads);
        
        cout << "\n✓ Sorting completed successfully!" << endl;
        
        // Save sorted students to CSV
        string outputFilename = "sorted_iit_students.csv";
        cout << "\nSaving sorted students to CSV..." << endl;
        saveSortedToCSV(iitManager, outputFilename);
        
    } catch (const exception& e) {
        cerr << "\n❌ ERROR: Sorting failed - " << e.what() << endl;
    }
}

// ============================================================================
// SEARCH INDEX DEMONSTRATION
// ============================================================================

/**
 * Demonstrate search index functionality
 */
void demonstrateSearchIndex() {
    try {
        if (iiitManager.getTotalStudents() == 0) {
            cout << "\n❌ ERROR: No students loaded yet!" << endl;
            return;
        }
        
        cout << "\n" << string(70, '=') << endl;
        cout << "Search Index Demo - Find High Performers" << endl;
        cout << string(70, '=') << endl;
        
        SearchIndex<IIITCourse> searchIndex;
        searchIndex.buildIndex(iiitManager.getStudents());
        
        searchIndex.printStatistics();
        
        cout << "\nEnter minimum grade (7-10): ";
        int minGrade = getValidatedInteger(7, 10);
        
        auto results = searchIndex.findAllStudentsByGrade(minGrade);
        
        cout << "\n✓ Found " << results.size() << " students with grade >= " << minGrade << endl;
        
        if (!results.empty() && results.size() <= 10) {
            cout << "\nTop performers:" << endl;
            for (size_t idx : results) {
                iiitManager.getStudent(idx).display();
            }
        }
        
    } catch (const exception& e) {
        cerr << "\n❌ ERROR: " << e.what() << endl;
    }
}

// ============================================================================
// MAIN MENU
// ============================================================================

void displayMainMenu() {
    cout << "\n" << string(70, '=') << endl;
    cout << "        ERP STUDENT MANAGEMENT SYSTEM - MAIN MENU" << endl;
    cout << string(70, '=') << endl;
    cout << "\n📁 FILE OPERATIONS" << endl;
    cout << "  1. Load Students from CSV File" << endl;
    
    cout << "\n👥 IIIT-DELHI SYSTEM (String Roll Numbers)" << endl;
    cout << "  2. Add IIIT Student" << endl;
    cout << "  3. Display IIIT Students" << endl;
    cout << "  4. Search IIIT Student by Roll Number" << endl;
    cout << "  5. Sort IIIT Students (Parallel + Save to CSV)" << endl;
    
    cout << "\n🏛️  IIT-DELHI SYSTEM (Integer Roll Numbers)" << endl;
    cout << "  6. Add IIT Student" << endl;
    cout << "  7. Display IIT Students" << endl;
    cout << "  8. Search IIT Student by Roll Number" << endl;
    cout << "  9. Sort IIT Students (Parallel + Save to CSV)" << endl;
    
    cout << "\n🔍 ADVANCED FEATURES" << endl;
    cout << " 10. Search Index Demo (High Performers)" << endl;
    
    cout << "\n🚪 EXIT" << endl;
    cout << "  0. Exit Program" << endl;
    
    cout << "\n" << string(70, '=') << endl;
}

/**
 * Main program entry point
 */
int main() {
    try {
        cout << "\n╔════════════════════════════════════════════════════════════════════╗" << endl;
        cout << "║          OOPD Assignment 4: Templates & Threads                    ║" << endl;
        cout << "║          Student Management System with Parallel Sorting           ║" << endl;
        cout << "╚════════════════════════════════════════════════════════════════════╝" << endl;
        
        int choice;
        bool running = true;
        
        while (running) {
            try {
                displayMainMenu();
                cout << "\nEnter your choice (0-10): ";
                choice = getValidatedInteger(0, 10);
                
                switch (choice) {
                    case 0:
                        cout << "\n👋 Thank you for using the ERP System!" << endl;
                        cout << "Exiting program..." << endl;
                        running = false;
                        break;
                        
                    case 1:
                        loadStudentsFromCSV();
                        break;
                        
                    case 2:
                        addStudentIIIT();
                        break;
                        
                    case 3:
                        displayStudentsIIIT();
                        break;
                        
                    case 4:
                        searchStudentIIIT();
                        break;
                        
                    case 5:
                        sortIIITStudents();
                        break;
                        
                    case 6:
                        addStudentIIT();
                        break;
                        
                    case 7:
                        displayStudentsIIT();
                        break;
                        
                    case 8:
                        searchStudentIIT();
                        break;
                        
                    case 9:
                        sortIITStudents();
                        break;
                        
                    case 10:
                        demonstrateSearchIndex();
                        break;
                        
                    default:
                        cout << "\n❌ Invalid choice! Please select 0-10." << endl;
                }
                
                if (running && choice != 0) {
                    cout << "\nPress Enter to continue...";
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                
            } catch (const exception& e) {
                cerr << "\n❌ ERROR in menu operation: " << e.what() << endl;
                cout << "\nPress Enter to continue...";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }
        
    } catch (const exception& e) {
        cerr << "\n💥 FATAL ERROR: " << e.what() << endl;
        cerr << "Program will now exit." << endl;
        return 1;
    }
    
    return 0;
}