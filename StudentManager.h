#ifndef STUDENT_MANAGER_H
#define STUDENT_MANAGER_H

#include "Student.h"
#include "Iterator.h"
#include <vector>
#include <algorithm>
#include <iostream>

// Template class to manage collection of students
template<typename RollType, typename CourseType>
class StudentManager {
private:
    std::vector<Student<RollType, CourseType>> students;
    std::vector<int> insertionOrder; // Indices in insertion order
    std::vector<int> sortedOrder;    // Indices in sorted order
    bool isSorted;

public:
    StudentManager() : isSorted(false) {}

    // Add student to manager
    void addStudent(const Student<RollType, CourseType>& student) {
        students.push_back(student);
        insertionOrder.push_back(students.size() - 1);
        isSorted = false;
    }

    // Get student by index
    Student<RollType, CourseType>& getStudent(int index) {
        return students[index];
    }

    const Student<RollType, CourseType>& getStudent(int index) const {
        return students[index];
    }

    // Get total number of students
    size_t getTotalStudents() const {
        return students.size();
    }

    // Sort students
    void sortStudents() {
        sortedOrder = insertionOrder;
        std::sort(sortedOrder.begin(), sortedOrder.end(),
            [this](int a, int b) {
                return students[a] < students[b];
            });
        isSorted = true;
    }

    // Get insertion order iterator
    InsertionOrderIterator<Student<RollType, CourseType>> getInsertionOrderIterator() {
        return InsertionOrderIterator<Student<RollType, CourseType>>(&students, &insertionOrder);
    }

    // Get sorted order iterator
    SortedOrderIterator<Student<RollType, CourseType>> getSortedOrderIterator() {
        if (!isSorted) sortStudents();
        return SortedOrderIterator<Student<RollType, CourseType>>(&students, &sortedOrder);
    }

    // Display all students in insertion order
    void displayInsertionOrder() const {
        std::cout << "\n=== Students in Insertion Order ===" << std::endl;
        for (int idx : insertionOrder) {
            std::cout << idx + 1 << ". ";
            students[idx].display();
        }
    }

    // Display all students in sorted order
    void displaySortedOrder() {
        if (!isSorted) sortStudents();
        std::cout << "\n=== Students in Sorted Order ===" << std::endl;
        for (int idx : sortedOrder) {
            std::cout << idx + 1 << ". ";
            students[idx].display();
        }
    }

    // Find students with high grade in specific course
    std::vector<int> findHighGradeStudents(int minGrade) const {
        std::vector<int> results;
        for (size_t i = 0; i < students.size(); i++) {
            if (students[i].hasGradeAboveInCourse(minGrade)) {
                results.push_back(i);
            }
        }
        return results;
    }

    // Get underlying vector for processing
    std::vector<Student<RollType, CourseType>>& getStudents() {
        return students;
    }

    const std::vector<Student<RollType, CourseType>>& getStudents() const {
        return students;
    }

    // Get sorted order indices (for CSV export)
    const std::vector<int>& getSortedOrderIndices() {
        if (!isSorted) sortStudents();
        return sortedOrder;
    }
};

#endif // STUDENT_MANAGER_H