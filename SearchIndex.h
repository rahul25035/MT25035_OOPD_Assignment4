#ifndef SEARCH_INDEX_H
#define SEARCH_INDEX_H

#include <map>
#include <string>
#include <vector>
#include <memory>

// Fast search index for students with high grades in specific courses
template <typename CourseType>
class SearchIndex {
private:
    // Map: CourseCode -> Vector of (StudentIndex, Grade)
    std::map<std::string, std::vector<std::pair<int, int>>> courseGradeIndex;

public:
    SearchIndex() {}

    // Add student to index
    void addStudent(int studentIndex, const std::string& courseCode, int gradePoints) {
        courseGradeIndex[courseCode].push_back({studentIndex, gradePoints});
    }

    // Find all students with grade >= minGrade in specific course
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

    // Build index from students
    template <typename StudentType>
    void buildIndex(const std::vector<StudentType>& students) {
        for (size_t i = 0; i < students.size(); i++) {
            const auto& courses = students[i].getCourses();
            for (const auto& course : courses) {
                std::string courseCode = getCourseCode(course);
                int gradePoints = course.getGradePoints();
                addStudent(i, courseCode, gradePoints);
            }
        }
    }

    // Get course code from course object (specialized for different course types)
    std::string getCourseCode(const std::string& course) const {
        return course;
    }

    // Get all indexed courses
    std::vector<std::string> getAllCourses() const {
        std::vector<std::string> courses;
        for (const auto& pair : courseGradeIndex) {
            courses.push_back(pair.first);
        }
        return courses;
    }

    // Print index statistics
    void printStatistics() const {
        std::cout << "\n=== Search Index Statistics ===" << std::endl;
        std::cout << "Total Courses Indexed: " << courseGradeIndex.size() << std::endl;
        for (const auto& pair : courseGradeIndex) {
            std::cout << "  Course: " << pair.first << " - Students: " << pair.second.size() << std::endl;
        }
    }
};

#endif // SEARCH_INDEX_H
