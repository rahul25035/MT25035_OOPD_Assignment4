#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>

// Template class to handle different data types for roll numbers and course identifiers
template<typename RollNoType, typename CourseCodeType>
class Student {
private:
    RollNoType rollNumber;
    std::string name;
    std::string branch;
    int startingYear;
    std::vector<CourseCodeType> currentCourses;
    std::map<CourseCodeType, int> previousCourses; // courseCode -> grade
    
public:
    // Constructor
    Student() : startingYear(0) {}
    
    Student(RollNoType rollNo, const std::string& n, const std::string& b, int year)
        : rollNumber(rollNo), name(n), branch(b), startingYear(year) {}
    
    // Getters
    RollNoType getRollNumber() const { return rollNumber; }
    std::string getName() const { return name; }
    std::string getBranch() const { return branch; }
    int getStartingYear() const { return startingYear; }
    
    const std::vector<CourseCodeType>& getCurrentCourses() const { return currentCourses; }
    const std::map<CourseCodeType, int>& getPreviousCourses() const { return previousCourses; }
    
    // Add current course
    void addCurrentCourse(const CourseCodeType& courseCode) {
        if (std::find(currentCourses.begin(), currentCourses.end(), courseCode) == currentCourses.end()) {
            currentCourses.push_back(courseCode);
        }
    }
    
    // Add previous course with grade
    void addPreviousCourse(const CourseCodeType& courseCode, int grade) {
        previousCourses[courseCode] = grade;
    }
    
    // Get grade for a specific course
    int getGradeForCourse(const CourseCodeType& courseCode) const {
        auto it = previousCourses.find(courseCode);
        if (it != previousCourses.end()) {
            return it->second;
        }
        return -1; // Course not found
    }
    
    // Setters for admin operations
    void setName(const std::string& n) { name = n; }
    void setBranch(const std::string& b) { branch = b; }
    void setStartingYear(int year) { startingYear = year; }
    
    // Display student information
    void display() const {
        std::cout << "Roll No: " << rollNumber << " | Name: " << name 
                  << " | Branch: " << branch << " | Year: " << startingYear << std::endl;
        std::cout << "  Current Courses: ";
        for (const auto& course : currentCourses) {
            std::cout << course << " ";
        }
        if (currentCourses.empty()) {
            std::cout << "None";
        }
        std::cout << std::endl;
        std::cout << "  Previous Courses: ";
        for (const auto& p : previousCourses) {
            std::cout << p.first << ":" << p.second << " ";
        }
        if (previousCourses.empty()) {
            std::cout << "None";
        }
        std::cout << std::endl;
    }
};

#endif // STUDENT_H
