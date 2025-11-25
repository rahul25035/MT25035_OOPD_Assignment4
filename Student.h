#ifndef STUDENT_H
#define STUDENT_H

#include <vector>
#include <string>
#include <iostream>

// Course structure definitions BEFORE they're used
struct IIITCourse {
    std::string code;
    int semester;
    char grade;
    
    IIITCourse() : code(""), semester(0), grade('D') {}
    IIITCourse(const std::string& c, int sem, char g) 
        : code(c), semester(sem), grade(g) {}
    
    int getGradePoints() const {
        switch(grade) {
            case 'A': return 10;
            case 'B': return 8;
            case 'C': return 6;
            case 'D': return 4;
            default: return 0;
        }
    }
};

struct IITCourse {
    int code;
    char grade;
    
    IITCourse() : code(0), grade('D') {}
    IITCourse(int c, char g) : code(c), grade(g) {}
    
    int getGradePoints() const {
        switch(grade) {
            case 'A': return 10;
            case 'B': return 8;
            case 'C': return 6;
            case 'D': return 4;
            default: return 0;
        }
    }
};

template<typename RollType, typename CourseType>
class Student {
private:
    RollType rollNumber;
    std::string name;
    std::string branch;
    int startYear;
    std::vector<CourseType> coursesTaken;

public:
    // Constructor
    Student() : rollNumber(RollType()), name(""), branch(""), startYear(0) {}
    
    Student(RollType roll, const std::string& n, const std::string& b, int year)
        : rollNumber(roll), name(n), branch(b), startYear(year) {}

    // Getters
    RollType getRollNumber() const { return rollNumber; }
    std::string getName() const { return name; }
    std::string getBranch() const { return branch; }
    int getStartYear() const { return startYear; }
    const std::vector<CourseType>& getCourses() const { return coursesTaken; }

    // Setters
    void setRollNumber(const RollType& roll) { rollNumber = roll; }
    void setName(const std::string& n) { name = n; }
    void setBranch(const std::string& b) { branch = b; }
    void setStartYear(int year) { startYear = year; }

    // Add course
    void addCourse(const CourseType& course) {
        coursesTaken.push_back(course);
    }

    // Find students with grade >= 9 in specific course
    bool hasGradeAboveInCourse(int minGrade) const {
        for (const auto& course : coursesTaken) {
            if (course.getGradePoints() >= minGrade) {
                return true;
            }
        }
        return false;
    }

    // Display student info
    void display() const {
        std::cout << "Roll: ";
        if (std::is_same<RollType, unsigned int>::value) {
            std::cout << rollNumber;
        } else {
            std::cout << rollNumber;
        }
        std::cout << " | Name: " << name << " | Branch: " << branch
                  << " | Year: " << startYear << " | Courses: " << coursesTaken.size() << std::endl;
    }

    // For sorting by different criteria
    bool operator<(const Student& other) const {
        if (startYear != other.startYear) return startYear < other.startYear;
        return name < other.name;
    }

    bool operator>(const Student& other) const {
        return other < *this;
    }

    bool operator==(const Student& other) const {
        return rollNumber == other.rollNumber;
    }
};

#endif // STUDENT_H
