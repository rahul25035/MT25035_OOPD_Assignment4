#ifndef SEARCH_INDEX_H
#define SEARCH_INDEX_H

#include <map>
#include <vector>
#include <string>
#include <set>
#include <iostream>
#include "Student.h"  // Include Student.h which now has the course definitions

// Fast search index for students with high grades in specific courses
template<typename CourseType>
class SearchIndex {
private:
    // Map: CourseCode -> Vector of (StudentIndex, Grade)
    std::map<std::string, std::vector<std::pair<int, int>>> courseGradeIndex;

public:
    SearchIndex() {}

    // Add student to index - Generic version
    void addStudent(int studentIndex, const std::string& courseCode, int gradePoints) {
        courseGradeIndex[courseCode].push_back({studentIndex, gradePoints});
    }

    // Find all students with grade >= minGrade in specific course
    // Time Complexity: O(k) where k = matching records
    std::vector<int> findStudentsByGradeInCourse(const std::string& courseCode, int minGrade) const {
        std::vector<int> results;
        auto it = courseGradeIndex.find(courseCode);
        if (it != courseGradeIndex.end()) {
            for (const auto& pair : it->second) {
                if (pair.second >= minGrade) {
                    results.push_back(pair.first);
                }
            }
        }
        return results;
    }

    // Find all students with grade >= minGrade across all courses
    // Time Complexity: O(n) where n = total index entries
    std::vector<int> findAllStudentsByGrade(int minGrade) const {
        std::vector<int> results;
        std::set<int> uniqueStudents; // To avoid duplicates
        
        for (const auto& courseMap : courseGradeIndex) {
            for (const auto& pair : courseMap.second) {
                if (pair.second >= minGrade) {
                    uniqueStudents.insert(pair.first);
                }
            }
        }
        
        for (int studentIdx : uniqueStudents) {
            results.push_back(studentIdx);
        }
        
        return results;
    }

    // Build index from students - SPECIALIZED FOR DIFFERENT COURSE TYPES
    template<typename StudentType>
    void buildIndex(const std::vector<StudentType>& students) {
        for (size_t i = 0; i < students.size(); i++) {
            const auto& courses = students[i].getCourses();
            for (const auto& course : courses) {
                std::string courseCode = getCourseCode(course);
                int gradePoints = course.getGradePoints();
                addStudent(static_cast<int>(i), courseCode, gradePoints);
            }
        }
    }

    // Get course code from course object - GENERIC VERSION (handles IIITCourse)
    std::string getCourseCode(const IIITCourse& course) const {
        return course.code;
    }

    // Get course code from course object - SPECIALIZED FOR IITCourse (integer code)
    std::string getCourseCode(const IITCourse& course) const {
        return std::to_string(course.code);
    }

    // Get all indexed courses
    std::vector<std::string> getAllCourses() const {
        std::vector<std::string> courses;
        for (const auto& pair : courseGradeIndex) {
            courses.push_back(pair.first);
        }
        return courses;
    }

    // Clear the index
    void clear() {
        courseGradeIndex.clear();
    }

    // Get number of indexed courses
    size_t getCourseCount() const {
        return courseGradeIndex.size();
    }

    // Get number of students indexed for a specific course
    size_t getStudentCountForCourse(const std::string& courseCode) const {
        auto it = courseGradeIndex.find(courseCode);
        if (it != courseGradeIndex.end()) {
            return it->second.size();
        }
        return 0;
    }

    // Print index statistics
    void printStatistics() const {
        std::cout << "\n=== Search Index Statistics ===" << std::endl;
        std::cout << "Total Courses Indexed: " << courseGradeIndex.size() << std::endl;
        int totalStudentEntries = 0;
        for (const auto& pair : courseGradeIndex) {
            std::cout << "  Course: " << pair.first << " - Students: " << pair.second.size() << std::endl;
            totalStudentEntries += pair.second.size();
        }
        std::cout << "Total Student-Course Entries: " << totalStudentEntries << std::endl;
    }
};

#endif // SEARCH_INDEX_H
