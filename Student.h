#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <vector>
#include <map>

/**
 * @class Student
 * @brief Template class to accommodate students with different data types
 * 
 * This template class uses templates to handle students where:
 * - Roll number can be string or unsigned int
 * - Course codes can be strings or integers
 * This enables the same class to be used across different university systems
 */
template <typename RollNumberType, typename CourseCodeType>
class Student {
private:
    std::string name;
    RollNumberType rollNumber;
    std::string branch;
    int startingYear;

    // Current courses: course code -> course name
    std::map<CourseCodeType, std::string> currentCourses;

    // Previous courses: course code -> grade
    std::map<CourseCodeType, int> previousCourses;

public:
    /**
     * @brief Constructor for Student
     * @param n Student name
     * @param rn Roll number (template type)
     * @param b Branch name
     * @param year Starting year
     */
    Student(const std::string& n, const RollNumberType& rn, const std::string& b, int year)
        : name(n), rollNumber(rn), branch(b), startingYear(year) {}

    // Getters
    /**
     * @brief Get student name
     * @return Student name
     */
    std::string getName() const { return name; }

    /**
     * @brief Get roll number
     * @return Roll number of template type
     */
    RollNumberType getRollNumber() const { return rollNumber; }

    /**
     * @brief Get branch
     * @return Branch name
     */
    std::string getBranch() const { return branch; }

    /**
     * @brief Get starting year
     * @return Starting year
     */
    int getStartingYear() const { return startingYear; }

    /**
     * @brief Add a current course
     * @param courseCode Course code (template type)
     * @param courseName Course name
     */
    void addCurrentCourse(const CourseCodeType& courseCode, const std::string& courseName) {
        currentCourses[courseCode] = courseName;
    }

    /**
     * @brief Add a previous course with grade
     * @param courseCode Course code (template type)
     * @param grade Grade obtained
     */
    void addPreviousCourse(const CourseCodeType& courseCode, int grade) {
        previousCourses[courseCode] = grade;
    }

    /**
     * @brief Get all current courses
     * @return Map of current courses
     */
    const std::map<CourseCodeType, std::string>& getCurrentCourses() const {
        return currentCourses;
    }

    /**
     * @brief Get all previous courses with grades
     * @return Map of previous courses with grades
     */
    const std::map<CourseCodeType, int>& getPreviousCourses() const {
        return previousCourses;
    }

    /**
     * @brief Get grade for a specific course
     * @param courseCode Course code
     * @return Grade (-1 if not found)
     */
    int getGradeForCourse(const CourseCodeType& courseCode) const {
        auto it = previousCourses.find(courseCode);
        if (it != previousCourses.end()) {
            return it->second;
        }
        return -1;
    }
};

#endif // STUDENT_H
