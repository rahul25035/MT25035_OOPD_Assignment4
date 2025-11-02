#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <iomanip>
#include <cmath>
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

// Function declarations
void displayMainMenu();
void displayIIITMenu();
void displayIITMenu();
void loadStudentsFromCSV();
void addStudentIIIT();
void addStudentIIT();
void displayStudentsIIIT();
void displayStudentsIIT();
void performParallelSorting();
void searchHighGradeStudents();
void displayStatistics();

// CSV parsing helper for IIIT courses
void parseIIITCourses(const string& coursesStr, vector<IIITCourse>& courses) {
    if (coursesStr.empty()) return;

    stringstream ss(coursesStr);
    string courseStr;
    while (getline(ss, courseStr, ';')) {
        if (courseStr.empty()) continue;

        size_t pos1 = courseStr.find(':');
        size_t pos2 = courseStr.rfind(':');

        if (pos1 != string::npos && pos2 != string::npos && pos1 != pos2) {
            string code = courseStr.substr(0, pos1);
            int sem = stoi(courseStr.substr(pos1 + 1, pos2 - pos1 - 1));
            char grade = courseStr[pos2 + 1];
            courses.push_back(IIITCourse(code, sem, grade));
        }
    }
}

// CSV parsing helper for IIT courses
void parseIITCourses(const string& coursesStr, vector<IITCourse>& courses) {
    if (coursesStr.empty()) return;

    stringstream ss(coursesStr);
    string courseStr;
    while (getline(ss, courseStr, ';')) {
        if (courseStr.empty()) continue;

        size_t pos = courseStr.find(':');
        if (pos != string::npos) {
            int code = stoi(courseStr.substr(0, pos));
            char grade = courseStr[pos + 1];
            courses.push_back(IITCourse(code, grade));
        }
    }
}

// Load students from CSV file
void loadStudentsFromCSV() {
    string filename;
    cout << "\nEnter CSV filename (default: students_sample_3000.csv): ";
    getline(cin, filename);
    if (filename.empty()) filename = "students_sample_3000.csv";

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return;
    }

    string line;
    getline(file, line); // Skip header

    int count = 0;
    while (getline(file, line) && count < 3000) {
        stringstream ss(line);
        string rollStr, name, branch, yearStr, iiitCoursesStr, iitCoursesStr;

        getline(ss, rollStr, ',');
        getline(ss, name, ',');
        getline(ss, branch, ',');
        getline(ss, yearStr, ',');
        getline(ss, iiitCoursesStr, ',');
        getline(ss, iitCoursesStr, ',');

        if (!rollStr.empty() && !name.empty()) {
            // Add to both systems with different types
            try {
                // IIIT system uses string roll numbers
                IIITStudent iiitStudent(rollStr, name, branch, stoi(yearStr));
                vector<IIITCourse> iiitCourses;
                parseIIITCourses(iiitCoursesStr, iiitCourses);
                for (const auto& course : iiitCourses) {
                    iiitStudent.addCourse(course);
                }
                iiitManager.addStudent(iiitStudent);

                // IIT system uses unsigned int roll numbers
                try {
                    unsigned int rollNum = stoul(rollStr);
                    IITStudent iitStudent(rollNum, name, branch, stoi(yearStr));
                    vector<IITCourse> iitCourses;
                    parseIITCourses(iitCoursesStr, iitCourses);
                    for (const auto& course : iitCourses) {
                        iitStudent.addCourse(course);
                    }
                    iitManager.addStudent(iitStudent);
                } catch (...) {
                    // Skip for non-numeric roll numbers
                }

                count++;
            } catch (...) {
                continue;
            }
        }
    }

    file.close();
    cout << "\n✓ Successfully loaded " << count << " students from " << filename << endl;
}

// Add IIIT student manually
void addStudentIIIT() {
    string roll, name, branch;
    int year;

    cout << "\nEnter IIIT Student Details:" << endl;
    cout << "Roll Number (string): "; getline(cin, roll);
    cout << "Name: "; getline(cin, name);
    cout << "Branch: "; getline(cin, branch);
    cout << "Start Year: "; cin >> year;
    cin.ignore();

    IIITStudent student(roll, name, branch, year);

    // Add courses
    int numCourses;
    cout << "Number of courses: "; cin >> numCourses;
    cin.ignore();

    for (int i = 0; i < numCourses; i++) {
        string code;
        int sem;
        char grade;
        cout << "  Course " << i+1 << " - Code: "; cin >> code;
        cout << "  Semester: "; cin >> sem;
        cout << "  Grade (A/B/C/D): "; cin >> grade;
        cin.ignore();

        student.addCourse(IIITCourse(code, sem, grade));
    }

    iiitManager.addStudent(student);
    cout << "✓ Student added successfully!" << endl;
}

// Add IIT student manually
void addStudentIIT() {
    unsigned int roll;
    string name, branch;
    int year;

    cout << "\nEnter IIT Student Details:" << endl;
    cout << "Roll Number (integer): "; cin >> roll;
    cin.ignore();
    cout << "Name: "; getline(cin, name);
    cout << "Branch: "; getline(cin, branch);
    cout << "Start Year: "; cin >> year;
    cin.ignore();

    IITStudent student(roll, name, branch, year);

    // Add courses
    int numCourses;
    cout << "Number of courses: "; cin >> numCourses;
    cin.ignore();

    for (int i = 0; i < numCourses; i++) {
        int code;
        char grade;
        cout << "  Course " << i+1 << " - Code (integer): "; cin >> code;
        cout << "  Grade (A/B/C/D): "; cin >> grade;
        cin.ignore();

        student.addCourse(IITCourse(code, grade));
    }

    iitManager.addStudent(student);
    cout << "✓ Student added successfully!" << endl;
}

// Display IIIT students
void displayStudentsIIIT() {
    if (iiitManager.getTotalStudents() == 0) {
        cout << "\nNo IIIT students loaded yet!" << endl;
        return;
    }

    int choice;
    cout << "\n1. Display in Insertion Order\n2. Display in Sorted Order\nChoice: ";
    cin >> choice;
    cin.ignore();

    if (choice == 1) {
        iiitManager.displayInsertionOrder();
    } else if (choice == 2) {
        iiitManager.displaySortedOrder();
    }
}

// Display IIT students
void displayStudentsIIT() {
    if (iitManager.getTotalStudents() == 0) {
        cout << "\nNo IIT students loaded yet!" << endl;
        return;
    }

    int choice;
    cout << "\n1. Display in Insertion Order\n2. Display in Sorted Order\nChoice: ";
    cin >> choice;
    cin.ignore();

    if (choice == 1) {
        iitManager.displayInsertionOrder();
    } else if (choice == 2) {
        iitManager.displaySortedOrder();
    }
}

// Parallel sorting
void performParallelSorting() {
    if (iiitManager.getTotalStudents() == 0) {
        cout << "\nNo students loaded! Load CSV first." << endl;
        return;
    }

    int numThreads;
    cout << "\nEnter number of threads (default 2): ";
    cin >> numThreads;
    cin.ignore();

    if (numThreads < 2) numThreads = 2;

    auto& students = iiitManager.getStudents();
    sortingManager.parallelSort(students, numThreads);

    cout << "\n✓ Sorting completed! Check sorting_thread_log.txt for details." << endl;
}

// Search high grade students
void searchHighGradeStudents() {
    if (iiitManager.getTotalStudents() == 0) {
        cout << "\nNo students loaded yet!" << endl;
        return;
    }

    cout << "\n=== Search High Grade Students ===" << endl;
    cout << "Minimum grade: 1=D, 2=C, 3=B, 4=A" << endl;
    int minGrade;
    cout << "Enter minimum grade points (1-10): "; 
    cin >> minGrade;
    cin.ignore();

    auto results = iiitManager.findHighGradeStudents(minGrade);

    cout << "\nFound " << results.size() << " students with grade >= " << minGrade << endl;
    cout << "\nResults (first 10):" << endl;
    int count = 0;
    for (int idx : results) {
        if (count >= 10) {
            cout << "... and " << (results.size() - 10) << " more" << endl;
            break;
        }
        cout << count+1 << ". ";
        iiitManager.getStudent(idx).display();
        count++;
    }
}

// Display statistics
void displayStatistics() {
    cout << "\n=== System Statistics ===" << endl;
    cout << "IIIT-Delhi System:" << endl;
    cout << "  Total Students: " << iiitManager.getTotalStudents() << endl;
    cout << "\nIIT-Delhi System:" << endl;
    cout << "  Total Students: " << iitManager.getTotalStudents() << endl;
}

// IIIT Menu
void displayIIITMenu() {
    while (true) {
        cout << "\n=== IIIT-Delhi System (String Roll Numbers) ===" << endl;
        cout << "1. Add Student\n2. Display Students\n3. Back to Main Menu\nChoice: ";
        int choice;
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: addStudentIIIT(); break;
            case 2: displayStudentsIIIT(); break;
            case 3: return;
            default: cout << "Invalid choice!" << endl;
        }
    }
}

// IIT Menu
void displayIITMenu() {
    while (true) {
        cout << "\n=== IIT-Delhi System (Integer Roll Numbers) ===" << endl;
        cout << "1. Add Student\n2. Display Students\n3. Back to Main Menu\nChoice: ";
        int choice;
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: addStudentIIT(); break;
            case 2: displayStudentsIIT(); break;
            case 3: return;
            default: cout << "Invalid choice!" << endl;
        }
    }
}

// Main Menu
void displayMainMenu() {
    while (true) {
        cout << "\n╔════════════════════════════════════════════════════════════╗" << endl;
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
        cout << "\nEnter your choice: ";

        int choice;
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: loadStudentsFromCSV(); break;
            case 2: displayIIITMenu(); break;
            case 3: displayIITMenu(); break;
            case 4: performParallelSorting(); break;
            case 5: searchHighGradeStudents(); break;
            case 6: displayStatistics(); break;
            case 7: 
                cout << "\nThank you for using the ERP System. Goodbye!" << endl;
                return;
            default: cout << "\nInvalid choice! Please try again." << endl;
        }
    }
}

int main() {
    try {
        displayMainMenu();
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}
