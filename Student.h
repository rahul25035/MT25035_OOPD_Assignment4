#ifndef STUDENT_H
#define STUDENT_H

#include <vector>
#include <map>
#include <string>
#include <iostream>

// Template class to accommodate different data types for roll numbers and course codes
template<typename RollNoType, typename CourseCodeType>
class Student {
private:
    RollNoType rollNumber;
    std::string name;
    std::string branch;
    int startingYear;
    std::vector<CourseCodeType> currentCourses;
    // Map: CourseCodeType -> Grade
    std::map<CourseCodeType, int> previousCourses;

public:
    // Constructor
    Student(RollNoType rollNo, const std::string& studentName, 
            const std::string& studentBranch, int year)
        : rollNumber(rollNo), name(studentName), branch(studentBranch), 
          startingYear(year) {}

    // Default constructor
    Student() : rollNumber(RollNoType()), name(""), branch(""), startingYear(0) {}

    // Getters for roll number
    RollNoType getRollNumber() const { return rollNumber; }

    // Getters for name
    std::string getName() const { return name; }

    // Getters for branch
    std::string getBranch() const { return branch; }

    // Getters for starting year
    int getStartingYear() const { return startingYear; }

    // Setters
    void setName(const std::string& studentName) { name = studentName; }
    void setBranch(const std::string& studentBranch) { branch = studentBranch; }
    void setStartingYear(int year) { startingYear = year; }

    // Add current course
    void addCurrentCourse(const CourseCodeType& courseCode) {
        currentCourses.push_back(courseCode);
    }

    // Add previous course with grade
    void addPreviousCourse(const CourseCodeType& courseCode, int grade) {
        previousCourses[courseCode] = grade;
    }

    // Get current courses
    const std::vector<CourseCodeType>& getCurrentCourses() const {
        return currentCourses;
    }

    // Get previous courses
    const std::map<CourseCodeType, int>& getPreviousCourses() const {
        return previousCourses;
    }

    // Get grade for a specific course
    int getGradeInCourse(const CourseCodeType& courseCode) const {
        auto it = previousCourses.find(courseCode);
        if (it != previousCourses.end()) {
            return it->second;
        }
        return -1; // Course not found
    }

    // Comparison operators for sorting
    bool operator<(const Student& other) const {
        return rollNumber < other.rollNumber;
    }

    bool operator>(const Student& other) const {
        return rollNumber > other.rollNumber;
    }

    bool operator==(const Student& other) const {
        return rollNumber == other.rollNumber;
    }
};

#endif // STUDENT_H
