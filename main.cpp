#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <iomanip>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <cctype>
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

// Global managers - FIXED: iitManager now has correct type
IIITStudentManager iiitManager;
IITStudentManager iitManager;  // ✅ FIXED: Was IIITStudentManager, now IITStudentManager
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
        cout << "\nEnter CSV filename (default: students_sample_3000.csv): ";
        getline(cin, filename);

        if (filename.empty()) {
            filename = "students_sample_3000.csv";
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
// PARALLEL SORTING WITH ERROR HANDLING
// ============================================================================

/**
 * Perform parallel sorting with comprehensive error handling
 */
void performParallelSorting() {
    try {
        if (iiitManager.getTotalStudents() == 0) {
            cout << "\n❌ ERROR: No students loaded!" << endl;
            cout << "Load CSV first (Option 1) before sorting." << endl;
            return;
        }

        cout << "\n" << string(70, '=') << endl;
        cout << "Parallel Sorting (Multi-threaded)" << endl;
        cout << "Total students to sort: " << iiitManager.getTotalStudents() << endl;
        cout << string(70, '=') << endl;

        cout << "\nEnter number of threads (2-16, default 2): ";
        int numThreads = getValidatedInteger(2, 16);

        auto& students = iiitManager.getStudents();

        try {
            sortingManager.parallelSort(students, numThreads);
            cout << "\n✓ Sorting completed successfully!" << endl;
            cout << "Check sorting_thread_log.txt for detailed thread statistics." << endl;
        } catch (const exception& e) {
            cerr << "\n❌ ERROR during sorting: " << e.what() << endl;
        }
    } catch (const exception& e) {
        cerr << "\n❌ ERROR: " << e.what() << endl;
    }
}

// ============================================================================
// HIGH-GRADE SEARCH WITH ERROR HANDLING
// ============================================================================

/**
 * Search for high-grade students with error handling
 */
void searchHighGradeStudents() {
    try {
        if (iiitManager.getTotalStudents() == 0) {
            cout << "\n❌ ERROR: No students loaded yet!" << endl;
            cout << "Load CSV first (Option 1) before searching." << endl;
            return;
        }

        cout << "\n" << string(70, '=') << endl;
        cout << "Search High Grade Students" << endl;
        cout << "Grade Scale: 1=D, 2=C, 3=B, 4=A, 5=A+" << endl;
        cout << string(70, '=') << endl;

        cout << "\nEnter minimum grade points (1-10): ";
        int minGrade = getValidatedInteger(1, 10);

        if (minGrade > 10) {
            throw out_of_range("Grade must be between 1 and 10");
        }

        auto results = iiitManager.findHighGradeStudents(minGrade);

        cout << "\n" << string(70, '=') << endl;
        cout << "✓ Search Results" << endl;
        cout << "Found " << results.size() << " students with grade >= " << minGrade << endl;
        cout << string(70, '=') << endl;

        int count = 0;
        for (int idx : results) {
            if (count >= 10) {
                cout << "\n... and " << (results.size() - 10) << " more results" << endl;
                cout << "\nShowing first 10 results. Use smaller minimum grade to see more." << endl;
                break;
            }
            cout << "\n" << count+1 << ". ";
            iiitManager.getStudent(idx).display();
            count++;
        }
    } catch (const out_of_range& e) {
        cerr << "\n❌ ERROR: " << e.what() << endl;
    } catch (const exception& e) {
        cerr << "\n❌ ERROR: " << e.what() << endl;
    }
}

// ============================================================================
// STATISTICS DISPLAY WITH ERROR HANDLING
// ============================================================================

/**
 * Display system statistics with error handling
 */
void displayStatistics() {
    try {
        cout << "\n" << string(70, '=') << endl;
        cout << "System Statistics" << endl;
        cout << string(70, '=') << endl;

        cout << "\nIIIT-Delhi System (String Roll Numbers):" << endl;
        cout << "  Total Students: " << iiitManager.getTotalStudents() << endl;

        cout << "\nIIT-Delhi System (Integer Roll Numbers):" << endl;
        cout << "  Total Students: " << iitManager.getTotalStudents() << endl;

        if (iiitManager.getTotalStudents() == 0 && iitManager.getTotalStudents() == 0) {
            cout << "\n⚠️  WARNING: No students loaded in either system!" << endl;
            cout << "Load students using Option 1 (Load from CSV)" << endl;
        }

        cout << string(70, '=') << endl;
    } catch (const exception& e) {
        cerr << "\n❌ ERROR: " << e.what() << endl;
    }
}

// ============================================================================
// IIIT SYSTEM SUBMENU WITH ERROR HANDLING
// ============================================================================

/**
 * IIIT-Delhi system submenu with exception handling
 */
void displayIIITMenu() {
    while (true) {
        try {
            cout << "\n" << string(70, '=') << endl;
            cout << "IIIT-Delhi System (String Roll Numbers)" << endl;
            cout << "=" << string(68, '=') << endl;
            cout << "1. Add Student" << endl;
            cout << "2. Display Students" << endl;
            cout << "3. Back to Main Menu" << endl;
            cout << string(70, '=') << endl;
            cout << "\nYour choice: ";

            int choice = getValidatedInteger(1, 3);

            if (choice == 1) {
                addStudentIIIT();
            } else if (choice == 2) {
                displayStudentsIIIT();
            } else if (choice == 3) {
                return;
            }
        } catch (const exception& e) {
            cerr << "\n❌ ERROR: " << e.what() << endl;
            cout << "Please try again." << endl;
        }
    }
}

// ============================================================================
// IIT SYSTEM SUBMENU WITH ERROR HANDLING
// ============================================================================

/**
 * IIT-Delhi system submenu with exception handling
 */
void displayIITMenu() {
    while (true) {
        try {
            cout << "\n" << string(70, '=') << endl;
            cout << "IIT-Delhi System (Integer Roll Numbers)" << endl;
            cout << "=" << string(68, '=') << endl;
            cout << "1. Add Student" << endl;
            cout << "2. Display Students" << endl;
            cout << "3. Back to Main Menu" << endl;
            cout << string(70, '=') << endl;
            cout << "\nYour choice: ";

            int choice = getValidatedInteger(1, 3);

            if (choice == 1) {
                addStudentIIT();
            } else if (choice == 2) {
                displayStudentsIIT();
            } else if (choice == 3) {
                return;
            }
        } catch (const exception& e) {
            cerr << "\n❌ ERROR: " << e.what() << endl;
            cout << "Please try again." << endl;
        }
    }
}

// ============================================================================
// MAIN MENU WITH COMPREHENSIVE ERROR HANDLING
// ============================================================================

/**
 * Main menu with full exception handling and input validation
 * Prevents infinite loops from invalid input
 */
void displayMainMenu() {
    while (true) {
        try {
            cout << "\n\n";
            cout << "╔════════════════════════════════════════════════════════════╗" << endl;
            cout << "║           ERP System - Student Management                  ║" << endl;
            cout << "║           Assignment 4: Templates and Threads              ║" << endl;
            cout << "╚════════════════════════════════════════════════════════════╝" << endl;
            cout << "\n1. Load Students from CSV (3000 students)" << endl;
            cout << "2. IIIT-Delhi System (String Roll Numbers)" << endl;
            cout << "3. IIT-Delhi System (Integer Roll Numbers)" << endl;
            cout << "4. Perform Parallel Sorting (Multi-threaded)" << endl;
            cout << "5. Search High Grade Students (Grade >= 9)" << endl;
            cout << "6. Display Statistics" << endl;
            cout << "7. Exit" << endl;
            cout << "\n" << string(70, '-') << endl;
            cout << "Enter your choice (1-7): ";

            int choice = getValidatedInteger(1, 7);

            switch (choice) {
                case 1:
                    loadStudentsFromCSV();
                    break;
                case 2:
                    displayIIITMenu();
                    break;
                case 3:
                    displayIITMenu();
                    break;
                case 4:
                    performParallelSorting();
                    break;
                case 5:
                    searchHighGradeStudents();
                    break;
                case 6:
                    displayStatistics();
                    break;
                case 7:
                    cout << "\nThank you for using the ERP System. Goodbye!" << endl;
                    return;
                default:
                    cerr << "\n❌ ERROR: Invalid choice! Please enter a number between 1 and 7." << endl;
            }
        } catch (const exception& e) {
            cerr << "\n❌ CRITICAL ERROR: " << e.what() << endl;
            cerr << "The program encountered an unexpected error." << endl;
            cout << "Please restart the program." << endl;
            return;
        }
    }
}

// ============================================================================
// MAIN FUNCTION WITH ERROR HANDLING
// ============================================================================

int main() {
    try {
        cout << "\n╔════════════════════════════════════════════════════════════╗" << endl;
        cout << "║  Initializing ERP System with Exception Handling...          ║" << endl;
        cout << "╚════════════════════════════════════════════════════════════╝" << endl;

        displayMainMenu();

        cout << "\nProgram exited successfully." << endl;
        return 0;
    } catch (const bad_alloc& e) {
        cerr << "\n❌ FATAL ERROR: Out of memory - " << e.what() << endl;
        return 2;
    } catch (const exception& e) {
        cerr << "\n❌ FATAL ERROR: " << e.what() << endl;
        return 1;
    } catch (...) {
        cerr << "\n❌ FATAL ERROR: Unknown exception occurred!" << endl;
        return 3;
    }
}
