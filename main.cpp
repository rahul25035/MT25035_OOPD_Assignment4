#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>
#include <chrono>
#include "Student.h"
#include "StudentDatabase.h"
#include "CSVReader.h"
#include "ParallelSort.h"

using namespace std;

// Type aliases for IIIT-Delhi (string course codes)
using IIITStudent = Student<string, string>;
using IIITDatabase = StudentDatabase<string, string>;

// Type aliases for IIT-Delhi (integer course codes)
using IITStudent = Student<string, int>;
using IITDatabase = StudentDatabase<string, int>;

/**
 * @brief Display all students in a given order
 * @param db Database to display
 * @param orderType "Insertion" or "Sorted"
 * @param maxDisplay Maximum students to display (0 = all)
 */
template <typename RollNumberType, typename CourseCodeType>
void displayStudents(const StudentDatabase<RollNumberType, CourseCodeType>& db, 
                     const string& orderType, int maxDisplay = 10) {
    cout << "
========== Students in " << orderType << " Order ==========" << endl;

    int count = 0;

    if (orderType == "Insertion") {
        for (auto it = db.getInsertionOrderIterator(); it != db.getInsertionOrderEnd(); ++it) {
            if (maxDisplay > 0 && count >= maxDisplay) {
                cout << "... (Displaying first " << maxDisplay << " records)" << endl;
                break;
            }

            auto student = *it;
            cout << "\nStudent " << (count + 1) << ":" << endl;
            cout << "  Name: " << student->getName() << endl;
            cout << "  Roll: " << student->getRollNumber() << endl;
            cout << "  Branch: " << student->getBranch() << endl;
            cout << "  Year: " << student->getStartingYear() << endl;

            // Display current courses
            auto currCourses = student->getCurrentCourses();
            if (!currCourses.empty()) {
                cout << "  Current Courses: ";
                for (const auto& [code, name] : currCourses) {
                    cout << code << " ";
                }
                cout << endl;
            }

            count++;
        }
    } else if (orderType == "Sorted") {
        for (auto it = db.getSortedOrderIterator(); it != db.getSortedOrderEnd(); ++it) {
            if (maxDisplay > 0 && count >= maxDisplay) {
                cout << "... (Displaying first " << maxDisplay << " records)" << endl;
                break;
            }

            auto student = it->second;
            cout << "\nStudent " << (count + 1) << ":" << endl;
            cout << "  Name: " << student->getName() << endl;
            cout << "  Roll: " << student->getRollNumber() << endl;
            cout << "  Branch: " << student->getBranch() << endl;
            cout << "  Year: " << student->getStartingYear() << endl;

            // Display current courses
            auto currCourses = student->getCurrentCourses();
            if (!currCourses.empty()) {
                cout << "  Current Courses: ";
                for (const auto& [code, name] : currCourses) {
                    cout << code << " ";
                }
                cout << endl;
            }

            count++;
        }
    }
}

/**
 * @brief Demonstrate Q1 & Q2: Template-based student management for IIIT-Delhi
 */
void demonstrateQ1Q2() {
    cout << "\n" << string(60, '=') << endl;
    cout << "Q1 & Q2: IIIT-Delhi Student Management (String Roll, String Codes)" << endl;
    cout << string(60, '=') << endl;

    IIITDatabase db;

    // Create sample IIIT-Delhi students with string roll numbers and string course codes
    auto student1 = make_shared<IIITStudent>("Rahul Kumar", "2025CSE001", "CSE", 2025);
    student1->addCurrentCourse("COL100", "Introduction to Computer Science");
    student1->addCurrentCourse("COL106", "Data Structures");
    student1->addPreviousCourse("COL331", 8);

    auto student2 = make_shared<IIITStudent>("Priya Singh", "2025CSE002", "CSE", 2025);
    student2->addCurrentCourse("COL100", "Introduction to Computer Science");
    student2->addCurrentCourse("COL216", "OOPD");
    student2->addPreviousCourse("COL106", 9);
    student2->addPreviousCourse("COL331", 7);

    auto student3 = make_shared<IIITStudent>("Arjun Sharma", "2025ECE001", "ECE", 2025);
    student3->addCurrentCourse("COL100", "Introduction to Computer Science");
    student3->addPreviousCourse("COL216", 10);
    student3->addPreviousCourse("COL106", 9);

    db.addStudent(student1);
    db.addStudent(student2);
    db.addStudent(student3);

    cout << "\nAdded " << db.size() << " students with string roll numbers and string course codes" << endl;

    displayStudents(db, "Insertion", 0);
    displayStudents(db, "Sorted", 0);
}

/**
 * @brief Demonstrate Q3: Parallel sorting of 3000 records
 */
void demonstrateQ3() {
    cout << "\n" << string(60, '=') << endl;
    cout << "Q3: Parallel Sorting with Thread Logging" << endl;
    cout << string(60, '=') << endl;

    IIITDatabase db;

    cout << "\nReading 3000 student records from CSV..." << endl;
    CSVReader<string, string> reader("students.csv");
    int recordsRead = reader.readCSV(db);
    cout << "Successfully read " << recordsRead << " records" << endl;

    // Create a vector of student pointers for sorting (by name)
    vector<pair<string, string>> sortData; // (name, rollNumber)

    for (auto it = db.getInsertionOrderIterator(); it != db.getInsertionOrderEnd(); ++it) {
        sortData.push_back({(*it)->getName(), (*it)->getRollNumber()});
    }

    cout << "\nSorting " << sortData.size() << " records using 2 threads..." << endl;

    auto startTotal = chrono::high_resolution_clock::now();

    ParallelSort<pair<string, string>> sorter(sortData, 2);
    sorter.sort();

    auto endTotal = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> totalTime = endTotal - startTotal;

    sorter.logThreadTimes();
    cout << "\nTotal Sorting Time: " << totalTime.count() << " ms" << endl;

    cout << "\nFirst 5 sorted records:" << endl;
    for (int i = 0; i < min(5, (int)sortData.size()); i++) {
        cout << i + 1 << ". " << sortData[i].first << " (" << sortData[i].second << ")" << endl;
    }
}

/**
 * @brief Demonstrate Q4: Different iterators for different orderings
 */
void demonstrateQ4() {
    cout << "\n" << string(60, '=') << endl;
    cout << "Q4: Different Iterators for Different Orders (Without Data Copy)" << endl;
    cout << string(60, '=') << endl;

    // IIIT-Delhi database (string roll, string codes)
    IIITDatabase db1;

    auto s1 = make_shared<IIITStudent>("Zara Malik", "2025CSE010", "CSE", 2025);
    s1->addCurrentCourse("COL216", "OOPD");
    s1->addPreviousCourse("COL106", 8);

    auto s2 = make_shared<IIITStudent>("Aditya Verma", "2025CSE005", "CSE", 2025);
    s2->addCurrentCourse("COL100", "Introduction to Computer Science");
    s2->addPreviousCourse("COL331", 9);

    auto s3 = make_shared<IIITStudent>("Pooja Nair", "2025CSE008", "CSE", 2025);
    s3->addCurrentCourse("COL106", "Data Structures");
    s3->addPreviousCourse("COL216", 7);

    db1.addStudent(s1);
    db1.addStudent(s2);
    db1.addStudent(s3);

    cout << "\nUsing Iterator for Insertion Order:" << endl;
    for (auto it = db1.getInsertionOrderIterator(); it != db1.getInsertionOrderEnd(); ++it) {
        cout << "  - " << (*it)->getName() << endl;
    }

    cout << "\nUsing Iterator for Sorted Order (by Name):" << endl;
    for (auto it = db1.getSortedOrderIterator(); it != db1.getSortedOrderEnd(); ++it) {
        cout << "  - " << it->second->getName() << endl;
    }
}

/**
 * @brief Demonstrate Q5: Quick search by grade without linear search
 */
void demonstrateQ5() {
    cout << "\n" << string(60, '=') << endl;
    cout << "Q5: Quick Search for Students by Grade (Placement Query)" << endl;
    cout << string(60, '=') << endl;

    IIITDatabase db;

    // Create students with grades in COL216 (OOPD course)
    auto s1 = make_shared<IIITStudent>("Rajesh Kumar", "2025CSE001", "CSE", 2025);
    s1->addCurrentCourse("COL216", "OOPD");
    s1->addPreviousCourse("COL216", 10);

    auto s2 = make_shared<IIITStudent>("Neha Singh", "2025CSE002", "CSE", 2025);
    s2->addCurrentCourse("COL216", "OOPD");
    s2->addPreviousCourse("COL216", 9);

    auto s3 = make_shared<IIITStudent>("Vikram Sharma", "2025CSE003", "CSE", 2025);
    s3->addCurrentCourse("COL216", "OOPD");
    s3->addPreviousCourse("COL216", 5);

    auto s4 = make_shared<IIITStudent>("Divya Patel", "2025CSE004", "CSE", 2025);
    s4->addCurrentCourse("COL216", "OOPD");
    s4->addPreviousCourse("COL216", 9);

    db.addStudent(s1);
    db.addStudent(s2);
    db.addStudent(s3);
    db.addStudent(s4);

    cout << "\nSearching for students with grade >= 9 in COL216 (OOPD)..." << endl;
    auto results = db.findStudentsByGrade("COL216", 9);

    cout << "\nFound " << results.size() << " student(s):" << endl;
    for (const auto& student : results) {
        cout << "  - " << student->getName() << " (Roll: " << student->getRollNumber() 
             << ", Grade: " << student->getGradeForCourse("COL216") << ")" << endl;
    }
}

/**
 * @brief Main program
 */
int main() {
    cout << setfill('=') << setw(70) << "=" << endl;
    cout << "Object-Oriented Programming and Design - Assignment 4" << endl;
    cout << "Templates and Threads - University ERP System" << endl;
    cout << setfill('=') << setw(70) << "=" << endl;

    try {
        // Question 1 & 2
        demonstrateQ1Q2();

        // Question 3
        demonstrateQ3();

        // Question 4
        demonstrateQ4();

        // Question 5
        demonstrateQ5();

        cout << "\n" << string(70, '=') << endl;
        cout << "All demonstrations completed successfully!" << endl;
        cout << string(70, '=') << endl;

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
